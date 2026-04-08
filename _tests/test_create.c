#include "unity.h"
#include "KString.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Short string creation (5 bytes)
void test_Create_ShortString(void)
{
    KString Str = KStringCreate("Hello", 5);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Str));
    TEST_ASSERT_TRUE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Str));
    KStringDestroy(Str);
}

// Empty string creation
void test_Create_EmptyString(void)
{
    KString Str = KStringCreate("", 0);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Str));
    TEST_ASSERT_TRUE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING("", KStringCStr(Str));
    KStringDestroy(Str);
}

// Exactly 12 bytes (maximum short)
void test_Create_ExactBoundary12(void)
{
    KString Str = KStringCreate("123456789012", 12);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(12, KStringSize(Str));
    TEST_ASSERT_TRUE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING("123456789012", KStringCStr(Str));
    KStringDestroy(Str);
}

// 13 bytes - first long string
void test_Create_FirstLongString13(void)
{
    KString Str = KStringCreate("1234567890123", 13);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(13, KStringSize(Str));
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING("1234567890123", KStringCStr(Str));
    KStringDestroy(Str);
}

// NULL pointer returns invalid
void test_Create_NullPointer(void)
{
    KString Str = KStringCreate(NULL, 5);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// Size overflow (>30-bit max) returns invalid
void test_Create_SizeOverflow(void)
{
    KString Str = KStringCreate("x", (size_t)KSTRING_SIZE_MASK + 1);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// Default encoding is UTF-8
void test_Create_DefaultEncoding(void)
{
    KString Str = KStringCreate("test", 4);
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// Large string content integrity
void test_Create_LargeString(void)
{
    char Buffer[1000];
    memset(Buffer, 'A', 1000);
    KString Str = KStringCreate(Buffer, 1000);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(1000, KStringSize(Str));
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    const char* pCStr = KStringCStr(Str);
    TEST_ASSERT_EQUAL_MEMORY(Buffer, pCStr, 1000);
    KStringDestroy(Str);
}

// Binary data with embedded NUL bytes
void test_Create_BinaryData(void)
{
    const char Data[] = "He\0lo";
    KString    Str    = KStringCreate(Data, 5);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Str));
    KStringDestroy(Str);
}

// Long string prefix field matches first 4 content bytes
void test_Create_LongPrefixStored(void)
{
    KString Str = KStringCreate("ABCDEFGHIJKLM", 13);
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_MEMORY("ABCD", Str.LongStr.Prefix, 4);
    KStringDestroy(Str);
}

// Persistent short string
void test_CreatePersistent_Short(void)
{
    KString Str = KStringCreatePersistent("Hi", 2);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Str));
    TEST_ASSERT_TRUE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING("Hi", KStringCStr(Str));
}

// Persistent long string - points to source, no allocation
void test_CreatePersistent_Long(void)
{
    static const char Source[] = "Persistent long!";
    KString           Str     = KStringCreatePersistent(Source, 16);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(16, KStringSize(Str));
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING(Source, KStringCStr(Str));
}

// Transient short string
void test_CreateTransient_Short(void)
{
    KString Str = KStringCreateTransient("abc", 3);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(3, KStringSize(Str));
    TEST_ASSERT_TRUE(KStringIsShort(Str));
}

// Transient long string - points to source
void test_CreateTransient_Long(void)
{
    static const char Source[] = "Transient string";
    KString           Str     = KStringCreateTransient(Source, 16);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(16, KStringSize(Str));
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    TEST_ASSERT_EQUAL_STRING(Source, KStringCStr(Str));
}

// Create with encoding UTF-16LE
void test_CreateWithEncoding_Utf16Le(void)
{
    const char Data[] = {0x48, 0x00, 0x69, 0x00}; // "Hi" in UTF-16LE
    KString    Str    = KStringCreateWithEncoding(Data, 4, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Str));
    TEST_ASSERT_EQUAL_UINT(4, KStringSize(Str));
    KStringDestroy(Str);
}

// Create with encoding UTF-16BE
void test_CreateWithEncoding_Utf16Be(void)
{
    const char Data[] = {0x00, 0x48, 0x00, 0x69}; // "Hi" in UTF-16BE
    KString    Str    = KStringCreateWithEncoding(Data, 4, KSTRING_ENCODING_UTF16BE);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// Create with encoding ANSI
void test_CreateWithEncoding_Ansi(void)
{
    KString Str = KStringCreateWithEncoding("test", 4, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Str));
    KStringDestroy(Str);
}

// Persistent with encoding
void test_CreatePersistentWithEncoding(void)
{
    KString Str = KStringCreatePersistentWithEncoding("data", 4, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Str));
}

// Transient with encoding
void test_CreateTransientWithEncoding(void)
{
    static const char Source[] = "transient enc!!.";
    KString Str = KStringCreateTransientWithEncoding(Source, 16, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Str));
    TEST_ASSERT_EQUAL_UINT(16, KStringSize(Str));
}

// CreateFromCStr simple
void test_CreateFromCStr_Simple(void)
{
    KString Str = KStringCreateFromCStr("Hello World");
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(11, KStringSize(Str));
    TEST_ASSERT_EQUAL_STRING("Hello World", KStringCStr(Str));
    KStringDestroy(Str);
}

// CreateFromCStr empty
void test_CreateFromCStr_Empty(void)
{
    KString Str = KStringCreateFromCStr("");
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Str));
    KStringDestroy(Str);
}

// CreateFromCStr NULL
void test_CreateFromCStr_Null(void)
{
    KString Str = KStringCreateFromCStr(NULL);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// CreatePersistentFromCStr
void test_CreatePersistentFromCStr(void)
{
    KString Str = KStringCreatePersistentFromCStr("persistent test");
    TEST_ASSERT_TRUE(KStringIsValid(Str));
    TEST_ASSERT_EQUAL_UINT(15, KStringSize(Str));
}

// CreatePersistentFromCStr NULL
void test_CreatePersistentFromCStr_Null(void)
{
    KString Str = KStringCreatePersistentFromCStr(NULL);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// NULL pointer with persistent
void test_CreatePersistent_Null(void)
{
    KString Str = KStringCreatePersistent(NULL, 5);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// NULL pointer with transient
void test_CreateTransient_Null(void)
{
    KString Str = KStringCreateTransient(NULL, 5);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// Size overflow for PersistentWithEncoding
void test_CreatePersistentWithEncoding_SizeOverflow(void)
{
    KString Str = KStringCreatePersistentWithEncoding("x", (size_t)KSTRING_SIZE_MASK + 1, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// Size overflow for TransientWithEncoding
void test_CreateTransientWithEncoding_SizeOverflow(void)
{
    KString Str = KStringCreateTransientWithEncoding("x", (size_t)KSTRING_SIZE_MASK + 1, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// NULL pointer with PersistentWithEncoding
void test_CreatePersistentWithEncoding_Null(void)
{
    KString Str = KStringCreatePersistentWithEncoding(NULL, 5, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

// NULL pointer with TransientWithEncoding
void test_CreateTransientWithEncoding_Null(void)
{
    KString Str = KStringCreateTransientWithEncoding(NULL, 5, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_FALSE(KStringIsValid(Str));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Create_ShortString);
    RUN_TEST(test_Create_EmptyString);
    RUN_TEST(test_Create_ExactBoundary12);
    RUN_TEST(test_Create_FirstLongString13);
    RUN_TEST(test_Create_NullPointer);
    RUN_TEST(test_Create_SizeOverflow);
    RUN_TEST(test_Create_DefaultEncoding);
    RUN_TEST(test_Create_LargeString);
    RUN_TEST(test_Create_BinaryData);
    RUN_TEST(test_Create_LongPrefixStored);
    RUN_TEST(test_CreatePersistent_Short);
    RUN_TEST(test_CreatePersistent_Long);
    RUN_TEST(test_CreateTransient_Short);
    RUN_TEST(test_CreateTransient_Long);
    RUN_TEST(test_CreateWithEncoding_Utf16Le);
    RUN_TEST(test_CreateWithEncoding_Utf16Be);
    RUN_TEST(test_CreateWithEncoding_Ansi);
    RUN_TEST(test_CreatePersistentWithEncoding);
    RUN_TEST(test_CreateTransientWithEncoding);
    RUN_TEST(test_CreateFromCStr_Simple);
    RUN_TEST(test_CreateFromCStr_Empty);
    RUN_TEST(test_CreateFromCStr_Null);
    RUN_TEST(test_CreatePersistentFromCStr);
    RUN_TEST(test_CreatePersistentFromCStr_Null);
    RUN_TEST(test_CreatePersistent_Null);
    RUN_TEST(test_CreateTransient_Null);
    RUN_TEST(test_CreatePersistentWithEncoding_SizeOverflow);
    RUN_TEST(test_CreateTransientWithEncoding_SizeOverflow);
    RUN_TEST(test_CreatePersistentWithEncoding_Null);
    RUN_TEST(test_CreateTransientWithEncoding_Null);
    return UNITY_END();
}
