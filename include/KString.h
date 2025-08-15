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
        uint32_t Size; // Internal: 32-bit size (max 4GB) to fit in 16 bytes

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

    // Get size (always fast - O(1))
    size_t KStringSize(const KString Str);

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
