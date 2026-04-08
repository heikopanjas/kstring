#include "unity.h"
#include "KString.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

//
// UTF-8 -> UTF-16LE
//

void test_Utf8ToUtf16Le_Ascii(void)
{
    KString Utf8   = KStringCreate("Hi", 2);
    KString Utf16  = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Utf16));
    // "Hi" -> 0x48,0x00 0x69,0x00 = 4 bytes
    TEST_ASSERT_EQUAL_UINT(4, KStringSize(Utf16));
    const char Expected[] = {0x48, 0x00, 0x69, 0x00};
    const char* pData = KStringCStr(Utf16);
    TEST_ASSERT_EQUAL_MEMORY(Expected, pData, 4);
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Le_2ByteChars(void)
{
    // u-umlaut: UTF-8 = 0xC3 0xBC, UTF-16LE = 0xFC 0x00
    const char Utf8Data[] = {(char)0xC3, (char)0xBC};
    KString    Utf8       = KStringCreate(Utf8Data, 2);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf16)); // 1 UTF-16 char = 2 bytes
    const char Expected[] = {(char)0xFC, 0x00};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf16), 2);
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Le_3ByteChars(void)
{
    // Euro sign: UTF-8 = 0xE2 0x82 0xAC, UTF-16LE = 0xAC 0x20
    const char Utf8Data[] = {(char)0xE2, (char)0x82, (char)0xAC};
    KString    Utf8       = KStringCreate(Utf8Data, 3);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf16));
    const char Expected[] = {(char)0xAC, 0x20};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf16), 2);
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Le_Empty(void)
{
    KString Utf8  = KStringCreate("", 0);
    KString Utf16 = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_UINT(0, KStringSize(Utf16));
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Le_WrongEncoding(void)
{
    KString Ansi  = KStringCreateWithEncoding("test", 4, KSTRING_ENCODING_ANSI);
    KString Utf16 = KStringConvertUtf8ToUtf16Le(Ansi);
    TEST_ASSERT_FALSE(KStringIsValid(Utf16));
    KStringDestroy(Ansi);
}

//
// UTF-16LE -> UTF-8
//

void test_Utf16LeToUtf8_Ascii(void)
{
    const char Utf16Data[] = {0x48, 0x00, 0x69, 0x00}; // "Hi"
    KString    Utf16       = KStringCreateWithEncoding(Utf16Data, 4, KSTRING_ENCODING_UTF16LE);
    KString    Utf8        = KStringConvertUtf16LeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Utf8));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf8));
    TEST_ASSERT_EQUAL_STRING("Hi", KStringCStr(Utf8));
    KStringDestroy(Utf8);
    KStringDestroy(Utf16);
}

void test_Utf16LeToUtf8_WrongEncoding(void)
{
    KString Utf8 = KStringCreate("test", 4);
    KString Res  = KStringConvertUtf16LeToUtf8(Utf8);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Utf8);
}

//
// UTF-8 <-> UTF-16BE
//

void test_Utf8ToUtf16Be_Simple(void)
{
    KString Utf8  = KStringCreate("Hi", 2);
    KString Utf16 = KStringConvertUtf8ToUtf16Be(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Utf16));
    // "Hi" in UTF-16BE: 0x00,0x48 0x00,0x69
    TEST_ASSERT_EQUAL_UINT(4, KStringSize(Utf16));
    const char Expected[] = {0x00, 0x48, 0x00, 0x69};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf16), 4);
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf16BeToUtf8_Simple(void)
{
    const char Utf16Data[] = {0x00, 0x48, 0x00, 0x69}; // "Hi" in UTF-16BE
    KString    Utf16       = KStringCreateWithEncoding(Utf16Data, 4, KSTRING_ENCODING_UTF16BE);
    KString    Utf8        = KStringConvertUtf16BeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf8));
    TEST_ASSERT_EQUAL_STRING("Hi", KStringCStr(Utf8));
    KStringDestroy(Utf8);
    KStringDestroy(Utf16);
}

//
// UTF-16LE <-> UTF-16BE byte swap
//

void test_Utf16LeToUtf16Be_Simple(void)
{
    const char LeData[] = {0x48, 0x00, 0x69, 0x00}; // "Hi" UTF-16LE
    KString    Le       = KStringCreateWithEncoding(LeData, 4, KSTRING_ENCODING_UTF16LE);
    KString    Be       = KStringConvertUtf16LeToUtf16Be(Le);
    TEST_ASSERT_TRUE(KStringIsValid(Be));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Be));
    const char Expected[] = {0x00, 0x48, 0x00, 0x69};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Be), 4);
    KStringDestroy(Be);
    KStringDestroy(Le);
}

void test_Utf16BeToUtf16Le_Simple(void)
{
    const char BeData[] = {0x00, 0x48, 0x00, 0x69}; // "Hi" UTF-16BE
    KString    Be       = KStringCreateWithEncoding(BeData, 4, KSTRING_ENCODING_UTF16BE);
    KString    Le       = KStringConvertUtf16BeToUtf16Le(Be);
    TEST_ASSERT_TRUE(KStringIsValid(Le));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Le));
    const char Expected[] = {0x48, 0x00, 0x69, 0x00};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Le), 4);
    KStringDestroy(Le);
    KStringDestroy(Be);
}

void test_Utf16_RoundTrip_LeBeLeEqualsOriginal(void)
{
    const char LeData[] = {0x48, 0x00, 0x69, 0x00, 0x21, 0x00}; // "Hi!"
    KString    Le1      = KStringCreateWithEncoding(LeData, 6, KSTRING_ENCODING_UTF16LE);
    KString    Be       = KStringConvertUtf16LeToUtf16Be(Le1);
    KString    Le2      = KStringConvertUtf16BeToUtf16Le(Be);
    TEST_ASSERT_TRUE(KStringIsValid(Le2));
    TEST_ASSERT_EQUAL_MEMORY(LeData, KStringCStr(Le2), 6);
    KStringDestroy(Le2);
    KStringDestroy(Be);
    KStringDestroy(Le1);
}

//
// UTF-8 <-> ANSI
//

void test_Utf8ToAnsi_AsciiOnly(void)
{
    KString Utf8 = KStringCreate("Hello", 5);
    KString Ansi = KStringConvertUtf8ToAnsi(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Ansi));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Ansi));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Ansi));
    TEST_ASSERT_EQUAL_MEMORY("Hello", KStringCStr(Ansi), 5);
    KStringDestroy(Ansi);
    KStringDestroy(Utf8);
}

void test_Utf8ToAnsi_Latin1Chars(void)
{
    // u-umlaut: UTF-8 = 0xC3 0xBC -> ANSI = 0xFC
    const char Utf8Data[] = {(char)0xC3, (char)0xBC};
    KString    Utf8       = KStringCreate(Utf8Data, 2);
    KString    Ansi       = KStringConvertUtf8ToAnsi(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Ansi));
    TEST_ASSERT_EQUAL_UINT(1, KStringSize(Ansi));
    TEST_ASSERT_EQUAL_UINT8(0xFC, (uint8_t)KStringCStr(Ansi)[0]);
    KStringDestroy(Ansi);
    KStringDestroy(Utf8);
}

void test_Utf8ToAnsi_UnmappableChars(void)
{
    // 3-byte UTF-8 char (e.g. Euro sign) becomes '?'
    const char Utf8Data[] = {(char)0xE2, (char)0x82, (char)0xAC};
    KString    Utf8       = KStringCreate(Utf8Data, 3);
    KString    Ansi       = KStringConvertUtf8ToAnsi(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Ansi));
    TEST_ASSERT_EQUAL_UINT(1, KStringSize(Ansi));
    TEST_ASSERT_EQUAL_CHAR('?', KStringCStr(Ansi)[0]);
    KStringDestroy(Ansi);
    KStringDestroy(Utf8);
}

void test_Utf8ToAnsi_WrongEncoding(void)
{
    KString Utf16 = KStringCreateWithEncoding("ab", 2, KSTRING_ENCODING_UTF16LE);
    KString Ansi  = KStringConvertUtf8ToAnsi(Utf16);
    TEST_ASSERT_FALSE(KStringIsValid(Ansi));
    KStringDestroy(Utf16);
}

void test_AnsiToUtf8_AsciiOnly(void)
{
    KString Ansi = KStringCreateWithEncoding("Hello", 5, KSTRING_ENCODING_ANSI);
    KString Utf8 = KStringConvertAnsiToUtf8(Ansi);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Utf8));
    TEST_ASSERT_EQUAL_UINT(5, KStringSize(Utf8));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Utf8));
    KStringDestroy(Utf8);
    KStringDestroy(Ansi);
}

void test_AnsiToUtf8_ExtendedChars(void)
{
    // ANSI 0xFC (u-umlaut) -> UTF-8 0xC3 0xBC
    const char AnsiData[] = {(char)0xFC};
    KString    Ansi       = KStringCreateWithEncoding(AnsiData, 1, KSTRING_ENCODING_ANSI);
    KString    Utf8       = KStringConvertAnsiToUtf8(Ansi);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf8));
    const char Expected[] = {(char)0xC3, (char)0xBC};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf8), 2);
    KStringDestroy(Utf8);
    KStringDestroy(Ansi);
}

void test_AnsiToUtf8_WrongEncoding(void)
{
    KString Utf8 = KStringCreate("test", 4);
    KString Res  = KStringConvertAnsiToUtf8(Utf8);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Utf8);
}

//
// KStringConvertToEncoding (generic dispatcher)
//

void test_ConvertToEncoding_SameEncoding(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Res = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF8, KStringGetEncoding(Res));
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf8ToUtf16Le(void)
{
    KString Str = KStringCreate("A", 1);
    KString Res = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Res));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Res)); // 1 char = 2 bytes in UTF-16
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf8ToAnsi(void)
{
    KString Str = KStringCreate("Hello", 5);
    KString Res = KStringConvertToEncoding(Str, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Res));
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf16LeToUtf8(void)
{
    const char Data[] = {0x41, 0x00}; // "A" in UTF-16LE
    KString    Str    = KStringCreateWithEncoding(Data, 2, KSTRING_ENCODING_UTF16LE);
    KString    Res    = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_STRING("A", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf16BeToUtf16Le(void)
{
    const char Data[] = {0x00, 0x41}; // "A" in UTF-16BE
    KString    Str    = KStringCreateWithEncoding(Data, 2, KSTRING_ENCODING_UTF16BE);
    KString    Res    = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Res));
    const char Expected[] = {0x41, 0x00};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Res), 2);
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Invalid(void)
{
    KString Inv = KStringInvalid();
    KString Res = KStringConvertToEncoding(Inv, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
}

void test_ConvertToEncoding_AnsiToUtf16Le(void)
{
    KString Ansi = KStringCreateWithEncoding("A", 1, KSTRING_ENCODING_ANSI);
    KString Res  = KStringConvertToEncoding(Ansi, KSTRING_ENCODING_UTF16LE);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16LE, KStringGetEncoding(Res));
    KStringDestroy(Res);
    KStringDestroy(Ansi);
}

//
// Round-trip tests
//

void test_RoundTrip_Utf8_Utf16Le_Utf8(void)
{
    KString Orig     = KStringCreate("Hello", 5);
    KString Utf16    = KStringConvertUtf8ToUtf16Le(Orig);
    KString RoundTrip = KStringConvertUtf16LeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(RoundTrip));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(RoundTrip));
    KStringDestroy(RoundTrip);
    KStringDestroy(Utf16);
    KStringDestroy(Orig);
}

void test_RoundTrip_Utf8_Ansi_Utf8_AsciiOnly(void)
{
    KString Orig     = KStringCreate("Hello", 5);
    KString Ansi     = KStringConvertUtf8ToAnsi(Orig);
    KString RoundTrip = KStringConvertAnsiToUtf8(Ansi);
    TEST_ASSERT_TRUE(KStringIsValid(RoundTrip));
    TEST_ASSERT_EQUAL_STRING("Hello", KStringCStr(RoundTrip));
    KStringDestroy(RoundTrip);
    KStringDestroy(Ansi);
    KStringDestroy(Orig);
}

void test_RoundTrip_Utf8_Utf16Be_Utf8(void)
{
    KString Orig     = KStringCreate("Test", 4);
    KString Utf16Be  = KStringConvertUtf8ToUtf16Be(Orig);
    KString RoundTrip = KStringConvertUtf16BeToUtf8(Utf16Be);
    TEST_ASSERT_TRUE(KStringIsValid(RoundTrip));
    TEST_ASSERT_EQUAL_STRING("Test", KStringCStr(RoundTrip));
    KStringDestroy(RoundTrip);
    KStringDestroy(Utf16Be);
    KStringDestroy(Orig);
}

//
// 4-byte UTF-8 (emoji) -> UTF-16 surrogate pairs
//

void test_Utf8ToUtf16Le_4ByteEmoji(void)
{
    // U+1F600 (grinning face): UTF-8 = F0 9F 98 80
    // UTF-16: surrogate pair D83D DE00
    const char Utf8Data[] = {(char)0xF0, (char)0x9F, (char)0x98, (char)0x80};
    KString    Utf8       = KStringCreate(Utf8Data, 4);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_UINT(4, KStringSize(Utf16)); // 2 UTF-16 code units = 4 bytes
    const char Expected[] = {(char)0x3D, (char)0xD8, (char)0x00, (char)0xDE};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf16), 4);
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

//
// UTF-16 surrogate pairs -> UTF-8
//

void test_Utf16LeToUtf8_SurrogatePair(void)
{
    // D83D DE00 -> U+1F600 -> UTF-8 F0 9F 98 80
    const char Utf16Data[] = {(char)0x3D, (char)0xD8, (char)0x00, (char)0xDE};
    KString    Utf16       = KStringCreateWithEncoding(Utf16Data, 4, KSTRING_ENCODING_UTF16LE);
    KString    Utf8        = KStringConvertUtf16LeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_UINT(4, KStringSize(Utf8));
    const char Expected[] = {(char)0xF0, (char)0x9F, (char)0x98, (char)0x80};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf8), 4);
    KStringDestroy(Utf8);
    KStringDestroy(Utf16);
}

void test_Utf16LeToUtf8_InvalidSurrogatePair(void)
{
    // High surrogate D800 followed by non-surrogate 0041 ('A')
    const char Utf16Data[] = {(char)0x00, (char)0xD8, (char)0x41, (char)0x00};
    KString    Utf16       = KStringCreateWithEncoding(Utf16Data, 4, KSTRING_ENCODING_UTF16LE);
    KString    Utf8        = KStringConvertUtf16LeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    // Invalid surrogate is skipped, then 'A' is output
    TEST_ASSERT_EQUAL_UINT(1, KStringSize(Utf8));
    TEST_ASSERT_EQUAL_STRING("A", KStringCStr(Utf8));
    KStringDestroy(Utf8);
    KStringDestroy(Utf16);
}

//
// UTF-16LE -> UTF-8: various code point ranges
//

void test_Utf16LeToUtf8_2ByteOutput(void)
{
    // U+00FC (u-umlaut): UTF-16LE = FC 00, UTF-8 = C3 BC (2-byte)
    const char Utf16Data[] = {(char)0xFC, (char)0x00};
    KString    Utf16       = KStringCreateWithEncoding(Utf16Data, 2, KSTRING_ENCODING_UTF16LE);
    KString    Utf8        = KStringConvertUtf16LeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf8));
    const char Expected[] = {(char)0xC3, (char)0xBC};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf8), 2);
    KStringDestroy(Utf8);
    KStringDestroy(Utf16);
}

void test_Utf16LeToUtf8_3ByteOutput(void)
{
    // U+20AC (Euro sign): UTF-16LE = AC 20, UTF-8 = E2 82 AC (3-byte)
    const char Utf16Data[] = {(char)0xAC, (char)0x20};
    KString    Utf16       = KStringCreateWithEncoding(Utf16Data, 2, KSTRING_ENCODING_UTF16LE);
    KString    Utf8        = KStringConvertUtf16LeToUtf8(Utf16);
    TEST_ASSERT_TRUE(KStringIsValid(Utf8));
    TEST_ASSERT_EQUAL_UINT(3, KStringSize(Utf8));
    const char Expected[] = {(char)0xE2, (char)0x82, (char)0xAC};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Utf8), 3);
    KStringDestroy(Utf8);
    KStringDestroy(Utf16);
}

//
// Truncated UTF-8 sequences
//

void test_Utf8ToUtf16Le_Truncated2Byte(void)
{
    // Start of 2-byte sequence (0xC3) but no continuation byte
    const char Utf8Data[] = {'A', (char)0xC3};
    KString    Utf8       = KStringCreate(Utf8Data, 2);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    // Only 'A' should be converted, truncated sequence is dropped
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf16));
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Le_Truncated3Byte(void)
{
    // Start of 3-byte sequence (0xE2 0x82) but missing last byte
    const char Utf8Data[] = {'A', (char)0xE2, (char)0x82};
    KString    Utf8       = KStringCreate(Utf8Data, 3);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf16)); // Only 'A'
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Le_Truncated4Byte(void)
{
    // Start of 4-byte sequence (0xF0 0x9F 0x98) but missing last byte
    const char Utf8Data[] = {'A', (char)0xF0, (char)0x9F, (char)0x98};
    KString    Utf8       = KStringCreate(Utf8Data, 4);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Utf16)); // Only 'A'
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

//
// Invalid UTF-8 byte
//

void test_Utf8ToUtf16Le_InvalidByte(void)
{
    // 0xFF is not a valid UTF-8 start byte - hits else branch with BytesRead=1
    // CodePoint stays 0, gets emitted as U+0000 (NULL), then 'B' follows
    const char Utf8Data[] = {'A', (char)0xFF, 'B'};
    KString    Utf8       = KStringCreate(Utf8Data, 3);
    KString    Utf16      = KStringConvertUtf8ToUtf16Le(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Utf16));
    // 'A' + U+0000 (from invalid byte) + 'B' = 3 UTF-16 chars = 6 bytes
    TEST_ASSERT_EQUAL_UINT(6, KStringSize(Utf16));
    KStringDestroy(Utf16);
    KStringDestroy(Utf8);
}

//
// UTF-8 -> ANSI: 2-byte char > 0xFF and 4-byte char
//

void test_Utf8ToAnsi_2ByteAbove0xFF(void)
{
    // U+0100 (Latin A with macron): UTF-8 = C4 80, code point 0x100 > 0xFF -> '?'
    const char Utf8Data[] = {(char)0xC4, (char)0x80};
    KString    Utf8       = KStringCreate(Utf8Data, 2);
    KString    Ansi       = KStringConvertUtf8ToAnsi(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Ansi));
    TEST_ASSERT_EQUAL_UINT(1, KStringSize(Ansi));
    TEST_ASSERT_EQUAL_CHAR('?', KStringCStr(Ansi)[0]);
    KStringDestroy(Ansi);
    KStringDestroy(Utf8);
}

void test_Utf8ToAnsi_4ByteChar(void)
{
    // 4-byte UTF-8 (emoji): F0 9F 98 80 -> '?' in ANSI
    const char Utf8Data[] = {(char)0xF0, (char)0x9F, (char)0x98, (char)0x80};
    KString    Utf8       = KStringCreate(Utf8Data, 4);
    KString    Ansi       = KStringConvertUtf8ToAnsi(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Ansi));
    TEST_ASSERT_EQUAL_UINT(1, KStringSize(Ansi));
    TEST_ASSERT_EQUAL_CHAR('?', KStringCStr(Ansi)[0]);
    KStringDestroy(Ansi);
    KStringDestroy(Utf8);
}

void test_Utf8ToAnsi_InvalidStartByte(void)
{
    // 0x80 is a bare continuation byte (not a valid start), hits the else i+=1 branch
    const char Utf8Data[] = {'A', (char)0x80, 'B'};
    KString    Utf8       = KStringCreate(Utf8Data, 3);
    KString    Ansi       = KStringConvertUtf8ToAnsi(Utf8);
    TEST_ASSERT_TRUE(KStringIsValid(Ansi));
    // 'A' + '?' (from 0x80 hitting else branch) + 'B' = 3 chars
    TEST_ASSERT_EQUAL_UINT(3, KStringSize(Ansi));
    TEST_ASSERT_EQUAL_CHAR('A', KStringCStr(Ansi)[0]);
    TEST_ASSERT_EQUAL_CHAR('?', KStringCStr(Ansi)[1]);
    TEST_ASSERT_EQUAL_CHAR('B', KStringCStr(Ansi)[2]);
    KStringDestroy(Ansi);
    KStringDestroy(Utf8);
}

//
// ConvertToEncoding dispatcher: indirect conversion paths
//

void test_ConvertToEncoding_Utf16LeToAnsi(void)
{
    const char Data[] = {0x48, 0x00, 0x69, 0x00}; // "Hi" in UTF-16LE
    KString    Str    = KStringCreateWithEncoding(Data, 4, KSTRING_ENCODING_UTF16LE);
    KString    Res    = KStringConvertToEncoding(Str, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Res));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Res));
    TEST_ASSERT_EQUAL_MEMORY("Hi", KStringCStr(Res), 2);
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf16BeToUtf8(void)
{
    const char Data[] = {0x00, 0x48, 0x00, 0x69}; // "Hi" in UTF-16BE
    KString    Str    = KStringCreateWithEncoding(Data, 4, KSTRING_ENCODING_UTF16BE);
    KString    Res    = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_STRING("Hi", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf16BeToAnsi(void)
{
    const char Data[] = {0x00, 0x48, 0x00, 0x69}; // "Hi" in UTF-16BE
    KString    Str    = KStringCreateWithEncoding(Data, 4, KSTRING_ENCODING_UTF16BE);
    KString    Res    = KStringConvertToEncoding(Str, KSTRING_ENCODING_ANSI);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_ANSI, KStringGetEncoding(Res));
    TEST_ASSERT_EQUAL_UINT(2, KStringSize(Res));
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_AnsiToUtf8(void)
{
    KString Ansi = KStringCreateWithEncoding("Hi", 2, KSTRING_ENCODING_ANSI);
    KString Res  = KStringConvertToEncoding(Ansi, KSTRING_ENCODING_UTF8);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_STRING("Hi", KStringCStr(Res));
    KStringDestroy(Res);
    KStringDestroy(Ansi);
}

void test_ConvertToEncoding_AnsiToUtf16Be(void)
{
    KString Ansi = KStringCreateWithEncoding("A", 1, KSTRING_ENCODING_ANSI);
    KString Res  = KStringConvertToEncoding(Ansi, KSTRING_ENCODING_UTF16BE);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Res));
    const char Expected[] = {0x00, 0x41};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Res), 2);
    KStringDestroy(Res);
    KStringDestroy(Ansi);
}

void test_ConvertToEncoding_Utf16LeToUtf16Be(void)
{
    const char Data[] = {0x48, 0x00, 0x69, 0x00}; // "Hi" in UTF-16LE
    KString    Str    = KStringCreateWithEncoding(Data, 4, KSTRING_ENCODING_UTF16LE);
    KString    Res    = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF16BE);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Res));
    const char Expected[] = {0x00, 0x48, 0x00, 0x69};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Res), 4);
    KStringDestroy(Res);
    KStringDestroy(Str);
}

void test_ConvertToEncoding_Utf8ToUtf16Be(void)
{
    KString Str = KStringCreate("A", 1);
    KString Res = KStringConvertToEncoding(Str, KSTRING_ENCODING_UTF16BE);
    TEST_ASSERT_TRUE(KStringIsValid(Res));
    TEST_ASSERT_EQUAL_INT(KSTRING_ENCODING_UTF16BE, KStringGetEncoding(Res));
    const char Expected[] = {0x00, 0x41};
    TEST_ASSERT_EQUAL_MEMORY(Expected, KStringCStr(Res), 2);
    KStringDestroy(Res);
    KStringDestroy(Str);
}

//
// Wrong-encoding validation for byte-swap functions
//

void test_Utf16LeToUtf16Be_WrongEncoding(void)
{
    KString Utf8 = KStringCreate("test", 4);
    KString Res  = KStringConvertUtf16LeToUtf16Be(Utf8);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Utf8);
}

void test_Utf16BeToUtf16Le_WrongEncoding(void)
{
    KString Utf8 = KStringCreate("test", 4);
    KString Res  = KStringConvertUtf16BeToUtf16Le(Utf8);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Utf8);
}

void test_Utf8ToUtf16Be_WrongEncoding(void)
{
    KString Ansi = KStringCreateWithEncoding("test", 4, KSTRING_ENCODING_ANSI);
    KString Res  = KStringConvertUtf8ToUtf16Be(Ansi);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Ansi);
}

void test_Utf16BeToUtf8_WrongEncoding(void)
{
    KString Utf8 = KStringCreate("test", 4);
    KString Res  = KStringConvertUtf16BeToUtf8(Utf8);
    TEST_ASSERT_FALSE(KStringIsValid(Res));
    KStringDestroy(Utf8);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Utf8ToUtf16Le_Ascii);
    RUN_TEST(test_Utf8ToUtf16Le_2ByteChars);
    RUN_TEST(test_Utf8ToUtf16Le_3ByteChars);
    RUN_TEST(test_Utf8ToUtf16Le_Empty);
    RUN_TEST(test_Utf8ToUtf16Le_WrongEncoding);
    RUN_TEST(test_Utf16LeToUtf8_Ascii);
    RUN_TEST(test_Utf16LeToUtf8_WrongEncoding);
    RUN_TEST(test_Utf8ToUtf16Be_Simple);
    RUN_TEST(test_Utf16BeToUtf8_Simple);
    RUN_TEST(test_Utf16LeToUtf16Be_Simple);
    RUN_TEST(test_Utf16BeToUtf16Le_Simple);
    RUN_TEST(test_Utf16_RoundTrip_LeBeLeEqualsOriginal);
    RUN_TEST(test_Utf8ToAnsi_AsciiOnly);
    RUN_TEST(test_Utf8ToAnsi_Latin1Chars);
    RUN_TEST(test_Utf8ToAnsi_UnmappableChars);
    RUN_TEST(test_Utf8ToAnsi_WrongEncoding);
    RUN_TEST(test_AnsiToUtf8_AsciiOnly);
    RUN_TEST(test_AnsiToUtf8_ExtendedChars);
    RUN_TEST(test_AnsiToUtf8_WrongEncoding);
    RUN_TEST(test_ConvertToEncoding_SameEncoding);
    RUN_TEST(test_ConvertToEncoding_Utf8ToUtf16Le);
    RUN_TEST(test_ConvertToEncoding_Utf8ToAnsi);
    RUN_TEST(test_ConvertToEncoding_Utf16LeToUtf8);
    RUN_TEST(test_ConvertToEncoding_Utf16BeToUtf16Le);
    RUN_TEST(test_ConvertToEncoding_Invalid);
    RUN_TEST(test_ConvertToEncoding_AnsiToUtf16Le);
    RUN_TEST(test_RoundTrip_Utf8_Utf16Le_Utf8);
    RUN_TEST(test_RoundTrip_Utf8_Ansi_Utf8_AsciiOnly);
    RUN_TEST(test_RoundTrip_Utf8_Utf16Be_Utf8);
    RUN_TEST(test_Utf8ToUtf16Le_4ByteEmoji);
    RUN_TEST(test_Utf16LeToUtf8_SurrogatePair);
    RUN_TEST(test_Utf16LeToUtf8_InvalidSurrogatePair);
    RUN_TEST(test_Utf16LeToUtf8_2ByteOutput);
    RUN_TEST(test_Utf16LeToUtf8_3ByteOutput);
    RUN_TEST(test_Utf8ToUtf16Le_Truncated2Byte);
    RUN_TEST(test_Utf8ToUtf16Le_Truncated3Byte);
    RUN_TEST(test_Utf8ToUtf16Le_Truncated4Byte);
    RUN_TEST(test_Utf8ToUtf16Le_InvalidByte);
    RUN_TEST(test_Utf8ToAnsi_2ByteAbove0xFF);
    RUN_TEST(test_Utf8ToAnsi_4ByteChar);
    RUN_TEST(test_Utf8ToAnsi_InvalidStartByte);
    RUN_TEST(test_ConvertToEncoding_Utf16LeToAnsi);
    RUN_TEST(test_ConvertToEncoding_Utf16LeToUtf16Be);
    RUN_TEST(test_ConvertToEncoding_Utf16BeToUtf8);
    RUN_TEST(test_ConvertToEncoding_Utf16BeToAnsi);
    RUN_TEST(test_ConvertToEncoding_AnsiToUtf8);
    RUN_TEST(test_ConvertToEncoding_AnsiToUtf16Be);
    RUN_TEST(test_ConvertToEncoding_Utf8ToUtf16Be);
    RUN_TEST(test_Utf16LeToUtf16Be_WrongEncoding);
    RUN_TEST(test_Utf16BeToUtf16Le_WrongEncoding);
    RUN_TEST(test_Utf8ToUtf16Be_WrongEncoding);
    RUN_TEST(test_Utf16BeToUtf8_WrongEncoding);
    return UNITY_END();
}
