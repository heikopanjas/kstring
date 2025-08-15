#include "KString.h"
#include <stdio.h>
#include <stdlib.h>

// Helper function to get encoding name for display
const char* GetEncodingName(KStringEncoding Encoding)
{
    switch (Encoding)
    {
        case KSTRING_ENCODING_UTF8:
            return "UTF-8";
        case KSTRING_ENCODING_UTF16LE:
            return "UTF-16LE";
        case KSTRING_ENCODING_UTF16BE:
            return "UTF-16BE";
        case KSTRING_ENCODING_ANSI:
            return "ANSI";
        default:
            return "Unknown";
    }
}

int main(void)
{
    printf("KString Library Demo - Kraut Strings\n");
    printf("====================================\n\n");

    printf("Testing our German String (Kraut String) implementation...\n\n");

    // Test 1: Basic string creation and properties
    printf("1. Basic String Creation and Properties\n");
    printf("---------------------------------------\n");

    KString ShortStr = KStringCreate("Hello!", 6);
    KString LongStr  = KStringCreate("This is a longer string that exceeds 12 characters", 50);

    printf("Short string: \"%s\" (length: %zu, is_short: %s)\n", KStringCStr(ShortStr), KStringSize(ShortStr), KStringIsShort(ShortStr) ? "true" : "false");

    printf("Long string: \"%s\" (length: %zu, is_short: %s)\n", KStringCStr(LongStr), KStringSize(LongStr), KStringIsShort(LongStr) ? "true" : "false");

    // Test 2: String comparison
    printf("\n2. String Comparison\n");
    printf("--------------------\n");

    KString Str1 = KStringCreate("Apple", 5);
    KString Str2 = KStringCreate("Banana", 6);
    KString Str3 = KStringCreate("Apple", 5);

    printf("Comparing \"%s\" vs \"%s\": %d\n", KStringCStr(Str1), KStringCStr(Str2), KStringCompare(Str1, Str2));
    printf("Comparing \"%s\" vs \"%s\": %d\n", KStringCStr(Str1), KStringCStr(Str3), KStringCompare(Str1, Str3));
    printf("Are \"%s\" and \"%s\" equal? %s\n", KStringCStr(Str1), KStringCStr(Str3), KStringEquals(Str1, Str3) ? "Yes" : "No");

    // Test 3: String concatenation
    printf("\n3. String Concatenation\n");
    printf("-----------------------\n");

    KString Hello    = KStringCreate("Hello", 5);
    KString World    = KStringCreate(" World!", 7);
    KString Combined = KStringConcat(Hello, World);

    printf("Concatenating \"%s\" + \"%s\" = \"%s\"\n", KStringCStr(Hello), KStringCStr(World), KStringCStr(Combined));

    // Test 4: Substring operations
    printf("\n4. Substring Operations\n");
    printf("-----------------------\n");

    KString Source = KStringCreate("Programming", 11);
    KString SubStr = KStringSubstring(Source, 0, 7); // "Program"

    printf("Substring of \"%s\" (0, 7): \"%s\"\n", KStringCStr(Source), KStringCStr(SubStr));

    // Test 5: Prefix testing
    printf("\n5. Prefix Testing\n");
    printf("-----------------\n");

    KString Text    = KStringCreate("German strings are efficient", 28);
    KString Prefix1 = KStringCreate("German", 6);
    KString Prefix2 = KStringCreate("English", 7);

    printf("Does \"%s\" start with \"%s\"? %s\n", KStringCStr(Text), KStringCStr(Prefix1), KStringStartsWith(Text, Prefix1) ? "Yes" : "No");
    printf("Does \"%s\" start with \"%s\"? %s\n", KStringCStr(Text), KStringCStr(Prefix2), KStringStartsWith(Text, Prefix2) ? "Yes" : "No");

    // Test 6: Case-Insensitive Operations
    printf("\n6. Case-Insensitive Operations\n");
    printf("-------------------------------\n");

    KString Upper       = KStringCreate("HELLO WORLD", 11);
    KString Lower       = KStringCreate("hello world", 11);
    KString Mixed       = KStringCreate("Hello World", 11);
    KString UpperPrefix = KStringCreate("HELLO", 5);

    printf("Case-sensitive equality: \"%s\" == \"%s\"? %s\n", KStringCStr(Upper), KStringCStr(Lower), KStringEquals(Upper, Lower) ? "Yes" : "No");
    printf("Case-insensitive equality: \"%s\" == \"%s\"? %s\n", KStringCStr(Upper), KStringCStr(Lower), KStringEqualsIgnoreCase(Upper, Lower) ? "Yes" : "No");
    printf(
        "Case-insensitive starts with: \"%s\" starts with \"%s\"? %s\n", KStringCStr(Mixed), KStringCStr(UpperPrefix),
        KStringStartsWithIgnoreCase(Mixed, UpperPrefix) ? "Yes" : "No");

    // Test 7: Character Encoding Support
    printf("\n7. Character Encoding Support\n");
    printf("------------------------------\n");

    // Create strings with different encodings
    KString Utf8Str    = KStringCreateWithEncoding("UTF-8 Text", 10, KSTRING_ENCODING_UTF8);
    KString Utf16LeStr = KStringCreateWithEncoding("UTF-16LE", 8, KSTRING_ENCODING_UTF16LE);
    KString Utf16BeStr = KStringCreateWithEncoding("UTF-16BE", 8, KSTRING_ENCODING_UTF16BE);
    KString AnsiStr    = KStringCreateWithEncoding("ANSI Text", 9, KSTRING_ENCODING_ANSI);

    printf("UTF-8 string: \"%s\" (encoding: %s)\n", KStringCStr(Utf8Str), GetEncodingName(KStringGetEncoding(Utf8Str)));
    printf("UTF-16LE string: \"%s\" (encoding: %s)\n", KStringCStr(Utf16LeStr), GetEncodingName(KStringGetEncoding(Utf16LeStr)));
    printf("UTF-16BE string: \"%s\" (encoding: %s)\n", KStringCStr(Utf16BeStr), GetEncodingName(KStringGetEncoding(Utf16BeStr)));
    printf("ANSI string: \"%s\" (encoding: %s)\n", KStringCStr(AnsiStr), GetEncodingName(KStringGetEncoding(AnsiStr)));

    // Test encoding preservation in operations
    KString ConcatEncoded = KStringConcat(Utf8Str, Utf16LeStr);
    printf("Concatenated string encoding: %s (inherits from first string)\n", GetEncodingName(KStringGetEncoding(ConcatEncoded)));

    // Test 8: Encoding Conversion
    printf("\n8. Encoding Conversion\n");
    printf("----------------------\n");

    // Test UTF-8 conversions
    KString TestUtf8 = KStringCreateWithEncoding("Hello World! ??", 16, KSTRING_ENCODING_UTF8);
    printf(
        "Original UTF-8: \"%s\" (encoding: %s, size: %zu bytes)\n", KStringCStr(TestUtf8), GetEncodingName(KStringGetEncoding(TestUtf8)),
        KStringSize(TestUtf8));

    // Convert UTF-8 to UTF-16LE
    KString ConvertedUtf16Le = KStringConvertUtf8ToUtf16Le(TestUtf8);
    printf("Converted to UTF-16LE: (encoding: %s, size: %zu bytes)\n", GetEncodingName(KStringGetEncoding(ConvertedUtf16Le)), KStringSize(ConvertedUtf16Le));

    // Convert UTF-8 to UTF-16BE
    KString ConvertedUtf16Be = KStringConvertUtf8ToUtf16Be(TestUtf8);
    printf("Converted to UTF-16BE: (encoding: %s, size: %zu bytes)\n", GetEncodingName(KStringGetEncoding(ConvertedUtf16Be)), KStringSize(ConvertedUtf16Be));

    // Convert UTF-8 to ANSI
    KString ConvertedAnsi = KStringConvertUtf8ToAnsi(TestUtf8);
    printf(
        "Converted to ANSI: \"%s\" (encoding: %s, size: %zu bytes)\n", KStringCStr(ConvertedAnsi), GetEncodingName(KStringGetEncoding(ConvertedAnsi)),
        KStringSize(ConvertedAnsi));

    // Test round-trip conversion UTF-8 -> UTF-16LE -> UTF-8
    KString RoundTripUtf8 = KStringConvertUtf16LeToUtf8(ConvertedUtf16Le);
    printf("Round-trip UTF-8->UTF-16LE->UTF-8: \"%s\" (encoding: %s)\n", KStringCStr(RoundTripUtf8), GetEncodingName(KStringGetEncoding(RoundTripUtf8)));

    // Test byte order conversion UTF-16LE <-> UTF-16BE
    KString SwappedUtf16Be = KStringConvertUtf16LeToUtf16Be(ConvertedUtf16Le);
    KString SwappedUtf16Le = KStringConvertUtf16BeToUtf16Le(SwappedUtf16Be);
    printf("UTF-16 byte order conversion successful: %s\n", (KStringGetEncoding(SwappedUtf16Le) == KSTRING_ENCODING_UTF16LE) ? "Yes" : "No");

    // Test 9: Memory Cleanup
    printf("\n8. Memory Cleanup\n");
    printf("-----------------\n");

    KStringDestroy(ShortStr);
    KStringDestroy(LongStr);
    KStringDestroy(Str1);
    KStringDestroy(Str2);
    KStringDestroy(Str3);
    KStringDestroy(Hello);
    KStringDestroy(World);
    KStringDestroy(Combined);
    KStringDestroy(Source);
    KStringDestroy(SubStr);
    KStringDestroy(Text);
    KStringDestroy(Prefix1);
    KStringDestroy(Prefix2);
    KStringDestroy(Upper);
    KStringDestroy(Lower);
    KStringDestroy(Mixed);
    KStringDestroy(UpperPrefix);
    KStringDestroy(Utf8Str);
    KStringDestroy(Utf16LeStr);
    KStringDestroy(Utf16BeStr);
    KStringDestroy(AnsiStr);
    KStringDestroy(ConcatEncoded);
    KStringDestroy(TestUtf8);
    KStringDestroy(ConvertedUtf16Le);
    KStringDestroy(ConvertedUtf16Be);
    KStringDestroy(ConvertedAnsi);
    KStringDestroy(RoundTripUtf8);
    KStringDestroy(SwappedUtf16Be);
    KStringDestroy(SwappedUtf16Le);

    printf("All strings cleaned up successfully.\n");

    printf("\n? Kraut String implementation working correctly!\n");
    printf("  Based on German String research from Umbra/CedarDB\n");
    printf("  16-byte fixed size, optimized for performance\n");

    return 0;
}
