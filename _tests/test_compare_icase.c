#include "unity.h"
#include "KString.h"

void setUp(void) {}
void tearDown(void) {}

//
// KStringCompareIgnoreCase tests
//

void test_CompareICase_EqualMixedCase(void)
{
    KString A = KStringCreate("Hello", 5);
    KString B = KStringCreate("HELLO", 5);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_CompareICase_DifferentLength(void)
{
    KString A = KStringCreate("abc", 3);
    KString B = KStringCreate("abcd", 4);
    TEST_ASSERT_TRUE(KStringCompareIgnoreCase(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_CompareICase_DifferentContent(void)
{
    KString A = KStringCreate("abc", 3);
    KString B = KStringCreate("XYZ", 3);
    TEST_ASSERT_TRUE(KStringCompareIgnoreCase(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_CompareICase_BothLong(void)
{
    KString A = KStringCreate("Hello World!!", 13);
    KString B = KStringCreate("HELLO WORLD!!", 13);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_CompareICase_NonAlpha(void)
{
    KString A = KStringCreate("123!@#", 6);
    KString B = KStringCreate("123!@#", 6);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_CompareICase_BoundaryChars(void)
{
    // '@' (0x40) is just below 'A', '[' (0x5B) is just above 'Z'
    // These should NOT be case-folded
    KString A = KStringCreate("@[", 2);
    KString B = KStringCreate("@[", 2);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));

    KString C = KStringCreate("`{", 2);
    // '`' (0x60) is just below 'a', '{' (0x7B) is just above 'z'
    TEST_ASSERT_TRUE(KStringCompareIgnoreCase(A, C) != 0);
    KStringDestroy(A);
    KStringDestroy(B);
    KStringDestroy(C);
}

void test_CompareICase_Empty(void)
{
    KString A = KStringCreate("", 0);
    KString B = KStringCreate("", 0);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

// Bug-regression test: compare with non-UTF-8 encoding
// Before fix, raw Size field included encoding bits causing wrong behavior
void test_CompareICase_WithNonUtf8Encoding(void)
{
    KString A = KStringCreateWithEncoding("Hello", 5, KSTRING_ENCODING_ANSI);
    KString B = KStringCreateWithEncoding("HELLO", 5, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

// Verify different encodings with same size still compare correctly
void test_CompareICase_DifferentEncodings_SameSize(void)
{
    KString A = KStringCreateWithEncoding("test", 4, KSTRING_ENCODING_UTF8);
    KString B = KStringCreateWithEncoding("TEST", 4, KSTRING_ENCODING_ANSI);
    // Different encoding bits but same size - should still compare content
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_CompareICase_LongWithNonUtf8(void)
{
    KString A = KStringCreateWithEncoding("Hello World!!", 13, KSTRING_ENCODING_ANSI);
    KString B = KStringCreateWithEncoding("HELLO WORLD!!", 13, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_EQUAL_INT(0, KStringCompareIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

//
// KStringEqualsIgnoreCase tests
//

void test_EqualsICase_True(void)
{
    KString A = KStringCreate("hello", 5);
    KString B = KStringCreate("HELLO", 5);
    TEST_ASSERT_TRUE(KStringEqualsIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_EqualsICase_False(void)
{
    KString A = KStringCreate("hello", 5);
    KString B = KStringCreate("world", 5);
    TEST_ASSERT_FALSE(KStringEqualsIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_EqualsICase_Empty(void)
{
    KString A = KStringCreate("", 0);
    KString B = KStringCreate("", 0);
    TEST_ASSERT_TRUE(KStringEqualsIgnoreCase(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

//
// KStringStartsWithIgnoreCase tests
//

void test_StartsWithICase_True(void)
{
    KString Str    = KStringCreate("Hello World", 11);
    KString Prefix = KStringCreate("HELLO", 5);
    TEST_ASSERT_TRUE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWithICase_False(void)
{
    KString Str    = KStringCreate("Hello World", 11);
    KString Prefix = KStringCreate("WORLD", 5);
    TEST_ASSERT_FALSE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWithICase_EmptyPrefix(void)
{
    KString Str    = KStringCreate("Hello", 5);
    KString Prefix = KStringCreate("", 0);
    TEST_ASSERT_TRUE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWithICase_PrefixLongerThanStr(void)
{
    KString Str    = KStringCreate("Hi", 2);
    KString Prefix = KStringCreate("HELLO", 5);
    TEST_ASSERT_FALSE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWithICase_LongStr_ShortPrefix(void)
{
    KString Str    = KStringCreate("ABCDEFGHIJKLM", 13);
    KString Prefix = KStringCreate("abc", 3);
    TEST_ASSERT_TRUE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWithICase_BothLong(void)
{
    KString Str    = KStringCreate("ABCDEFGHIJKLMNOP", 16);
    KString Prefix = KStringCreate("abcdefghijklm", 13);
    TEST_ASSERT_TRUE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

// Long strings with different prefixes - exercises prefix mismatch fast path
void test_CompareICase_LongDifferentPrefix(void)
{
    KString A = KStringCreate("ABCDEFGHIJKLM", 13);
    KString B = KStringCreate("XBCDEFGHIJKLM", 13);
    TEST_ASSERT_TRUE(KStringCompareIgnoreCase(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

// Long strings where prefixes match case-insensitively but differ in full content
void test_CompareICase_LongSamePrefixDifferentContent(void)
{
    KString A = KStringCreate("ABCDxxxxYYYYY", 13);
    KString B = KStringCreate("abcdxxxxZZZZZ", 13);
    TEST_ASSERT_TRUE(KStringCompareIgnoreCase(A, B) != 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

// StartsWithIgnoreCase: long Str, short Prefix > 4 chars (exercises full data comparison)
void test_StartsWithICase_LongStr_ShortPrefix_Over4(void)
{
    KString Str    = KStringCreate("Hello World!!", 13);
    KString Prefix = KStringCreate("hello wor", 9);
    TEST_ASSERT_TRUE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

// StartsWithIgnoreCase: long Str, short Prefix > 4 chars, mismatch
void test_StartsWithICase_LongStr_ShortPrefix_Over4_False(void)
{
    KString Str    = KStringCreate("Hello World!!", 13);
    KString Prefix = KStringCreate("hello xyz", 9);
    TEST_ASSERT_FALSE(KStringStartsWithIgnoreCase(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_CompareICase_EqualMixedCase);
    RUN_TEST(test_CompareICase_DifferentLength);
    RUN_TEST(test_CompareICase_DifferentContent);
    RUN_TEST(test_CompareICase_BothLong);
    RUN_TEST(test_CompareICase_NonAlpha);
    RUN_TEST(test_CompareICase_BoundaryChars);
    RUN_TEST(test_CompareICase_Empty);
    RUN_TEST(test_CompareICase_WithNonUtf8Encoding);
    RUN_TEST(test_CompareICase_DifferentEncodings_SameSize);
    RUN_TEST(test_CompareICase_LongWithNonUtf8);
    RUN_TEST(test_EqualsICase_True);
    RUN_TEST(test_EqualsICase_False);
    RUN_TEST(test_EqualsICase_Empty);
    RUN_TEST(test_StartsWithICase_True);
    RUN_TEST(test_StartsWithICase_False);
    RUN_TEST(test_StartsWithICase_EmptyPrefix);
    RUN_TEST(test_StartsWithICase_PrefixLongerThanStr);
    RUN_TEST(test_StartsWithICase_LongStr_ShortPrefix);
    RUN_TEST(test_StartsWithICase_BothLong);
    RUN_TEST(test_CompareICase_LongDifferentPrefix);
    RUN_TEST(test_CompareICase_LongSamePrefixDifferentContent);
    RUN_TEST(test_StartsWithICase_LongStr_ShortPrefix_Over4);
    RUN_TEST(test_StartsWithICase_LongStr_ShortPrefix_Over4_False);
    return UNITY_END();
}
