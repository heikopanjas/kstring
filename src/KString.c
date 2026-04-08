//////////////////////////////////////////////////////////////////////////////
//
// MIT License
//
// Copyright (c) 2025 Heiko Panjas
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////////

#include "KString.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

//
// KString Implementation - Kraut Strings
// Based on German String research from Umbra/CedarDB
//

// Pointer tagging masks for storage class
#define KSTRING_PTR_MASK     0x3FFFFFFFFFFFFFFFULL  // 62-bit pointer mask
#define KSTRING_CLASS_MASK   0xC000000000000000ULL  // 2-bit storage class mask
#define KSTRING_CLASS_SHIFT  62

// Invalid length marker for error handling
#define KSTRING_INVALID_LENGTH UINT32_MAX

// Memory alignment for optimal performance
#define KSTRING_ALIGNMENT 8

// Thread-local storage platform compatibility
#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

//
// Private Memory Management Functions
//

// Allocate aligned memory using calloc (private function)
// Rounds up Size to next 8-byte boundary for optimal memory access
inline static void* KS_Alloc(size_t Size)
{
    if (0 == Size)
    {
        return NULL;
    }

    // Round up to next 8-byte boundary
    size_t AlignedSize = (Size + KSTRING_ALIGNMENT - 1) & ~(KSTRING_ALIGNMENT - 1);

    // Use calloc for zero-initialized memory (0x00 fill)
    return calloc(1, AlignedSize);
}

// Free memory and set pointer to NULL (private function)
// Takes a pointer to a pointer for safe memory release
inline static void KS_Release(void** ppPtr)
{
    if (NULL != ppPtr && NULL != *ppPtr)
    {
        free(*ppPtr);
        *ppPtr = NULL;
    }
}

//
// Private Helper Functions
//

// Check if string length qualifies for short representation
inline static bool KS_IsShortString(size_t Size)
{
    return Size <= KSTRING_MAX_SHORT_LENGTH;
}

// Extract storage class from tagged pointer
inline static KStringStorageClass KS_GetStorageClass(uint64_t PtrAndClass)
{
    return (KStringStorageClass)((PtrAndClass & KSTRING_CLASS_MASK) >> KSTRING_CLASS_SHIFT);
}

// Extract pointer from tagged pointer
inline static void* KS_GetPointer(uint64_t PtrAndClass)
{
    return (void*)(PtrAndClass & KSTRING_PTR_MASK);
}

// Create tagged pointer with storage class
inline static uint64_t KS_CreateTaggedPointer(void* pPointer, KStringStorageClass StorageClass)
{
    uint64_t Ptr = (uint64_t)pPointer;

    // Validate that pointer fits in 62-bit space (security check)
    if ((Ptr & ~KSTRING_PTR_MASK) != 0)
    {
        // Pointer has bits set in the upper 2 bits, which would be corrupted
        // This is a critical error that should not happen in normal operation
        return 0; // Return invalid tagged pointer
    }

    uint64_t Class = ((uint64_t)StorageClass) << KSTRING_CLASS_SHIFT;
    return (Ptr & KSTRING_PTR_MASK) | Class;
}

// Extract size from Size field (30 bits)
inline static size_t KS_GetSizeFromField(uint32_t SizeField)
{
    return (size_t)(SizeField & KSTRING_SIZE_MASK);
}

// Extract encoding from Size field (upper 2 bits)
inline static KStringEncoding KS_GetEncodingFromField(uint32_t SizeField)
{
    return (KStringEncoding)((SizeField & KSTRING_ENCODING_MASK) >> KSTRING_ENCODING_SHIFT);
}

// Create Size field with size and encoding
inline static uint32_t KS_CreateSizeField(size_t Size, KStringEncoding Encoding)
{
    // Enhanced size validation to prevent truncation and overflow
    if (Size > KSTRING_SIZE_MASK || Size > UINT32_MAX)
    {
        return KSTRING_INVALID_LENGTH; // Size too large
    }

    uint32_t SizeField     = (uint32_t)Size;
    uint32_t EncodingField = ((uint32_t)Encoding) << KSTRING_ENCODING_SHIFT;
    return SizeField | EncodingField;
}

//
// Core Operations
//

KString KStringCreate(const char* pStr, const size_t Size)
{
    // Use UTF-8 as default encoding
    return KStringCreateWithEncoding(pStr, Size, KSTRING_ENCODING_UTF8);
}

KString KStringCreateWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    KString Result;
    Result.Size = KS_CreateSizeField(Size, Encoding);

    if (KSTRING_INVALID_LENGTH == Result.Size)
    {
        return KStringInvalid();
    }

    if (KS_IsShortString(Size))
    {
        // Short string: store inline
        memcpy(Result.Content, pStr, Size);
        // Zero-fill remaining bytes for consistent comparison
        if (Size < KSTRING_MAX_SHORT_LENGTH)
        {
            memset(Result.Content + Size, 0, KSTRING_MAX_SHORT_LENGTH - Size);
        }
    }
    else
    {
        // Long string: allocate memory and store prefix
        char* pData = KS_Alloc(Size + 1); // +1 for null terminator, zero-initialized
        if (NULL == pData)
        {
            return KStringInvalid();
        }

        memcpy(pData, pStr, Size);
        pData[Size] = '\0';

        // Store first 4 characters as prefix (safe copy with bounds checking)
        size_t PrefixSize = (Size >= 4) ? 4 : Size;
        memcpy(Result.LongStr.Prefix, pStr, PrefixSize);
        if (PrefixSize < 4)
        {
            memset(Result.LongStr.Prefix + PrefixSize, 0, 4 - PrefixSize);
        }

        // Create tagged pointer with TEMPORARY storage class
        Result.LongStr.PtrAndClass = KS_CreateTaggedPointer(pData, KSTRING_TEMPORARY);

        // Validate tagged pointer creation (security check)
        if (0 == Result.LongStr.PtrAndClass)
        {
            KS_Release((void**)&pData);
            return KStringInvalid();
        }
    }

    return Result;
}

KString KStringCreatePersistent(const char* pStr, const size_t Size)
{
    // Use UTF-8 as default encoding
    return KStringCreatePersistentWithEncoding(pStr, Size, KSTRING_ENCODING_UTF8);
}

KString KStringCreatePersistentWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    KString Result;
    Result.Size = KS_CreateSizeField(Size, Encoding);

    if (KSTRING_INVALID_LENGTH == Result.Size)
    {
        return KStringInvalid();
    }

    if (KS_IsShortString(Size))
    {
        // Short strings are always persistent (no pointer needed)
        memcpy(Result.Content, pStr, Size);
        if (Size < KSTRING_MAX_SHORT_LENGTH)
        {
            memset(Result.Content + Size, 0, KSTRING_MAX_SHORT_LENGTH - Size);
        }
    }
    else
    {
        // Long persistent string: point directly to source (no allocation)
        // Store first 4 characters as prefix (safe copy with bounds checking)
        size_t PrefixSize = (Size >= 4) ? 4 : Size;
        memcpy(Result.LongStr.Prefix, pStr, PrefixSize);
        if (PrefixSize < 4)
        {
            memset(Result.LongStr.Prefix + PrefixSize, 0, 4 - PrefixSize);
        }
        Result.LongStr.PtrAndClass = KS_CreateTaggedPointer((void*)pStr, KSTRING_PERSISTENT);
    }

    return Result;
}

KString KStringCreateTransient(const char* pStr, const size_t Size)
{
    // Use UTF-8 as default encoding
    return KStringCreateTransientWithEncoding(pStr, Size, KSTRING_ENCODING_UTF8);
}

KString KStringCreateTransientWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    KString Result;
    Result.Size = KS_CreateSizeField(Size, Encoding);

    if (KSTRING_INVALID_LENGTH == Result.Size)
    {
        return KStringInvalid();
    }

    if (KS_IsShortString(Size))
    {
        // Short string: copy inline
        memcpy(Result.Content, pStr, Size);
        if (Size < KSTRING_MAX_SHORT_LENGTH)
        {
            memset(Result.Content + Size, 0, KSTRING_MAX_SHORT_LENGTH - Size);
        }
    }
    else
    {
        // Long transient string: point directly to source
        // Store first 4 characters as prefix (safe copy with bounds checking)
        size_t PrefixSize = (Size >= 4) ? 4 : Size;
        memcpy(Result.LongStr.Prefix, pStr, PrefixSize);
        if (PrefixSize < 4)
        {
            memset(Result.LongStr.Prefix + PrefixSize, 0, 4 - PrefixSize);
        }
        Result.LongStr.PtrAndClass = KS_CreateTaggedPointer((void*)pStr, KSTRING_TRANSIENT);
    }

    return Result;
}

//
// Convenience Functions for Null-Terminated Strings (less secure)
//

KString KStringCreateFromCStr(const char* pStr)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    size_t Size = strlen(pStr);
    return KStringCreate(pStr, Size);
}

KString KStringCreatePersistentFromCStr(const char* pStr)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    size_t Size = strlen(pStr);
    return KStringCreatePersistent(pStr, Size);
}

void KStringDestroy(const KString Str)
{
    // Only temporary long strings need cleanup
    if (false == KStringIsShort(Str))
    {
        KStringStorageClass StorageClass = KS_GetStorageClass(Str.LongStr.PtrAndClass);
        if (KSTRING_TEMPORARY == StorageClass)
        {
            void* pData = KS_GetPointer(Str.LongStr.PtrAndClass);
            KS_Release((void**)&pData);
        }
    }
}

//
// Access Operations
//

const char* KStringCStr(const KString Str)
{
    if (false == KStringIsValid(Str))
    {
        return NULL;
    }

    if (KStringIsShort(Str))
    {
        // For short strings, we need to return a null-terminated copy
        // Use rotating buffers to handle multiple calls in one expression
        static THREAD_LOCAL char ShortBuffers[4][KSTRING_MAX_SHORT_LENGTH + 1];
        static THREAD_LOCAL int  BufferIndex = 0;

        char* Buffer = ShortBuffers[BufferIndex];
        BufferIndex  = (BufferIndex + 1) % 4;

        size_t Size = KS_GetSizeFromField(Str.Size);
        memcpy(Buffer, Str.Content, Size);
        Buffer[Size] = '\0';
        return Buffer;
    }
    else
    {
        // Long strings are already null-terminated
        return (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);
    }
}

size_t KStringSize(const KString Str)
{
    return KStringIsValid(Str) ? KS_GetSizeFromField(Str.Size) : 0;
}

KStringEncoding KStringGetEncoding(const KString Str)
{
    return KStringIsValid(Str) ? KS_GetEncodingFromField(Str.Size) : KSTRING_ENCODING_UTF8;
}

bool KStringIsShort(const KString Str)
{
    return KS_IsShortString(KS_GetSizeFromField(Str.Size));
}

//
// Comparison Operations
//

int KStringCompare(const KString StrA, const KString StrB)
{
    // Extract actual sizes for comparison
    size_t SizeA = KS_GetSizeFromField(StrA.Size);
    size_t SizeB = KS_GetSizeFromField(StrB.Size);

    // Fast path: compare lengths first
    if (SizeA != SizeB)
    {
        return (SizeA < SizeB) ? -1 : 1;
    }

    // Same length - compare content
    if (true == KStringIsShort(StrA) && true == KStringIsShort(StrB))
    {
        // Both short: direct memory comparison
        return memcmp(StrA.Content, StrB.Content, SizeA);
    }
    else if (true == KStringIsShort(StrA) || true == KStringIsShort(StrB))
    {
        // One short, one long: get actual data
        const char* pDataA = (true == KStringIsShort(StrA)) ? StrA.Content : (const char*)KS_GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (true == KStringIsShort(StrB)) ? StrB.Content : (const char*)KS_GetPointer(StrB.LongStr.PtrAndClass);
        return memcmp(pDataA, pDataB, SizeA);
    }
    else
    {
        // Both long: compare prefixes first (fast path)
        int PrefixCmp = memcmp(StrA.LongStr.Prefix, StrB.LongStr.Prefix, 4);
        if (0 != PrefixCmp)
        {
            return PrefixCmp;
        }

        // Prefixes match: compare full strings
        const char* pDataA = (const char*)KS_GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (const char*)KS_GetPointer(StrB.LongStr.PtrAndClass);
        return memcmp(pDataA, pDataB, SizeA);
    }
}

bool KStringEquals(const KString StrA, const KString StrB)
{
    return KStringCompare(StrA, StrB) == 0;
}

bool KStringStartsWith(const KString Str, const KString Prefix)
{
    // Extract actual sizes using helper functions (security fix)
    size_t StrSize    = KS_GetSizeFromField(Str.Size);
    size_t PrefixSize = KS_GetSizeFromField(Prefix.Size);

    if (PrefixSize > StrSize)
    {
        return false;
    }

    if (0 == PrefixSize)
    {
        return true;
    }

    // Compare prefixes efficiently
    if (true == KStringIsShort(Str) && true == KStringIsShort(Prefix))
    {
        return memcmp(Str.Content, Prefix.Content, PrefixSize) == 0;
    }
    else if (true == KStringIsShort(Str))
    {
        const char* pPrefixData = (true == KStringIsShort(Prefix)) ? Prefix.Content : (const char*)KS_GetPointer(Prefix.LongStr.PtrAndClass);
        return memcmp(Str.Content, pPrefixData, PrefixSize) == 0;
    }
    else if (true == KStringIsShort(Prefix))
    {
        // Str is long, Prefix is short: check against stored prefix first
        if (PrefixSize <= 4)
        {
            return memcmp(Str.LongStr.Prefix, Prefix.Content, PrefixSize) == 0;
        }
        else
        {
            const char* pStrData = (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);
            return memcmp(pStrData, Prefix.Content, PrefixSize) == 0;
        }
    }
    else
    {
        // Both long: use stored prefixes for fast comparison
        if (PrefixSize <= 4)
        {
            return memcmp(Str.LongStr.Prefix, Prefix.LongStr.Prefix, PrefixSize) == 0;
        }
        else
        {
            // Prefix is longer than 4 chars: need full comparison
            const char* pStrData    = (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);
            const char* pPrefixData = (const char*)KS_GetPointer(Prefix.LongStr.PtrAndClass);
            return memcmp(pStrData, pPrefixData, PrefixSize) == 0;
        }
    }
}

//
// Private Helper Functions for Case-Insensitive Operations
//

// Convert character to lowercase (ASCII only for performance)
inline static char KS_ToLowerAscii(char c)
{
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

// Case-insensitive memory comparison (ASCII only)
inline static int KS_MemcmpIgnoreCase(const char* pStr1, const char* pStr2, size_t Size)
{
    for (size_t i = 0; i < Size; ++i)
    {
        char c1 = KS_ToLowerAscii(pStr1[i]);
        char c2 = KS_ToLowerAscii(pStr2[i]);
        if (c1 != c2)
        {
            return (c1 < c2) ? -1 : 1;
        }
    }
    return 0;
}

//
// Case-Insensitive Comparison Operations
//

int KStringCompareIgnoreCase(const KString StrA, const KString StrB)
{
    // Fast path: compare lengths first
    if (StrA.Size != StrB.Size)
    {
        return (StrA.Size < StrB.Size) ? -1 : 1;
    }

    // Same length - compare content case-insensitively
    if (true == KStringIsShort(StrA) && true == KStringIsShort(StrB))
    {
        // Both short: direct memory comparison
        return KS_MemcmpIgnoreCase(StrA.Content, StrB.Content, StrA.Size);
    }
    else if (true == KStringIsShort(StrA) || true == KStringIsShort(StrB))
    {
        // One short, one long: get actual data
        const char* pDataA = (true == KStringIsShort(StrA)) ? StrA.Content : (const char*)KS_GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (true == KStringIsShort(StrB)) ? StrB.Content : (const char*)KS_GetPointer(StrB.LongStr.PtrAndClass);
        return KS_MemcmpIgnoreCase(pDataA, pDataB, StrA.Size);
    }
    else
    {
        // Both long: compare prefixes first (fast path)
        int PrefixCmp = KS_MemcmpIgnoreCase(StrA.LongStr.Prefix, StrB.LongStr.Prefix, 4);
        if (0 != PrefixCmp)
        {
            return PrefixCmp;
        }

        // Prefixes match: compare full strings
        const char* pDataA = (const char*)KS_GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (const char*)KS_GetPointer(StrB.LongStr.PtrAndClass);
        return KS_MemcmpIgnoreCase(pDataA, pDataB, StrA.Size);
    }
}

bool KStringEqualsIgnoreCase(const KString StrA, const KString StrB)
{
    return 0 == KStringCompareIgnoreCase(StrA, StrB);
}

bool KStringStartsWithIgnoreCase(const KString Str, const KString Prefix)
{
    // Extract actual sizes using helper functions (security fix)
    size_t StrSize    = KS_GetSizeFromField(Str.Size);
    size_t PrefixSize = KS_GetSizeFromField(Prefix.Size);

    if (PrefixSize > StrSize)
    {
        return false;
    }

    if (0 == PrefixSize)
    {
        return true;
    }

    // Compare prefixes efficiently (case-insensitive)
    if (true == KStringIsShort(Str) && true == KStringIsShort(Prefix))
    {
        return 0 == KS_MemcmpIgnoreCase(Str.Content, Prefix.Content, PrefixSize);
    }
    else if (true == KStringIsShort(Str))
    {
        const char* pPrefixData = (true == KStringIsShort(Prefix)) ? Prefix.Content : (const char*)KS_GetPointer(Prefix.LongStr.PtrAndClass);
        return 0 == KS_MemcmpIgnoreCase(Str.Content, pPrefixData, PrefixSize);
    }
    else if (true == KStringIsShort(Prefix))
    {
        // Str is long, Prefix is short: check against stored prefix first
        if (PrefixSize <= 4)
        {
            return 0 == KS_MemcmpIgnoreCase(Str.LongStr.Prefix, Prefix.Content, PrefixSize);
        }
        else
        {
            const char* pStrData = (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);
            return 0 == KS_MemcmpIgnoreCase(pStrData, Prefix.Content, PrefixSize);
        }
    }
    else
    {
        // Both long: use stored prefixes for fast comparison
        if (PrefixSize <= 4)
        {
            return 0 == KS_MemcmpIgnoreCase(Str.LongStr.Prefix, Prefix.LongStr.Prefix, PrefixSize);
        }
        else
        {
            // Prefix is longer than 4 chars: need full comparison
            const char* pStrData    = (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);
            const char* pPrefixData = (const char*)KS_GetPointer(Prefix.LongStr.PtrAndClass);
            return 0 == KS_MemcmpIgnoreCase(pStrData, pPrefixData, PrefixSize);
        }
    }
}

//
// String Operations
//

KString KStringConcat(const KString StrA, const KString StrB)
{
    if (false == KStringIsValid(StrA) || false == KStringIsValid(StrB))
    {
        return KStringInvalid();
    }

    size_t SizeA = KS_GetSizeFromField(StrA.Size);
    size_t SizeB = KS_GetSizeFromField(StrB.Size);

    // Check for arithmetic overflow in addition (security check)
    if (SizeA > SIZE_MAX - SizeB)
    {
        return KStringInvalid(); // Overflow would occur
    }

    size_t TotalLength = SizeA + SizeB;

    if (TotalLength > KSTRING_SIZE_MASK)
    {
        return KStringInvalid();
    }

    // Use encoding from first string
    KStringEncoding Encoding = KS_GetEncodingFromField(StrA.Size);

    // Allocate buffer for concatenated string
    char* pBuffer = KS_Alloc(TotalLength + 1); // Zero-initialized
    if (NULL == pBuffer)
    {
        return KStringInvalid();
    }

    // Copy data from both strings
    const char* pDataA = (true == KStringIsShort(StrA)) ? StrA.Content : (const char*)KS_GetPointer(StrA.LongStr.PtrAndClass);
    const char* pDataB = (true == KStringIsShort(StrB)) ? StrB.Content : (const char*)KS_GetPointer(StrB.LongStr.PtrAndClass);

    memcpy(pBuffer, pDataA, SizeA);
    memcpy(pBuffer + SizeA, pDataB, SizeB);
    pBuffer[TotalLength] = '\0';

    // Create result KString with encoding from first string
    KString Result;
    Result.Size = KS_CreateSizeField(TotalLength, Encoding);

    if (KSTRING_INVALID_LENGTH == Result.Size)
    {
        KS_Release((void**)&pBuffer);
        return KStringInvalid();
    }

    if (KS_IsShortString(TotalLength))
    {
        // Result fits in short string
        memcpy(Result.Content, pBuffer, TotalLength);
        if (TotalLength < KSTRING_MAX_SHORT_LENGTH)
        {
            memset(Result.Content + TotalLength, 0, KSTRING_MAX_SHORT_LENGTH - TotalLength);
        }
        KS_Release((void**)&pBuffer); // Don't need allocated buffer
    }
    else
    {
        // Result is long string
        memcpy(Result.LongStr.Prefix, pBuffer, 4);
        Result.LongStr.PtrAndClass = KS_CreateTaggedPointer(pBuffer, KSTRING_TEMPORARY);
    }

    return Result;
}

KString KStringSubstring(const KString Str, const size_t Offset, const size_t Size)
{
    if (false == KStringIsValid(Str))
    {
        return KStringInvalid();
    }

    size_t          StrSize  = KS_GetSizeFromField(Str.Size);
    KStringEncoding Encoding = KS_GetEncodingFromField(Str.Size);

    if (Offset >= StrSize)
    {
        return KStringInvalid();
    }

    // Create local copy for modification
    size_t LocalSize = Size;

    // Clamp size to available characters
    if (Offset + LocalSize > StrSize)
    {
        LocalSize = StrSize - Offset;
    }

    if (LocalSize > KSTRING_SIZE_MASK)
    {
        return KStringInvalid();
    }

    const char* pSourceData = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);

    KString Result;
    Result.Size = KS_CreateSizeField(LocalSize, Encoding);

    if (KSTRING_INVALID_LENGTH == Result.Size)
    {
        return KStringInvalid();
    }

    if (KS_IsShortString(LocalSize))
    {
        // Result fits in short string
        memcpy(Result.Content, pSourceData + Offset, LocalSize);
        if (LocalSize < KSTRING_MAX_SHORT_LENGTH)
        {
            memset(Result.Content + LocalSize, 0, KSTRING_MAX_SHORT_LENGTH - LocalSize);
        }
    }
    else
    {
        // Result requires long string
        char* pBuffer = KS_Alloc(LocalSize + 1); // Zero-initialized
        if (NULL == pBuffer)
        {
            return KStringInvalid();
        }

        memcpy(pBuffer, pSourceData + Offset, LocalSize);
        pBuffer[LocalSize] = '\0';

        memcpy(Result.LongStr.Prefix, pBuffer, 4);
        Result.LongStr.PtrAndClass = KS_CreateTaggedPointer(pBuffer, KSTRING_TEMPORARY);
    }

    return Result;
}

//
// Encoding Conversion Operations
//

// Helper function: Convert UTF-8 to UTF-16 (Little Endian)
static size_t KS_ConvertUtf8ToUtf16Le(const char* pUtf8, size_t Utf8Size, uint16_t* pUtf16, size_t MaxUtf16Size)
{
    size_t Utf16Count = 0;
    size_t i          = 0;

    while (i < Utf8Size && Utf16Count < MaxUtf16Size - 1)
    {
        uint32_t CodePoint = 0;
        size_t   BytesRead = 0;

        uint8_t FirstByte = (uint8_t)pUtf8[i];

        if (FirstByte < 0x80)
        {
            // 1-byte character (ASCII)
            CodePoint = FirstByte;
            BytesRead = 1;
        }
        else if ((FirstByte & 0xE0) == 0xC0)
        {
            // 2-byte character
            if (i + 1 < Utf8Size)
            {
                CodePoint = ((FirstByte & 0x1F) << 6) | (pUtf8[i + 1] & 0x3F);
                BytesRead = 2;
            }
            else
            {
                break;
            }
        }
        else if ((FirstByte & 0xF0) == 0xE0)
        {
            // 3-byte character
            if (i + 2 < Utf8Size)
            {
                CodePoint = ((FirstByte & 0x0F) << 12) | ((pUtf8[i + 1] & 0x3F) << 6) | (pUtf8[i + 2] & 0x3F);
                BytesRead = 3;
            }
            else
            {
                break;
            }
        }
        else if ((FirstByte & 0xF8) == 0xF0)
        {
            // 4-byte character (surrogate pair in UTF-16)
            if (i + 3 < Utf8Size)
            {
                CodePoint = ((FirstByte & 0x07) << 18) | ((pUtf8[i + 1] & 0x3F) << 12) | ((pUtf8[i + 2] & 0x3F) << 6) | (pUtf8[i + 3] & 0x3F);
                BytesRead = 4;

                // Convert to surrogate pair
                if (CodePoint > 0xFFFF && Utf16Count < MaxUtf16Size - 2)
                {
                    CodePoint            -= 0x10000;
                    pUtf16[Utf16Count++]  = (uint16_t)(0xD800 + (CodePoint >> 10));
                    pUtf16[Utf16Count++]  = (uint16_t)(0xDC00 + (CodePoint & 0x3FF));
                }
                else if (CodePoint <= 0xFFFF)
                {
                    pUtf16[Utf16Count++] = (uint16_t)CodePoint;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            // Invalid UTF-8 sequence, skip
            BytesRead = 1;
        }

        if (BytesRead < 4 && CodePoint <= 0xFFFF)
        {
            pUtf16[Utf16Count++] = (uint16_t)CodePoint;
        }

        i += BytesRead;
    }

    return Utf16Count;
}

// Helper function: Convert UTF-16 (Little Endian) to UTF-8
static size_t KS_ConvertUtf16LeToUtf8(const uint16_t* pUtf16, size_t Utf16Size, char* pUtf8, size_t MaxUtf8Size)
{
    size_t Utf8Count = 0;
    size_t i         = 0;

    while (i < Utf16Size && Utf8Count < MaxUtf8Size - 4)
    {
        uint32_t CodePoint = pUtf16[i];

        // Handle surrogate pairs
        if (CodePoint >= 0xD800 && CodePoint <= 0xDBFF && i + 1 < Utf16Size)
        {
            uint32_t LowSurrogate = pUtf16[i + 1];
            if (LowSurrogate >= 0xDC00 && LowSurrogate <= 0xDFFF)
            {
                CodePoint  = 0x10000 + ((CodePoint - 0xD800) << 10) + (LowSurrogate - 0xDC00);
                i         += 2;
            }
            else
            {
                i++;
                continue; // Invalid surrogate pair
            }
        }
        else
        {
            i++;
        }

        // Convert code point to UTF-8
        if (CodePoint < 0x80)
        {
            pUtf8[Utf8Count++] = (char)CodePoint;
        }
        else if (CodePoint < 0x800)
        {
            pUtf8[Utf8Count++] = (char)(0xC0 | (CodePoint >> 6));
            pUtf8[Utf8Count++] = (char)(0x80 | (CodePoint & 0x3F));
        }
        else if (CodePoint < 0x10000)
        {
            pUtf8[Utf8Count++] = (char)(0xE0 | (CodePoint >> 12));
            pUtf8[Utf8Count++] = (char)(0x80 | ((CodePoint >> 6) & 0x3F));
            pUtf8[Utf8Count++] = (char)(0x80 | (CodePoint & 0x3F));
        }
        else if (CodePoint < 0x110000)
        {
            pUtf8[Utf8Count++] = (char)(0xF0 | (CodePoint >> 18));
            pUtf8[Utf8Count++] = (char)(0x80 | ((CodePoint >> 12) & 0x3F));
            pUtf8[Utf8Count++] = (char)(0x80 | ((CodePoint >> 6) & 0x3F));
            pUtf8[Utf8Count++] = (char)(0x80 | (CodePoint & 0x3F));
        }
    }

    return Utf8Count;
}

// Helper function: Convert UTF-8 to ANSI (Windows-1252)
static size_t KS_ConvertUtf8ToAnsi(const char* pUtf8, size_t Utf8Size, char* pAnsi, size_t MaxAnsiSize)
{
    size_t AnsiCount = 0;
    size_t i         = 0;

    // Simple lookup table for common UTF-8 -> Windows-1252 conversions
    // This is a simplified version - a complete implementation would need full mapping tables
    while (i < Utf8Size && AnsiCount < MaxAnsiSize - 1)
    {
        uint8_t FirstByte = (uint8_t)pUtf8[i];

        if (FirstByte < 0x80)
        {
            // ASCII characters map directly
            pAnsi[AnsiCount++] = pUtf8[i];
            i++;
        }
        else if ((FirstByte & 0xE0) == 0xC0 && i + 1 < Utf8Size)
        {
            // 2-byte UTF-8 character
            uint32_t CodePoint = ((FirstByte & 0x1F) << 6) | (pUtf8[i + 1] & 0x3F);

            // Map common characters to Windows-1252
            if (CodePoint <= 0xFF)
            {
                pAnsi[AnsiCount++] = (char)CodePoint;
            }
            else
            {
                // Unmappable character, use replacement
                pAnsi[AnsiCount++] = '?';
            }
            i += 2;
        }
        else
        {
            // Multi-byte character that doesn't map to ANSI, use replacement
            pAnsi[AnsiCount++] = '?';
            // Skip UTF-8 sequence
            if ((FirstByte & 0xF0) == 0xE0)
            {
                i += 3;
            }
            else if ((FirstByte & 0xF8) == 0xF0)
            {
                i += 4;
            }
            else
            {
                i += 1;
            }
        }
    }

    return AnsiCount;
}

// Helper function: Convert ANSI (Windows-1252) to UTF-8
static size_t KS_ConvertAnsiToUtf8(const char* pAnsi, size_t AnsiSize, char* pUtf8, size_t MaxUtf8Size)
{
    size_t Utf8Count = 0;

    for (size_t i = 0; i < AnsiSize && Utf8Count < MaxUtf8Size - 3; i++)
    {
        uint8_t AnsiChar = (uint8_t)pAnsi[i];

        if (AnsiChar < 0x80)
        {
            // ASCII characters map directly
            pUtf8[Utf8Count++] = pAnsi[i];
        }
        else
        {
            // Windows-1252 extended characters (0x80-0xFF)
            // Convert to UTF-8 (most map to 2-byte sequences)
            uint32_t CodePoint = AnsiChar; // Simplified - real implementation needs full mapping

            if (CodePoint < 0x800)
            {
                pUtf8[Utf8Count++] = (char)(0xC0 | (CodePoint >> 6));
                pUtf8[Utf8Count++] = (char)(0x80 | (CodePoint & 0x3F));
            }
            else
            {
                pUtf8[Utf8Count++] = (char)(0xE0 | (CodePoint >> 12));
                pUtf8[Utf8Count++] = (char)(0x80 | ((CodePoint >> 6) & 0x3F));
                pUtf8[Utf8Count++] = (char)(0x80 | (CodePoint & 0x3F));
            }
        }
    }

    return Utf8Count;
}

// Convert string to different encoding
KString KStringConvertToEncoding(const KString Str, const KStringEncoding TargetEncoding)
{
    if (false == KStringIsValid(Str))
    {
        return KStringInvalid();
    }

    KStringEncoding SourceEncoding = KS_GetEncodingFromField(Str.Size);

    // If already the target encoding, return a copy
    if (SourceEncoding == TargetEncoding)
    {
        size_t      StrSize = KS_GetSizeFromField(Str.Size);
        const char* pData   = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);
        return KStringCreateWithEncoding(pData, StrSize, TargetEncoding);
    }

    // Route to specific conversion functions
    switch (SourceEncoding)
    {
        case KSTRING_ENCODING_UTF8:
            switch (TargetEncoding)
            {
                case KSTRING_ENCODING_UTF16LE:
                    return KStringConvertUtf8ToUtf16Le(Str);
                case KSTRING_ENCODING_UTF16BE:
                    return KStringConvertUtf8ToUtf16Be(Str);
                case KSTRING_ENCODING_ANSI:
                    return KStringConvertUtf8ToAnsi(Str);
                default:
                    return KStringInvalid();
            }

        case KSTRING_ENCODING_UTF16LE:
            switch (TargetEncoding)
            {
                case KSTRING_ENCODING_UTF8:
                    return KStringConvertUtf16LeToUtf8(Str);
                case KSTRING_ENCODING_UTF16BE:
                    return KStringConvertUtf16LeToUtf16Be(Str);
                case KSTRING_ENCODING_ANSI:
                {
                    // Convert UTF-16LE -> UTF-8 -> ANSI
                    KString Utf8Str = KStringConvertUtf16LeToUtf8(Str);
                    if (false == KStringIsValid(Utf8Str))
                    {
                        return KStringInvalid();
                    }
                    KString AnsiStr = KStringConvertUtf8ToAnsi(Utf8Str);
                    KStringDestroy(Utf8Str);
                    return AnsiStr;
                }
                default:
                    return KStringInvalid();
            }

        case KSTRING_ENCODING_UTF16BE:
            switch (TargetEncoding)
            {
                case KSTRING_ENCODING_UTF8:
                    return KStringConvertUtf16BeToUtf8(Str);
                case KSTRING_ENCODING_UTF16LE:
                    return KStringConvertUtf16BeToUtf16Le(Str);
                case KSTRING_ENCODING_ANSI:
                {
                    // Convert UTF-16BE -> UTF-8 -> ANSI
                    KString Utf8Str = KStringConvertUtf16BeToUtf8(Str);
                    if (false == KStringIsValid(Utf8Str))
                    {
                        return KStringInvalid();
                    }
                    KString AnsiStr = KStringConvertUtf8ToAnsi(Utf8Str);
                    KStringDestroy(Utf8Str);
                    return AnsiStr;
                }
                default:
                    return KStringInvalid();
            }

        case KSTRING_ENCODING_ANSI:
            switch (TargetEncoding)
            {
                case KSTRING_ENCODING_UTF8:
                    return KStringConvertAnsiToUtf8(Str);
                case KSTRING_ENCODING_UTF16LE:
                {
                    // Convert ANSI -> UTF-8 -> UTF-16LE
                    KString Utf8Str = KStringConvertAnsiToUtf8(Str);
                    if (false == KStringIsValid(Utf8Str))
                    {
                        return KStringInvalid();
                    }
                    KString Utf16LeStr = KStringConvertUtf8ToUtf16Le(Utf8Str);
                    KStringDestroy(Utf8Str);
                    return Utf16LeStr;
                }
                case KSTRING_ENCODING_UTF16BE:
                {
                    // Convert ANSI -> UTF-8 -> UTF-16BE
                    KString Utf8Str = KStringConvertAnsiToUtf8(Str);
                    if (false == KStringIsValid(Utf8Str))
                    {
                        return KStringInvalid();
                    }
                    KString Utf16BeStr = KStringConvertUtf8ToUtf16Be(Utf8Str);
                    KStringDestroy(Utf8Str);
                    return Utf16BeStr;
                }
                default:
                    return KStringInvalid();
            }

        default:
            return KStringInvalid();
    }
}

// UTF-8 <-> UTF-16LE conversion
KString KStringConvertUtf8ToUtf16Le(const KString Str)
{
    if (false == KStringIsValid(Str) || KS_GetEncodingFromField(Str.Size) != KSTRING_ENCODING_UTF8)
    {
        return KStringInvalid();
    }

    size_t      Utf8Size  = KS_GetSizeFromField(Str.Size);
    const char* pUtf8Data = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);

    // Estimate UTF-16 size (worst case: each UTF-8 byte becomes a UTF-16 character)
    size_t    MaxUtf16Count = Utf8Size;
    uint16_t* pUtf16Buffer  = (uint16_t*)KS_Alloc((MaxUtf16Count + 1) * sizeof(uint16_t));
    if (NULL == pUtf16Buffer)
    {
        return KStringInvalid();
    }

    size_t Utf16Count = KS_ConvertUtf8ToUtf16Le(pUtf8Data, Utf8Size, pUtf16Buffer, MaxUtf16Count);

    KString Result = KStringCreateWithEncoding((const char*)pUtf16Buffer, Utf16Count * sizeof(uint16_t), KSTRING_ENCODING_UTF16LE);
    KS_Release((void**)&pUtf16Buffer);

    return Result;
}

KString KStringConvertUtf16LeToUtf8(const KString Str)
{
    if (false == KStringIsValid(Str) || KS_GetEncodingFromField(Str.Size) != KSTRING_ENCODING_UTF16LE)
    {
        return KStringInvalid();
    }

    size_t          Utf16Size  = KS_GetSizeFromField(Str.Size) / sizeof(uint16_t);
    const uint16_t* pUtf16Data = (const uint16_t*)((true == KStringIsShort(Str)) ? Str.Content : KS_GetPointer(Str.LongStr.PtrAndClass));

    // Estimate UTF-8 size (worst case: each UTF-16 character becomes 4 UTF-8 bytes)
    size_t MaxUtf8Size = Utf16Size * 4;
    char*  pUtf8Buffer = KS_Alloc(MaxUtf8Size + 1);
    if (NULL == pUtf8Buffer)
    {
        return KStringInvalid();
    }

    size_t Utf8Count = KS_ConvertUtf16LeToUtf8(pUtf16Data, Utf16Size, pUtf8Buffer, MaxUtf8Size);

    KString Result = KStringCreateWithEncoding(pUtf8Buffer, Utf8Count, KSTRING_ENCODING_UTF8);
    KS_Release((void**)&pUtf8Buffer);

    return Result;
}

// UTF-8 <-> UTF-16BE conversion
KString KStringConvertUtf8ToUtf16Be(const KString Str)
{
    // First convert to UTF-16LE, then swap bytes
    KString Utf16LeStr = KStringConvertUtf8ToUtf16Le(Str);
    if (false == KStringIsValid(Utf16LeStr))
    {
        return KStringInvalid();
    }

    return KStringConvertUtf16LeToUtf16Be(Utf16LeStr);
}

KString KStringConvertUtf16BeToUtf8(const KString Str)
{
    // First convert to UTF-16LE, then convert to UTF-8
    KString Utf16LeStr = KStringConvertUtf16BeToUtf16Le(Str);
    if (false == KStringIsValid(Utf16LeStr))
    {
        return KStringInvalid();
    }

    KString Result = KStringConvertUtf16LeToUtf8(Utf16LeStr);
    KStringDestroy(Utf16LeStr);

    return Result;
}

// UTF-16LE <-> UTF-16BE conversion (byte swapping)
KString KStringConvertUtf16LeToUtf16Be(const KString Str)
{
    if (false == KStringIsValid(Str) || KS_GetEncodingFromField(Str.Size) != KSTRING_ENCODING_UTF16LE)
    {
        return KStringInvalid();
    }

    size_t      DataSize    = KS_GetSizeFromField(Str.Size);
    const char* pSourceData = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);

    char* pSwappedData = KS_Alloc(DataSize + 2); // +2 for potential null terminator
    if (NULL == pSwappedData)
    {
        return KStringInvalid();
    }

    // Swap bytes for UTF-16 byte order conversion
    for (size_t i = 0; i < DataSize - 1; i += 2)
    {
        pSwappedData[i]     = pSourceData[i + 1]; // High byte
        pSwappedData[i + 1] = pSourceData[i];     // Low byte
    }

    KString Result = KStringCreateWithEncoding(pSwappedData, DataSize, KSTRING_ENCODING_UTF16BE);
    KS_Release((void**)&pSwappedData);

    return Result;
}

KString KStringConvertUtf16BeToUtf16Le(const KString Str)
{
    if (false == KStringIsValid(Str) || KS_GetEncodingFromField(Str.Size) != KSTRING_ENCODING_UTF16BE)
    {
        return KStringInvalid();
    }

    size_t      DataSize    = KS_GetSizeFromField(Str.Size);
    const char* pSourceData = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);

    char* pSwappedData = KS_Alloc(DataSize + 2); // +2 for potential null terminator
    if (NULL == pSwappedData)
    {
        return KStringInvalid();
    }

    // Swap bytes for UTF-16 byte order conversion
    for (size_t i = 0; i < DataSize - 1; i += 2)
    {
        pSwappedData[i]     = pSourceData[i + 1]; // High byte
        pSwappedData[i + 1] = pSourceData[i];     // Low byte
    }

    KString Result = KStringCreateWithEncoding(pSwappedData, DataSize, KSTRING_ENCODING_UTF16LE);
    KS_Release((void**)&pSwappedData);

    return Result;
}

// UTF-8 <-> ANSI conversion
KString KStringConvertUtf8ToAnsi(const KString Str)
{
    if (false == KStringIsValid(Str) || KS_GetEncodingFromField(Str.Size) != KSTRING_ENCODING_UTF8)
    {
        return KStringInvalid();
    }

    size_t      Utf8Size  = KS_GetSizeFromField(Str.Size);
    const char* pUtf8Data = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);

    char* pAnsiBuffer = KS_Alloc(Utf8Size + 1); // ANSI is typically smaller than UTF-8
    if (NULL == pAnsiBuffer)
    {
        return KStringInvalid();
    }

    size_t AnsiCount = KS_ConvertUtf8ToAnsi(pUtf8Data, Utf8Size, pAnsiBuffer, Utf8Size);

    KString Result = KStringCreateWithEncoding(pAnsiBuffer, AnsiCount, KSTRING_ENCODING_ANSI);
    KS_Release((void**)&pAnsiBuffer);

    return Result;
}

KString KStringConvertAnsiToUtf8(const KString Str)
{
    if (false == KStringIsValid(Str) || KS_GetEncodingFromField(Str.Size) != KSTRING_ENCODING_ANSI)
    {
        return KStringInvalid();
    }

    size_t      AnsiSize  = KS_GetSizeFromField(Str.Size);
    const char* pAnsiData = (true == KStringIsShort(Str)) ? Str.Content : (const char*)KS_GetPointer(Str.LongStr.PtrAndClass);

    char* pUtf8Buffer = KS_Alloc(AnsiSize * 3 + 1); // UTF-8 can be up to 3x larger
    if (NULL == pUtf8Buffer)
    {
        return KStringInvalid();
    }

    size_t Utf8Count = KS_ConvertAnsiToUtf8(pAnsiData, AnsiSize, pUtf8Buffer, AnsiSize * 3);

    KString Result = KStringCreateWithEncoding(pUtf8Buffer, Utf8Count, KSTRING_ENCODING_UTF8);
    KS_Release((void**)&pUtf8Buffer);

    return Result;
}

//
// Error Handling
//

bool KStringIsValid(const KString Str)
{
    return Str.Size != KSTRING_INVALID_LENGTH;
}

KString KStringInvalid(void)
{
    KString Invalid = {0};
    Invalid.Size    = KSTRING_INVALID_LENGTH;
    return Invalid;
}
