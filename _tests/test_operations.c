#include "unity.h"
#include "KString.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

//
// KStringConcat tests
//

void test_Concat_TwoShort_ResultShort(void)
{
    KString A   = KStringCreate("Hi", 2);
    KString B   = KStringCreate("!", 1);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_UINT(3, KStringSize(Res));
    TEST_ASSERT_TRUE(KStringIsShort(Res));
    TEST_ASSERT_EQUAL_STRING("Hi!", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_TwoShort_ResultLong(void)
{
    KString A   = KStringCreate("Hello ", 6);
    KString B   = KStringCreate("World!!", 7);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_UINT(13, KStringSize(Res));
    TEST_ASSERT_FALSE(KStringIsShort(Res));
    TEST_ASSERT_EQUAL_STRING("Hello World!!", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_ShortAndLong(void)
{
    KString A   = KStringCreate("Hi ", 3);
    KString B   = KStringCreate("1234567890123", 13);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_UINT(16, KStringSize(Res));
    TEST_ASSERT_EQUAL_STRING("Hi 1234567890123", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_LongAndLong(void)
{
    KString A   = KStringCreate("ABCDEFGHIJKLM", 13);
    KString B   = KStringCreate("NOPQRSTUVWXYZ!", 14);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_UINT(27, KStringSize(Res));
    TEST_ASSERT_EQUAL_STRING("ABCDEFGHIJKLMNOPQRSTUVWXYZ!", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_WithEmpty(void)
{
    KString A   = KStringCreate("Hello", 5);
    KString B   = KStringCreate("", 0);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Res));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_EmptyAndEmpty(void)
{
    KString A   = KStringCreate("", 0);
    KString B   = KStringCreate("", 0);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_InvalidFirst(void)
{
    KString Inv = KStringInvalid();
    KString B   = KStringCreate("abc", 3);
    KString Res = KStringConcat(Inv, B);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(B);
}

void test_Concat_InvalidSecond(void)
{
    KString A   = KStringCreate("abc", 3);
    KString Inv = KStringInvalid();
    KString Res = KStringConcat(A, Inv);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(A);
}

void test_Concat_PreservesEncoding(void)
{
    KString A   = KStringCreateWithEncoding("AB", 2, KSTRING_ENCODING_ANSI);
    KString B   = KStringCreateWithEncoding("CD", 2, KSTRING_ENCODING_ANSI);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

void test_Concat_ContentIntegrity(void)
{
    KString A   = KStringCreate("ABCDEF", 6);
    KString B   = KStringCreate("GHIJKLM", 7);
    KString Res = KStringConcat(A, B);
    const char* pCStr = KStringCStr(Res);
    TEST_ASSERT_EQUAL_MEMORY("ABCDEFGHIJKLM", pCStr, 13);
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

//
// KStringSubstring tests
//

void test_Substring_FromShort(void)
{
    KString Str = KStringCreate("Hello World", 11);
    KString Sub = KStringSubstring(Str, 6, 5);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Sub));
    TEST_ASSERT_EQUAL_STRING("World", KStringCStr(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_FromLong(void)
{
    KString Str = KStringCreate("Hello World!!", 13);
    KString Sub = KStringSubstring(Str, 6, 7);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_EQUAL_UINT(7, KStringSize(Sub));
    TEST_ASSERT_EQUAL_STRING("World!!", KStringCStr(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_FullString(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Sub = KStringSubstring(Str, 0, 5);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Sub));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_SingleChar(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Sub = KStringSubstring(Str, 2, 1);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_EQUAL_UINT(1, KStringSize(Sub));
    TEST_ASSERT_EQUAL_STRING("l", KStringCStr(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_ZeroSize(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Sub = KStringSubstring(Str, 0, 0);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_OffsetAtEnd(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Sub = KStringSubstring(Str, 5, 1);
    TEST_ASSERT_FALSE(KStringIsValid(Sub)); // offset >= size
}

void test_Substring_OffsetBeyondEnd(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Sub = KStringSubstring(Str, 10, 1);
    TEST_ASSERT_FALSE(KStringIsValid(Sub));
    KStringDestroy(Str);
}

void test_Substring_SizeClamped(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Sub = KStringSubstring(Str, 3, 100);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Sub));
    TEST_ASSERT_EQUAL_STRING("lo", KStringCStr(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_InvalidInput(void)
{
    KString Inv = KStringInvalid();
    KString Sub = KStringSubstring(Inv, 0, 1);
    TEST_ASSERT_FALSE(KStringIsValid(Sub));
}

void test_Substring_ResultShortFromLong(void)
{
    KString Str = KStringCreate("Hello World!!", 13);
    KString Sub = KStringSubstring(Str, 0, 5);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_TRUE(KStringIsShort(Sub));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_ResultLongFromLong(void)
{
    char Buffer[100];
    memset(Buffer, 'X', 100);
    KString Str = KStringCreate(Buffer, 100);
    KString Sub = KStringSubstring(Str, 10, 50);
    TEST_ASSERT_TRUE(KStringIsValid(Sub));
    TEST_ASSERT_FALSE(KStringIsShort(Sub));
    TEST_ASSERT_EQUAL_UINT(50, KStringSize(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

void test_Substring_PreservesEncoding(void)
{
    KString Str = KStringCreateWithEncoding("Hello", 5, KSTRING_ENCODING_ANSI);
    KString Sub = KStringSubstring(Str, 0, 3);
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Sub));
    KStringDestroy(Sub);
    KStringDestroy(Str);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Concat_TwoShort_ResultShort);
    RUN_TEST(test_Concat_TwoShort_ResultLong);
    RUN_TEST(test_Concat_ShortAndLong);
    RUN_TEST(test_Concat_LongAndLong);
    RUN_TEST(test_Concat_WithEmpty);
    RUN_TEST(test_Concat_EmptyAndEmpty);
    RUN_TEST(test_Concat_InvalidFirst);
    RUN_TEST(test_Concat_InvalidSecond);
    RUN_TEST(test_Concat_PreservesEncoding);
    RUN_TEST(test_Concat_ContentIntegrity);
    RUN_TEST(test_Substring_FromShort);
    RUN_TEST(test_Substring_FromLong);
    RUN_TEST(test_Substring_FullString);
    RUN_TEST(test_Substring_SingleChar);
    RUN_TEST(test_Substring_ZeroSize);
    RUN_TEST(test_Substring_OffsetAtEnd);
    RUN_TEST(test_Substring_OffsetBeyondEnd);
    RUN_TEST(test_Substring_SizeClamped);
    RUN_TEST(test_Substring_InvalidInput);
    RUN_TEST(test_Substring_ResultShortFromLong);
    RUN_TEST(test_Substring_ResultLongFromLong);
    RUN_TEST(test_Substring_PreservesEncoding);
    return UNITY_END();
}
