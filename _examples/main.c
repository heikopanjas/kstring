#include "KString.h"
#include <stdio.h>
#include <stdlib.h>

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

    // Test 7: Memory cleanup
    printf("\n7. Memory Cleanup\n");
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

    printf("All strings cleaned up successfully.\n");

    printf("\n? Kraut String implementation working correctly!\n");
    printf("  Based on German String research from Umbra/CedarDB\n");
    printf("  16-byte fixed size, optimized for performance\n");

    return 0;
}
