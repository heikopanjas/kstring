#include "unity.h"
#include "KString.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Destroy short string - no crash (short strings have no heap allocation)
void test_Destroy_ShortString(void)
{
    KString Str = KStringCreate("short", 5);
    KStringDestroy(Str);
    // If we get here without crashing, test passes
    TEST_PASS();
}

// Destroy long temporary string - frees memory
void test_Destroy_LongTemporary(void)
{
    KString Str = KStringCreate("This is a long string", 21);
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    KStringDestroy(Str);
    TEST_PASS();
}

// Destroy long persistent string - no free, no crash
void test_Destroy_LongPersistent(void)
{
    static const char Source[] = "Persistent string!!";
    KString           Str     = KStringCreatePersistent(Source, 19);
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    KStringDestroy(Str);
    // Source should still be valid
    TEST_ASSERT_EQUAL_STRING("Persistent string!!", Source);
}

// Destroy long transient string - no free, no crash
void test_Destroy_LongTransient(void)
{
    static const char Source[] = "Transient string!!";
    KString           Str     = KStringCreateTransient(Source, 18);
    TEST_ASSERT_FALSE(KStringIsShort(Str));
    KStringDestroy(Str);
    TEST_ASSERT_EQUAL_STRING("Transient string!!", Source);
}

// Destroy invalid string - no crash
void test_Destroy_InvalidString(void)
{
    KString Inv = KStringInvalid();
    KStringDestroy(Inv);
    TEST_PASS();
}

// Destroy empty string - no crash
void test_Destroy_EmptyString(void)
{
    KString Str = KStringCreate("", 0);
    KStringDestroy(Str);
    TEST_PASS();
}

// Verify temporary storage class for KStringCreate (long)
void test_StorageClass_Temporary(void)
{
    KString     Str   = KStringCreate("KStringCreate long", 18);
    const char* pCStr = KStringCStr(Str);
    TEST_ASSERT_NOT_NULL(pCStr);
    TEST_ASSERT_EQUAL_STRING("KStringCreate long", pCStr);
    KStringDestroy(Str);
}

// Verify persistent storage class - CStr points to original data
void test_StorageClass_Persistent(void)
{
    static const char Source[] = "Persistent source!!";
    KString           Str     = KStringCreatePersistent(Source, 19);
    const char*       pCStr   = KStringCStr(Str);
    // For persistent long strings, CStr should point to original
    TEST_ASSERT_EQUAL_PTR(Source, pCStr);
}

// Verify transient storage class - CStr points to original data
void test_StorageClass_Transient(void)
{
    static const char Source[] = "Transient source!!";
    KString           Str     = KStringCreateTransient(Source, 18);
    const char*       pCStr   = KStringCStr(Str);
    TEST_ASSERT_EQUAL_PTR(Source, pCStr);
}

// Concat result is temporary (needs destroy)
void test_Concat_ResultIsTemporary(void)
{
    KString A   = KStringCreate("Hello ", 6);
    KString B   = KStringCreate("World!!", 7);
    KString Res = KStringConcat(A, B);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_STRING("Hello World!!", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(A);
    KStringDestroy(B);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Destroy_ShortString);
    RUN_TEST(test_Destroy_LongTemporary);
    RUN_TEST(test_Destroy_LongPersistent);
    RUN_TEST(test_Destroy_LongTransient);
    RUN_TEST(test_Destroy_InvalidString);
    RUN_TEST(test_Destroy_EmptyString);
    RUN_TEST(test_StorageClass_Temporary);
    RUN_TEST(test_StorageClass_Persistent);
    RUN_TEST(test_StorageClass_Transient);
    RUN_TEST(test_Concat_ResultIsTemporary);
    return UNITY_END();
}
