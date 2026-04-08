#include "unity.h"
#include "KString.h"

void setUp(void) {}
void tearDown(void) {}

//
// KStringCompare tests
//

void test_Compare_EqualShort(void)
{
    KString A = KStringCreate("Hello", 5);
    KString B = KStringCreate("Hello", 5);
    TEST_ASSERT_EQUAL_INT(0, KStringCompare(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_EqualLong(void)
{
    KString A = KStringCreate("Hello World!!", 13);
    KString B = KStringCreate("Hello World!!", 13);
    TEST_ASSERT_EQUAL_INT(0, KStringCompare(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_DifferentLength(void)
{
    KString A = KStringCreate("abc", 3);
    KString B = KStringCreate("abcd", 4);
    TEST_ASSERT_TRUE(KStringCompare(A, B) < 0);
    TEST_ASSERT_TRUE(KStringCompare(B, A) > 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_DifferentContentShort(void)
{
    KString A = KStringCreate("abc", 3);
    KString B = KStringCreate("abd", 3);
    TEST_ASSERT_TRUE(KStringCompare(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_PrefixDiffers(void)
{
    KString A = KStringCreate("ABCDEFGHIJKLM", 13);
    KString B = KStringCreate("XBCDEFGHIJKLM", 13);
    TEST_ASSERT_TRUE(KStringCompare(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_PrefixMatchesContentDiffers(void)
{
    KString A = KStringCreate("ABCDXFGHIJKLM", 13);
    KString B = KStringCreate("ABCDYFGHIJKLM", 13);
    TEST_ASSERT_TRUE(KStringCompare(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_Empty(void)
{
    KString A = KStringCreate("", 0);
    KString B = KStringCreate("", 0);
    TEST_ASSERT_EQUAL_INT(0, KStringCompare(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_EmptyVsNonEmpty(void)
{
    KString A = KStringCreate("", 0);
    KString B = KStringCreate("a", 1);
    TEST_ASSERT_TRUE(KStringCompare(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Compare_ShortVsLong_DifferentLength(void)
{
    KString A = KStringCreate("123456789012", 12);
    KString B = KStringCreate("1234567890123", 13);
    TEST_ASSERT_TRUE(KStringCompare(A, B) < 0);
    KStringDestroy(A);
    KStringDestroy(B);
}

//
// KStringEquals tests
//

void test_Equals_True(void)
{
    KString A = KStringCreate("test", 4);
    KString B = KStringCreate("test", 4);
    TEST_ASSERT_TRUE(KStringEquals(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Equals_False(void)
{
    KString A = KStringCreate("test", 4);
    KString B = KStringCreate("tset", 4);
    TEST_ASSERT_FALSE(KStringEquals(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Equals_EmptyStrings(void)
{
    KString A = KStringCreate("", 0);
    KString B = KStringCreate("", 0);
    TEST_ASSERT_TRUE(KStringEquals(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Equals_LongVsLong(void)
{
    KString A = KStringCreate("1234567890123", 13);
    KString B = KStringCreate("1234567890123", 13);
    TEST_ASSERT_TRUE(KStringEquals(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Equals_DifferentLengths(void)
{
    KString A = KStringCreate("abc", 3);
    KString B = KStringCreate("abcd", 4);
    TEST_ASSERT_FALSE(KStringEquals(A, B));
    KStringDestroy(A);
    KStringDestroy(B);
}

//
// KStringStartsWith tests
//

void test_StartsWith_True(void)
{
    KString Str    = KStringCreate("Hello World", 11);
    KString Prefix = KStringCreate("Hello", 5);
    TEST_ASSERT_TRUE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_False(void)
{
    KString Str    = KStringCreate("Hello World", 11);
    KString Prefix = KStringCreate("World", 5);
    TEST_ASSERT_FALSE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_EmptyPrefix(void)
{
    KString Str    = KStringCreate("Hello", 5);
    KString Prefix = KStringCreate("", 0);
    TEST_ASSERT_TRUE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_PrefixLongerThanStr(void)
{
    KString Str    = KStringCreate("Hi", 2);
    KString Prefix = KStringCreate("Hello", 5);
    TEST_ASSERT_FALSE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_ExactMatch(void)
{
    KString Str    = KStringCreate("Hello", 5);
    KString Prefix = KStringCreate("Hello", 5);
    TEST_ASSERT_TRUE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_LongStr_ShortPrefix_Under4(void)
{
    KString Str    = KStringCreate("ABCDEFGHIJKLM", 13);
    KString Prefix = KStringCreate("ABC", 3);
    TEST_ASSERT_TRUE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_LongStr_ShortPrefix_Over4(void)
{
    KString Str    = KStringCreate("ABCDEFGHIJKLM", 13);
    KString Prefix = KStringCreate("ABCDEFGH", 8);
    TEST_ASSERT_TRUE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_LongStr_LongPrefix(void)
{
    KString Str    = KStringCreate("ABCDEFGHIJKLMNOP", 16);
    KString Prefix = KStringCreate("ABCDEFGHIJKLM", 13);
    TEST_ASSERT_TRUE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

void test_StartsWith_LongStr_LongPrefix_False(void)
{
    KString Str    = KStringCreate("ABCDEFGHIJKLMNOP", 16);
    KString Prefix = KStringCreate("ABCDXFGHIJKLM", 13);
    TEST_ASSERT_FALSE(KStringStartsWith(Str, Prefix));
    KStringDestroy(Str);
    KStringDestroy(Prefix);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Compare_EqualShort);
    RUN_TEST(test_Compare_EqualLong);
    RUN_TEST(test_Compare_DifferentLength);
    RUN_TEST(test_Compare_DifferentContentShort);
    RUN_TEST(test_Compare_PrefixDiffers);
    RUN_TEST(test_Compare_PrefixMatchesContentDiffers);
    RUN_TEST(test_Compare_Empty);
    RUN_TEST(test_Compare_EmptyVsNonEmpty);
    RUN_TEST(test_Compare_ShortVsLong_DifferentLength);
    RUN_TEST(test_Equals_True);
    RUN_TEST(test_Equals_False);
    RUN_TEST(test_Equals_EmptyStrings);
    RUN_TEST(test_Equals_LongVsLong);
    RUN_TEST(test_Equals_DifferentLengths);
    RUN_TEST(test_StartsWith_True);
    RUN_TEST(test_StartsWith_False);
    RUN_TEST(test_StartsWith_EmptyPrefix);
    RUN_TEST(test_StartsWith_PrefixLongerThanStr);
    RUN_TEST(test_StartsWith_ExactMatch);
    RUN_TEST(test_StartsWith_LongStr_ShortPrefix_Under4);
    RUN_TEST(test_StartsWith_LongStr_ShortPrefix_Over4);
    RUN_TEST(test_StartsWith_LongStr_LongPrefix);
    RUN_TEST(test_StartsWith_LongStr_LongPrefix_False);
    return UNITY_END();
}
