#include "unity.h"
#include "KString.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// CStr returns correct content for short string
void test_CStr_ShortString(void)
{
    KString Str = KStringCreate("Hello", 5);
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Str));
    KStringDestroy(Str);
}

// CStr returns correct content for long string
void test_CStr_LongString(void)
{
    KString Str = KStringCreate("Hello World!!", 13);
    TEST_ASSERT_EQUAL_STRING("Hello World!!", KStringCStr(Str));
    KStringDestroy(Str);
}

// CStr returns empty string for empty KString
void test_CStr_EmptyString(void)
{
    KString Str = KStringCreate("", 0);
    TEST_ASSERT_EQUAL_STRING("", KStringCStr(Str));
    KStringDestroy(Str);
}

// CStr returns NULL for invalid string
void test_CStr_InvalidString(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_NULL(KStringCStr(Inv));
}

// CStr short string is properly null-terminated
void test_CStr_ShortNullTerminated(void)
{
    KString     Str  = KStringCreate("abc", 3);
    const char* pCStr = KStringCStr(Str);
    TEST_ASSERT_EQUAL_CHAR('\0', pCStr[3]);
    KStringDestroy(Str);
}

// CStr rotating buffers - 4 calls on short strings should all be valid
void test_CStr_RotatingBuffers(void)
{
    KString S1 = KStringCreate("aaa", 3);
    KString S2 = KStringCreate("bbb", 3);
    KString S3 = KStringCreate("ccc", 3);
    KString S4 = KStringCreate("ddd", 3);

    const char* p1 = KStringCStr(S1);
    const char* p2 = KStringCStr(S2);
    const char* p3 = KStringCStr(S3);
    const char* p4 = KStringCStr(S4);

    // All 4 should still be valid (rotating buffer of 4)
    TEST_ASSERT_EQUAL_STRING("aaa", p1);
    TEST_ASSERT_EQUAL_STRING("bbb", p2);
    TEST_ASSERT_EQUAL_STRING("ccc", p3);
    TEST_ASSERT_EQUAL_STRING("ddd", p4);

    KStringDestroy(S1);
    KStringDestroy(S2);
    KStringDestroy(S3);
    KStringDestroy(S4);
}

// Size for short string
void test_Size_Short(void)
{
    KString Str = KStringCreate("test", 4);
    TEST_ASSERT_EQUAL_UINT(4, KStringSize(Str));
    KStringDestroy(Str);
}

// Size for long string
void test_Size_Long(void)
{
    KString Str = KStringCreate("1234567890123", 13);
    TEST_ASSERT_EQUAL_UINT(13, KStringSize(Str));
    KStringDestroy(Str);
}

// Size for empty string
void test_Size_Empty(void)
{
    KString Str = KStringCreate("", 0);
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Str));
    KStringDestroy(Str);
}

// Size for invalid string
void test_Size_Invalid(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Inv));
}

// Size for boundary 12
void test_Size_Boundary12(void)
{
    KString Str = KStringCreate("123456789012", 12);
    TEST_ASSERT_EQUAL_UINT(12, KStringSize(Str));
    KStringDestroy(Str);
}

// GetEncoding default is UTF-8
void test_GetEncoding_Default(void)
{
    KString Str = KStringCreate("test", 4);
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// GetEncoding UTF-16LE
void test_GetEncoding_Utf16Le(void)
{
    const char Data[] = {0x48, 0x00};
    KString    Str    = KStringCreateWithEncoding(Data, 2, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// GetEncoding UTF-16BE
void test_GetEncoding_Utf16Be(void)
{
    const char Data[] = {0x00, 0x48};
    KString    Str    = KStringCreateWithEncoding(Data, 2, KSTRING_ENCODING_UTF16BE);
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// GetEncoding ANSI
void test_GetEncoding_Ansi(void)
{
    KString Str = KStringCreateWithEncoding("x", 1, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// GetEncoding on invalid returns UTF8
void test_GetEncoding_Invalid(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Inv));
}

// IsShort true for <=12
void test_IsShort_Short(void)
{
    KString Str = KStringCreate("123456789012", 12);
    TEST_ASSERT_TRUE(KStringIsShort(Str));
    KStringDestroy(Str);
}

// IsShort false for >12
void test_IsShort_Long(void)
{
    KString Str = KStringCreate("1234567890123", 13);
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    KStringDestroy(Str);
}

// IsShort true for empty
void test_IsShort_Empty(void)
{
    KString Str = KStringCreate("", 0);
    TEST_ASSERT_TRUE(KStringIsShort(Str));
    KStringDestroy(Str);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_CStr_ShortString);
    RUN_TEST(test_CStr_LongString);
    RUN_TEST(test_CStr_EmptyString);
    RUN_TEST(test_CStr_InvalidString);
    RUN_TEST(test_CStr_ShortNullTerminated);
    RUN_TEST(test_CStr_RotatingBuffers);
    RUN_TEST(test_Size_Short);
    RUN_TEST(test_Size_Long);
    RUN_TEST(test_Size_Empty);
    RUN_TEST(test_Size_Invalid);
    RUN_TEST(test_Size_Boundary12);
    RUN_TEST(test_GetEncoding_Default);
    RUN_TEST(test_GetEncoding_Utf16Le);
    RUN_TEST(test_GetEncoding_Utf16Be);
    RUN_TEST(test_GetEncoding_Ansi);
    RUN_TEST(test_GetEncoding_Invalid);
    RUN_TEST(test_IsShort_Short);
    RUN_TEST(test_IsShort_Long);
    RUN_TEST(test_IsShort_Empty);
    return UNITY_END();
}
