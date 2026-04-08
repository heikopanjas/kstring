#include "unity.h"
#include "KString.h"
#include <stdint.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// sizeof(KString) must be exactly 16 bytes
void test_StructSize_Is16Bytes(void)
{
    TEST_ASSERT_EQUAL_UINT(16, sizeof(KString));
}

// KStringInvalid returns a struct with Size == UINT32_MAX
void test_Invalid_SizeIsUint32Max(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_EQUAL_UINT32(UINT32_MAX, Inv.Size);
}

// KStringIsValid returns false for invalid string
void test_IsValid_Invalid(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_FALSE(KStringIsValid(Inv));
}

// KStringIsValid returns true for a valid short string
void test_IsValid_ValidShort(void)
{
    KString Str = KStringCreate("Hello", 5);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    KStringDestroy(Str);
}

// KStringIsValid returns true for a valid long string
void test_IsValid_ValidLong(void)
{
    KString Str = KStringCreate("Hello World!!", 13);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    KStringDestroy(Str);
}

// Invalid string content bytes are zeroed
void test_Invalid_ContentZeroed(void)
{
    KString Inv = KStringInvalid();
    char    Zero[12];
    memset(Zero, 0, 12);
    TEST_ASSERT_EQUAL_MEMORY(Zero, Inv.Content, 12);
}

// CStr on invalid returns NULL
void test_Operations_OnInvalid_CStr(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_NULL(KStringCStr(Inv));
}

// Size on invalid returns 0
void test_Operations_OnInvalid_Size(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Inv));
}

// GetEncoding on invalid returns UTF8 as default
void test_Operations_OnInvalid_Encoding(void)
{
    KString Inv = KStringInvalid();
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Inv));
}

// Concat with invalid first operand returns invalid
void test_Operations_OnInvalid_ConcatFirst(void)
{
    KString Inv   = KStringInvalid();
    KString Valid = KStringCreate("abc", 3);
    KString Res   = KStringConcat(Inv, Valid);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Valid);
}

// Concat with invalid second operand returns invalid
void test_Operations_OnInvalid_ConcatSecond(void)
{
    KString Valid = KStringCreate("abc", 3);
    KString Inv   = KStringInvalid();
    KString Res   = KStringConcat(Valid, Inv);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Valid);
}

// Substring of invalid returns invalid
void test_Operations_OnInvalid_Substring(void)
{
    KString Inv = KStringInvalid();
    KString Res = KStringSubstring(Inv, 0, 1);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
}

// Convert invalid string returns invalid
void test_Operations_OnInvalid_Convert(void)
{
    KString Inv = KStringInvalid();
    KString Res = KStringConvertToEncoding(Inv, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_StructSize_Is16Bytes);
    RUN_TEST(test_Invalid_SizeIsUint32Max);
    RUN_TEST(test_IsValid_Invalid);
    RUN_TEST(test_IsValid_ValidShort);
    RUN_TEST(test_IsValid_ValidLong);
    RUN_TEST(test_Invalid_ContentZeroed);
    RUN_TEST(test_Operations_OnInvalid_CStr);
    RUN_TEST(test_Operations_OnInvalid_Size);
    RUN_TEST(test_Operations_OnInvalid_Encoding);
    RUN_TEST(test_Operations_OnInvalid_ConcatFirst);
    RUN_TEST(test_Operations_OnInvalid_ConcatSecond);
    RUN_TEST(test_Operations_OnInvalid_Substring);
    RUN_TEST(test_Operations_OnInvalid_Convert);
    return UNITY_END();
}
