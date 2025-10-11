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

#ifndef KSTRING_H
#define KSTRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//
// KString Library - Kraut Strings Implementation
// Based on German String research from Umbra/CedarDB
// 16-byte fixed-size string representation for optimal performance
//

#ifdef __cplusplus
extern "C" {
#endif

// Maximum length for short strings (stored inline)
#define KSTRING_MAX_SHORT_LENGTH 12

    // Character encoding types (stored in upper 2 bits of Size field)
    typedef enum
    {
        KSTRING_ENCODING_UTF8    = 0, // Default UTF-8 encoding
        KSTRING_ENCODING_UTF16LE = 1, // UTF-16 Little Endian
        KSTRING_ENCODING_UTF16BE = 2, // UTF-16 Big Endian
        KSTRING_ENCODING_ANSI    = 3  // ANSI/Windows-1252 (legacy Windows)
    } KStringEncoding;

// Bit masks for Size field (32 bits total)
#define KSTRING_SIZE_MASK         0x3FFF'FFFF  // 30-bit size mask (max ~1GB)
#define KSTRING_ENCODING_MASK     0xC000'0000  // 2-bit encoding mask
#define KSTRING_ENCODING_SHIFT    30

    // Storage classes for long strings
    typedef enum
    {
        KSTRING_PERSISTENT = 0, // Valid forever (literals, constants)
        KSTRING_TRANSIENT  = 1, // Temporarily valid
        KSTRING_TEMPORARY  = 2  // Needs cleanup
    } KStringStorageClass;

// KString structure - exactly 16 bytes
// Use pragma pack to eliminate padding
#pragma pack(push, 1)

    typedef struct KString
    {
        uint32_t Size; // 30-bit size (max ~1GB) + 2-bit encoding in upper bits

        // Union must be exactly 12 bytes to total 16 bytes
        union
        {
            // Short string (?12 chars): inline content (12 bytes)
            char Content[12];

            // Long string (>12 chars): prefix (4 bytes) + pointer+class (8 bytes) = 12 bytes
            struct
            {
                char     Prefix[4];   // First 4 characters (4 bytes)
                uint64_t PtrAndClass; // 62-bit pointer + 2-bit storage class (8 bytes)
            } LongStr;
        };
    } KString;

#pragma pack(pop)

    // Compile-time assertion to ensure 16-byte size
    _Static_assert(sizeof(KString) == 16, "KString must be exactly 16 bytes");

    //
    // Core Operations
    //

    // Create KString from C string (returns by value for register efficiency)
    KString KStringCreate(const char* pStr, const size_t Size);
    KString KStringCreatePersistent(const char* pStr, const size_t Size);
    KString KStringCreateTransient(const char* pStr, const size_t Size);

    // Create KString with explicit encoding
    KString KStringCreateWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);
    KString KStringCreatePersistentWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);
    KString KStringCreateTransientWithEncoding(const char* pStr, const size_t Size, const KStringEncoding Encoding);

    // Convenience functions for null-terminated strings (less secure, use with caution)
    KString KStringCreateFromCStr(const char* pStr);
    KString KStringCreatePersistentFromCStr(const char* pStr);

    // Cleanup (only needed for temporary strings)
    void KStringDestroy(const KString Str);

    //
    // Access Operations
    //

    // Get C-string representation (may allocate for short strings)
    const char* KStringCStr(const KString Str);

    // Get size in bytes (always fast - O(1))
    size_t KStringSize(const KString Str);

    // Get character encoding
    KStringEncoding KStringGetEncoding(const KString Str);

    // Check if string is stored inline
    bool KStringIsShort(const KString Str);

    //
    // Comparison Operations (optimized with prefix)
    //

    // Compare two strings (-1, 0, 1)
    int KStringCompare(const KString StrA, const KString StrB);

    // Check equality (optimized fast path)
    bool KStringEquals(const KString StrA, const KString StrB);

    // Check if string starts with prefix
    bool KStringStartsWith(const KString Str, const KString Prefix);

    // Case-insensitive comparison operations
    int  KStringCompareIgnoreCase(const KString StrA, const KString StrB);
    bool KStringEqualsIgnoreCase(const KString StrA, const KString StrB);
    bool KStringStartsWithIgnoreCase(const KString Str, const KString Prefix);

    //
    // String Operations (create new strings)
    //

    // Concatenate two strings
    KString KStringConcat(const KString StrA, const KString StrB);

    // Extract substring
    KString KStringSubstring(const KString Str, const size_t Offset, const size_t Size);

    //
    // Encoding Conversion Operations
    //

    // Convert string to different encoding (creates new string)
    KString KStringConvertToEncoding(const KString Str, const KStringEncoding TargetEncoding);

    // UTF-8 <-> UTF-16LE conversion
    KString KStringConvertUtf8ToUtf16Le(const KString Str);
    KString KStringConvertUtf16LeToUtf8(const KString Str);

    // UTF-8 <-> UTF-16BE conversion
    KString KStringConvertUtf8ToUtf16Be(const KString Str);
    KString KStringConvertUtf16BeToUtf8(const KString Str);

    // UTF-16LE <-> UTF-16BE conversion
    KString KStringConvertUtf16LeToUtf16Be(const KString Str);
    KString KStringConvertUtf16BeToUtf16Le(const KString Str);

    // UTF-8 <-> ANSI conversion (Windows-1252)
    KString KStringConvertUtf8ToAnsi(const KString Str);
    KString KStringConvertAnsiToUtf8(const KString Str);

    //
    // Error Handling
    //

    // Check if KString is valid (not an error result)
    bool KStringIsValid(const KString Str);

    // Create invalid KString (for error returns)
    KString KStringInvalid(void);

#ifdef __cplusplus
}
#endif

#endif // KSTRING_H
