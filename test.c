#pragma once
#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "http.c"

#pragma comment(lib, "pathcch.lib")

static uint32_t
MimeLookupExtnTest()
{
 uint32_t Ret = 1;
 for (uint32_t I = 0; I < HttpArrLn(MimeTab); ++I)
 {
  char *ExtName = (char *)MimeTab[I][0];
  char *Mime = (char *)MimeTab[I][1];
  char Ext[32] = {0};

  Ext[0] = '.';
  strncat_s(Ext, HttpArrLn(Ext), ExtName, strlen(ExtName));

  wchar_t ExtW[32] = {0};
  mbstowcs_s(0, ExtW, HttpArrLn(ExtW), Ext, strlen(Ext));

  char *Res = MimeLookupExtnW(ExtW);
  if (!Res)
  {
   Ret = 0;
  }
  
  if (!Res)
  {
   printf("MimeLookupExtnW failed for: %S\n", ExtW);
  }
 }

 if (Ret)
 {
  printf("MimeLookupExtnW test passed!\n");
 }
 else
 {
  printf("MimeLookupExtnW test failed.\n");
 }

 return Ret;
}

static uint32_t
MimeLookupExtnFailTest()
{
 char *Res = MimeLookupExtnW(L".jpegasdfasdfasdf");
 if (Res)
 {
  printf("MimeLookupExtnW fail test failed.\n");
 }
 else
 {
  printf("MimeLookupExtnW fail test passed!\n");
 }
 return !Res;
}

static uint32_t
MimeLookupPathTest()
{
 wchar_t *Path = L".\\mime.jpeg";
 char *Res = MimeLookupPathW(Path, wcslen(Path) + 1);
 if (Res && !strcmp(Res, "image/jpeg"))
 {
  printf("MimeLookupPathW basic test passed!\n");
  return 1;
 }
 else
 {
  printf("MimeLookupPathW basic test failed.\n");
  return 0;
 }
}

static uint32_t
MimeLookupPathFailTest()
{
 wchar_t *Path = L".\\asdf";
 char *Res = MimeLookupPathW(Path, wcslen(Path) + 1);
 if (Res && !strcmp(Res, "application/octet-stream"))
 {
  printf("MimeLookupPathW fail test passed!\n");
  return 1;
 }
 else {
  printf("MimeLookupPathW fail test failed.\n");
  return 0;
 }
}

static uint32_t
HttpSegmentIterTest()
{
 uint32_t Ret = 1;
 // basic test
 {
  char *Str = "/asdf/asdf ";
  char *Start = Str;
  char *End = HttpFindNextSpace(Str, Str + strlen(Str));
  uint32_t Res;

  // iter 1
  Res = HttpSegmentIter(&Start, End);

  Ret &= Res;
  Ret &= Start == (Str + 5);

  // iter 2
  Res = HttpSegmentIter(&Start, End);

  Ret &= Res;
  Ret &= Start == End;

  // iter 3
  Res = HttpSegmentIter(&Start, End);

  Ret &= !Res;
  Ret &= Start == End;
 }

 if (Ret)
 {
  printf("HttpSegmentIterTest passed!\n");
 }
 else
 {
  printf("HttpSegmentIterTest failed.\n");
 }
 
 return Ret;
}

static uint32_t
HttpGetPathTest()
{
 uint32_t Ret = 1;
 // basic test
 {
  char * Path          = "/asdf HTTP/1.1";
  size_t PathLn        = strlen(Path);
  
  char * In            = Path;
  char * InEnd         = Path + PathLn; 
  char   Out[MAX_PATH] = {0};
  char * Expect        = "/asdf";
  size_t ExpectLn      = strlen(Expect);

  uint32_t WriteLn     = HttpGetPath(&In, InEnd, Out, sizeof(Out));

  Ret &= !!WriteLn;
  Ret &= In == (Path + 6);
  Ret &= StrEq(Out, WriteLn, Expect, ExpectLn);
 }

 // pct encoding test
 {
  char * Path          = "/as%20df HTTP/1.1";
  size_t PathLn        = strlen(Path);
  
  char * In            = Path;
  char * InEnd         = Path + PathLn; 
  char   Out[MAX_PATH] = {0};
  char * Expect        = "/as df";
  size_t ExpectLn      = strlen(Expect);

  uint32_t WriteLn     = HttpGetPath(&In, InEnd, Out, sizeof(Out));

  Ret &= !!WriteLn;
  Ret &= In == (Path + 9);
  Ret &= StrEq(Out, WriteLn, Expect, ExpectLn);
 }

 if (Ret)
 {
  printf("HttpGetPathTest passed!\n");
 }
 else
 {
  printf("HttpGetPathTest failed.\n");
 }
 
 return Ret;
}

static uint32_t
HttpParseRequestTest()
{
 uint32_t Ret = 1;

 // basic test
 {
  char *InStr =
   "GET /asdf HTTP/1.1\r\n"
   "user-agent: yaak\r\n"
   "accept: */*\r\n"
   "host: localhost:3000\r\n"
   "\r\n";
  size_t InStrLn = strlen(InStr);
  http_parse_ctx Ctx = {0};

  uint32_t Res = HttpParseRequest(InStr, InStrLn, &Ctx);

  char *ExpectPath = "/asdf";
  Ret &= Res;
  Ret &= Ctx.MethodType == HttpMethodGet;
  Ret &= StrEq(ExpectPath, strlen(ExpectPath), Ctx.Path, Ctx.PathLn);
 }

 if (Ret)
 {
  printf("HttpParseRequestTest passed!\n");
 }
 else
 {
  printf("HttpParseRequestTest failed.\n");
 }

 return Ret;
}

static uint32_t
Utf8TestCaseEx(char *Name, char *Str, size_t StrLn, uint32_t Expect)
{
 uint32_t Res = Utf8Validate(Str, StrLn);

 printf("Test '%s'... ", Name);
 if (Res == Expect)
 {
  puts("OK");
  return 1;
 }
 else
 {
  puts("FAIL");
  return 0;
 }
}

#define TestCase(name, string, length, expected)  do {if (!Utf8TestCaseEx(name, string, length, expected)) { ++Fail; }} while (0)

// tests from: https://github.com/hisahi/utf8chk/blob/master/utf8chk_test.c
static uint32_t
Utf8ValidateTest()
{
 uint32_t Fail = 0;
 TestCase("Empty string with implicit length", "", 0, 1);
 TestCase("Valid ASCII string with explicit length", "bar", 3, 1);
 TestCase("Valid ASCII string with explicit shorter length", "bar", 2, 1);
 TestCase("Valid UTF-8 string containing two-byte sequence with explicit length", "\xd2\x91", 2, 1);
 TestCase("Valid UTF-8 string containing three-byte sequence with explicit length", "\xe3\x83\x84", 3, 1);
 TestCase("Valid UTF-8 string containing four-byte sequence with explicit length", "\xf0\x9f\x98\x83", 4, 1);
 TestCase("Valid UTF-8 string with explicit length", "\xe8\xa9\x9e\xe8\xaa\x9e", 6, 1);
 TestCase("Valid UTF-8 string with explicit shorter length", "\xe8\xa9\x9e\xe8\xaa\x9e", 3, 1);
 TestCase("Valid UTF-8 string #2 with explicit length", "\xe8\xa9\x9e\xe8\xaa\x9e", 6, 1);
 TestCase("Valid UTF-8 string #2 with explicit shorter length", "\xe8\xa9\x9e\xe8\xaa\x9e", 3, 1);
 TestCase("Valid UTF-8 string #3 with explicit length", "\x48\x65\x6c\x6c\x6f\x20\x77\x6f\x72\x6c\x64\x2c\x20\xce\x9a\xce\xb1\xce\xbb\xce\xb7\xce\xbc\xe1\xbd\xb3\xcf\x81\xce\xb1\x20\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce\xb5\x2c\x20\xe3\x82\xb3\xe3\x83\xb3\xe3\x83\x8b\xe3\x83\x81\xe3\x83\x8f", 59, 1);
 TestCase("Last valid UTF-8 one-byte sequence", "\x7f", 1, 1);
 TestCase("First valid UTF-8 two-byte sequence", "\xc2\x80", 2, 1);
 TestCase("Last valid UTF-8 two-byte sequence", "\xdf\xbf", 2, 1);
 TestCase("First valid UTF-8 three-byte sequence", "\xe0\xa0\x80", 3, 1);
 TestCase("Last valid UTF-8 three-byte sequence (U+FFFF)", "\xef\xbf\xbf", 3, 1);
 TestCase("First valid UTF-8 four-byte sequence", "\xf0\x90\x80\x80", 4, 1);
 TestCase("Last valid UTF-8 four-byte sequence", "\xf4\x8f\xbf\xbf", 4, 1);
 TestCase("U+FFFD", "\xef\xbf\xbd", 3, 1);
 TestCase("Code point out of range (U+110000)", "\xf4\x90\x80\x80", 4, 0);
 TestCase("Code point way out of range (U+1FFFFF)", "\xf7\xbf\xbf\xbf", 4, 0);
 TestCase("Unexpected continuation character #1", "a\x80", 2, 0);
 TestCase("Unexpected continuation character #2", "\xbf", 1, 0);
 TestCase("Two-byte sequence cut short with explicit length", "\xc2", 1, 0);
 TestCase("Three-byte sequence cut short after 1 byte with explicit length", "\xe0", 1, 0);
 TestCase("Three-byte sequence cut short after 2 bytes with explicit length", "\xe0\xa0", 2, 0);
 TestCase("Four-byte sequence cut short after 1 byte with explicit length", "\xf0", 1, 0);
 TestCase("Four-byte sequence cut short after 2 bytes with explicit length", "\xf0\x90", 2, 0);
 TestCase("Four-byte sequence cut short after 3 bytes with explicit length", "\xf0\x90\x80", 3, 0);
 TestCase("Two-byte sequence cut short by another character", "\xc2\x62", 2, 0);
 TestCase("Three-byte sequence cut short by another character after 1 byte", "\xe0\x62\x62", 3, 0);
 TestCase("Three-byte sequence cut short by another character after 2 bytes", "\xe0\xa0\x62", 3, 0);
 TestCase("Four-byte sequence cut short by another character after 1 byte", "\xf0\x62\x62\x62", 4, 0);
 TestCase("Four-byte sequence cut short by another character after 2 bytes", "\xf0\x90\x62\x62", 4, 0);
 TestCase("Four-byte sequence cut short by another character after 3 bytes", "\xf0\x90\x80\x62", 4, 0);
 TestCase("Invalid start byte #1", "\xf8", 1, 0);
 TestCase("Invalid start byte #2", "\xff", 1, 0);
 // todo decide whether or not to allow noncharacters
 // TEST_CASE("Noncharacter #1 when allowed", "\xef\xbf\xbe", 3, 1);
 // TEST_CASE("Noncharacter #2 when allowed", "\xef\xb7\x90", 3, 1);
 // TEST_CASE("Noncharacter #3 when allowed", "\xef\xb7\xaf", 3, 1);
 // TEST_CASE("Noncharacter #4 when allowed", "\xf3\xbf\xbf\xbe", 4, 1);
 // TestCase("Noncharacter #1 when banned", "\xef\xbf\xbe", 3, 0);
 // TestCase("Noncharacter #2 when banned", "\xef\xb7\x90", 3, 0);
 // TestCase("Noncharacter #3 when banned", "\xef\xb7\xaf", 3, 0);
 // TestCase("Noncharacter #4 when banned", "\xf3\xbf\xbf\xbe", 4, 0);
 TestCase("Null byte banned with explicit length", "a\x00", 2, 0);
 TestCase("Minimum overlong two-byte sequence", "\xc0\x80", 2,  0);
 TestCase("Maximum overlong two-byte sequence", "\xc1\xbf", 2,  0);
 TestCase("Minimum overlong three-byte sequence", "\xe0\x80\x80", 3, 0);
 TestCase("Maximum overlong three-byte sequence", "\xe0\x9f\xbf", 3, 0);
 TestCase("Minimum overlong four-byte sequence", "\xf0\x80\x80\x80", 4, 0);
 TestCase("Maximum overlong four-byte sequence", "\xf0\x8f\xbf\xbf", 4, 0);
 TestCase("When overlong not banned", "\xe0\x9f\xbf", 3, 0);
 TestCase("C0 80 allowed", "\xc0\x80", 2, 0);
 TestCase("Minimum overlong two-byte sequence with C0 80 allowed", "\xc0\x81", 2, 0);
 TestCase("Three-byte null not allowed if C0 80 allowed", "\xe0\x80\x80", 3, 0);
 TestCase("Surrogates when banned", "\xed\xa0\x81\xed\xb0\x80", 6, 0);
 TestCase("Surrogates when allowed", "\xed\xa0\x81\xed\xb0\x80", 6, 0);
 TestCase("Surrogate truncated", "\xed\xa0\x81", 3, 0);
 TestCase("Low surrogate truncated by one byte", "\xed\xa0\x81\xed\xb0", 5, 0);
 TestCase("Low surrogate truncated by two bytes", "\xed\xa0\x81\xed", 4, 0);
 TestCase("Surrogate low before high", "\xed\xb0\x80\xed\xa0\x81", 6, 0);
 TestCase("Surrogate high-high", "\xed\xa0\x81\xed\xa0\x81", 6, 0);
 TestCase("Surrogate truncated without validation", "\xed\xa0\x81", 3, 0);
 TestCase("Surrogate low before high without validation", "\xed\xb0\x80\xed\xa0\x81", 6, 0);
 TestCase("Surrogate high-high without validation", "\xed\xa0\x81\xed\xa0\x81", 6, 0);
 if (Fail)
 {
  printf("%d utf8 tests failed.\n", Fail);
 }
 else
 {
  puts("All utf8 tests OK.");
 }
 return !Fail;
}

int wmain(int argc, wchar_t const *argv[])
{
 uint32_t FailCnt = 0;
 uint32_t TestCnt = 7;
 FailCnt += !MimeLookupExtnTest();
 FailCnt += !MimeLookupExtnFailTest();
 FailCnt += !MimeLookupPathTest();
 FailCnt += !MimeLookupPathFailTest();
 FailCnt += !HttpSegmentIterTest();
 FailCnt += !HttpGetPathTest();
 FailCnt += !HttpParseRequestTest();
 FailCnt += !Utf8ValidateTest();
 printf("%d tests passed, %d tests failed.\n", TestCnt - FailCnt, FailCnt);
 return 0;
}
