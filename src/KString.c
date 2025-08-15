#include "KString.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//
// KString Implementation - Kraut Strings
// Based on German String research from Umbra/CedarDB
//

// Pointer tagging masks for storage class
#define KSTRING_PTR_MASK     0x3FFF'FFFF'FFFF'FFFFULL  // 62-bit pointer mask
#define KSTRING_CLASS_MASK   0xC000'0000'0000'0000ULL  // 2-bit storage class mask
#define KSTRING_CLASS_SHIFT  62

// Invalid length marker for error handling
#define KSTRING_INVALID_LENGTH UINT32_MAX

//
// Private Helper Functions
//

// Check if string length qualifies for short representation
inline static bool IsShortString(size_t Size)
{
    return Size <= KSTRING_MAX_SHORT_LENGTH;
}

// Extract storage class from tagged pointer
inline static KStringStorageClass GetStorageClass(uint64_t PtrAndClass)
{
    return (KStringStorageClass)((PtrAndClass & KSTRING_CLASS_MASK) >> KSTRING_CLASS_SHIFT);
}

// Extract pointer from tagged pointer
inline static void* GetPointer(uint64_t PtrAndClass)
{
    return (void*)(PtrAndClass & KSTRING_PTR_MASK);
}

// Create tagged pointer with storage class
inline static uint64_t CreateTaggedPointer(void* pPointer, KStringStorageClass StorageClass)
{
    uint64_t Ptr   = (uint64_t)pPointer;
    uint64_t Class = ((uint64_t)StorageClass) << KSTRING_CLASS_SHIFT;
    return (Ptr & KSTRING_PTR_MASK) | Class;
}

//
// Core Operations
//

KString KStringCreate(const char* pStr, const size_t Size)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    // Handle size overflow (convert size_t to uint32_t)
    if (Size > UINT32_MAX)
    {
        return KStringInvalid();
    }

    KString Result;
    Result.Size = (uint32_t)Size;

    if (IsShortString(Size))
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
        char* pData = calloc(Size + 1, 1); // +1 for null terminator, zero-initialized
        if (NULL == pData)
        {
            return KStringInvalid();
        }

        memcpy(pData, pStr, Size);
        pData[Size] = '\0';

        // Store first 4 characters as prefix
        memcpy(Result.LongStr.Prefix, pStr, 4);

        // Create tagged pointer with TEMPORARY storage class
        Result.LongStr.PtrAndClass = CreateTaggedPointer(pData, KSTRING_TEMPORARY);
    }

    return Result;
}

KString KStringCreatePersistent(const char* pStr, const size_t Size)
{
    if (NULL == pStr)
    {
        return KStringInvalid();
    }

    if (Size > UINT32_MAX)
    {
        return KStringInvalid();
    }

    KString Result;
    Result.Size = (uint32_t)Size;

    if (IsShortString(Size))
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
        memcpy(Result.LongStr.Prefix, pStr, 4);
        Result.LongStr.PtrAndClass = CreateTaggedPointer((void*)pStr, KSTRING_PERSISTENT);
    }

    return Result;
}

KString KStringCreateTransient(const char* pStr, const size_t Size)
{
    if (NULL == pStr || Size > UINT32_MAX)
    {
        return KStringInvalid();
    }

    KString Result;
    Result.Size = (uint32_t)Size;

    if (IsShortString(Size))
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
        memcpy(Result.LongStr.Prefix, pStr, 4);
        Result.LongStr.PtrAndClass = CreateTaggedPointer((void*)pStr, KSTRING_TRANSIENT);
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
        KStringStorageClass StorageClass = GetStorageClass(Str.LongStr.PtrAndClass);
        if (KSTRING_TEMPORARY == StorageClass)
        {
            void* pData = GetPointer(Str.LongStr.PtrAndClass);
            free(pData);
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
        static __thread char ShortBuffers[4][KSTRING_MAX_SHORT_LENGTH + 1];
        static __thread int  BufferIndex = 0;

        char* Buffer = ShortBuffers[BufferIndex];
        BufferIndex  = (BufferIndex + 1) % 4;

        memcpy(Buffer, Str.Content, Str.Size);
        Buffer[Str.Size] = '\0';
        return Buffer;
    }
    else
    {
        // Long strings are already null-terminated
        return (const char*)GetPointer(Str.LongStr.PtrAndClass);
    }
}

size_t KStringSize(const KString Str)
{
    return KStringIsValid(Str) ? (size_t)Str.Size : 0;
}

bool KStringIsShort(const KString Str)
{
    return IsShortString(Str.Size);
}

//
// Comparison Operations
//

int KStringCompare(const KString StrA, const KString StrB)
{
    // Fast path: compare lengths first
    if (StrA.Size != StrB.Size)
    {
        return (StrA.Size < StrB.Size) ? -1 : 1;
    }

    // Same length - compare content
    if (true == KStringIsShort(StrA) && true == KStringIsShort(StrB))
    {
        // Both short: direct memory comparison
        return memcmp(StrA.Content, StrB.Content, StrA.Size);
    }
    else if (true == KStringIsShort(StrA) || true == KStringIsShort(StrB))
    {
        // One short, one long: get actual data
        const char* pDataA = (true == KStringIsShort(StrA)) ? StrA.Content : (const char*)GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (true == KStringIsShort(StrB)) ? StrB.Content : (const char*)GetPointer(StrB.LongStr.PtrAndClass);
        return memcmp(pDataA, pDataB, StrA.Size);
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
        const char* pDataA = (const char*)GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (const char*)GetPointer(StrB.LongStr.PtrAndClass);
        return memcmp(pDataA, pDataB, StrA.Size);
    }
}

bool KStringEquals(const KString StrA, const KString StrB)
{
    return KStringCompare(StrA, StrB) == 0;
}

bool KStringStartsWith(const KString Str, const KString Prefix)
{
    if (Prefix.Size > Str.Size)
    {
        return false;
    }

    if (Prefix.Size == 0)
    {
        return true;
    }

    // Compare prefixes efficiently
    if (true == KStringIsShort(Str) && true == KStringIsShort(Prefix))
    {
        return memcmp(Str.Content, Prefix.Content, Prefix.Size) == 0;
    }
    else if (true == KStringIsShort(Str))
    {
        const char* pPrefixData = (true == KStringIsShort(Prefix)) ? Prefix.Content : (const char*)GetPointer(Prefix.LongStr.PtrAndClass);
        return memcmp(Str.Content, pPrefixData, Prefix.Size) == 0;
    }
    else if (true == KStringIsShort(Prefix))
    {
        // Str is long, Prefix is short: check against stored prefix first
        if (Prefix.Size <= 4)
        {
            return memcmp(Str.LongStr.Prefix, Prefix.Content, Prefix.Size) == 0;
        }
        else
        {
            const char* pStrData = (const char*)GetPointer(Str.LongStr.PtrAndClass);
            return memcmp(pStrData, Prefix.Content, Prefix.Size) == 0;
        }
    }
    else
    {
        // Both long: use stored prefixes for fast comparison
        if (Prefix.Size <= 4)
        {
            return memcmp(Str.LongStr.Prefix, Prefix.LongStr.Prefix, Prefix.Size) == 0;
        }
        else
        {
            // Prefix is longer than 4 chars: need full comparison
            const char* pStrData    = (const char*)GetPointer(Str.LongStr.PtrAndClass);
            const char* pPrefixData = (const char*)GetPointer(Prefix.LongStr.PtrAndClass);
            return memcmp(pStrData, pPrefixData, Prefix.Size) == 0;
        }
    }
}

//
// Private Helper Functions for Case-Insensitive Operations
//

// Convert character to lowercase (ASCII only for performance)
inline static char ToLowerAscii(char c)
{
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

// Case-insensitive memory comparison (ASCII only)
inline static int MemcmpIgnoreCase(const char* pStr1, const char* pStr2, size_t Size)
{
    for (size_t i = 0; i < Size; ++i)
    {
        char c1 = ToLowerAscii(pStr1[i]);
        char c2 = ToLowerAscii(pStr2[i]);
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
        return MemcmpIgnoreCase(StrA.Content, StrB.Content, StrA.Size);
    }
    else if (true == KStringIsShort(StrA) || true == KStringIsShort(StrB))
    {
        // One short, one long: get actual data
        const char* pDataA = (true == KStringIsShort(StrA)) ? StrA.Content : (const char*)GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (true == KStringIsShort(StrB)) ? StrB.Content : (const char*)GetPointer(StrB.LongStr.PtrAndClass);
        return MemcmpIgnoreCase(pDataA, pDataB, StrA.Size);
    }
    else
    {
        // Both long: compare prefixes first (fast path)
        int PrefixCmp = MemcmpIgnoreCase(StrA.LongStr.Prefix, StrB.LongStr.Prefix, 4);
        if (0 != PrefixCmp)
        {
            return PrefixCmp;
        }

        // Prefixes match: compare full strings
        const char* pDataA = (const char*)GetPointer(StrA.LongStr.PtrAndClass);
        const char* pDataB = (const char*)GetPointer(StrB.LongStr.PtrAndClass);
        return MemcmpIgnoreCase(pDataA, pDataB, StrA.Size);
    }
}

bool KStringEqualsIgnoreCase(const KString StrA, const KString StrB)
{
    return 0 == KStringCompareIgnoreCase(StrA, StrB);
}

bool KStringStartsWithIgnoreCase(const KString Str, const KString Prefix)
{
    if (Prefix.Size > Str.Size)
    {
        return false;
    }

    if (0 == Prefix.Size)
    {
        return true;
    }

    // Compare prefixes efficiently (case-insensitive)
    if (true == KStringIsShort(Str) && true == KStringIsShort(Prefix))
    {
        return 0 == MemcmpIgnoreCase(Str.Content, Prefix.Content, Prefix.Size);
    }
    else if (true == KStringIsShort(Str))
    {
        const char* pPrefixData = (true == KStringIsShort(Prefix)) ? Prefix.Content : (const char*)GetPointer(Prefix.LongStr.PtrAndClass);
        return 0 == MemcmpIgnoreCase(Str.Content, pPrefixData, Prefix.Size);
    }
    else if (true == KStringIsShort(Prefix))
    {
        // Str is long, Prefix is short: check against stored prefix first
        if (Prefix.Size <= 4)
        {
            return 0 == MemcmpIgnoreCase(Str.LongStr.Prefix, Prefix.Content, Prefix.Size);
        }
        else
        {
            const char* pStrData = (const char*)GetPointer(Str.LongStr.PtrAndClass);
            return 0 == MemcmpIgnoreCase(pStrData, Prefix.Content, Prefix.Size);
        }
    }
    else
    {
        // Both long: use stored prefixes for fast comparison
        if (Prefix.Size <= 4)
        {
            return 0 == MemcmpIgnoreCase(Str.LongStr.Prefix, Prefix.LongStr.Prefix, Prefix.Size);
        }
        else
        {
            // Prefix is longer than 4 chars: need full comparison
            const char* pStrData    = (const char*)GetPointer(Str.LongStr.PtrAndClass);
            const char* pPrefixData = (const char*)GetPointer(Prefix.LongStr.PtrAndClass);
            return 0 == MemcmpIgnoreCase(pStrData, pPrefixData, Prefix.Size);
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

    size_t TotalLength = (size_t)StrA.Size + (size_t)StrB.Size;

    if (TotalLength > UINT32_MAX)
    {
        return KStringInvalid();
    }

    // Allocate buffer for concatenated string
    char* pBuffer = calloc(TotalLength + 1, 1); // Zero-initialized
    if (NULL == pBuffer)
    {
        return KStringInvalid();
    }

    // Copy data from both strings
    const char* pDataA = (true == KStringIsShort(StrA)) ? StrA.Content : (const char*)GetPointer(StrA.LongStr.PtrAndClass);
    const char* pDataB = (true == KStringIsShort(StrB)) ? StrB.Content : (const char*)GetPointer(StrB.LongStr.PtrAndClass);

    memcpy(pBuffer, pDataA, StrA.Size);
    memcpy(pBuffer + StrA.Size, pDataB, StrB.Size);
    pBuffer[TotalLength] = '\0';

    // Create result KString
    KString Result;
    Result.Size = (uint32_t)TotalLength;

    if (IsShortString(TotalLength))
    {
        // Result fits in short string
        memcpy(Result.Content, pBuffer, TotalLength);
        if (TotalLength < KSTRING_MAX_SHORT_LENGTH)
        {
            memset(Result.Content + TotalLength, 0, KSTRING_MAX_SHORT_LENGTH - TotalLength);
        }
        free(pBuffer); // Don't need allocated buffer
    }
    else
    {
        // Result is long string
        memcpy(Result.LongStr.Prefix, pBuffer, 4);
        Result.LongStr.PtrAndClass = CreateTaggedPointer(pBuffer, KSTRING_TEMPORARY);
    }

    return Result;
}

KString KStringSubstring(const KString Str, const size_t Offset, const size_t Size)
{
    if (false == KStringIsValid(Str) || Offset >= Str.Size)
    {
        return KStringInvalid();
    }

    // Create local copy for modification
    size_t LocalSize = Size;

    // Clamp size to available characters
    if (Offset + LocalSize > Str.Size)
    {
        LocalSize = Str.Size - Offset;
    }

    if (LocalSize > UINT32_MAX)
    {
        return KStringInvalid();
    }

    const char* pSourceData = (true == KStringIsShort(Str)) ? Str.Content : (const char*)GetPointer(Str.LongStr.PtrAndClass);

    KString Result;
    Result.Size = (uint32_t)LocalSize;

    if (IsShortString(LocalSize))
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
        char* pBuffer = calloc(LocalSize + 1, 1); // Zero-initialized
        if (NULL == pBuffer)
        {
            return KStringInvalid();
        }

        memcpy(pBuffer, pSourceData + Offset, LocalSize);
        pBuffer[LocalSize] = '\0';

        memcpy(Result.LongStr.Prefix, pBuffer, 4);
        Result.LongStr.PtrAndClass = CreateTaggedPointer(pBuffer, KSTRING_TEMPORARY);
    }

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
