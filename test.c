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
 printf("%d tests passed, %d tests failed.\n", TestCnt - FailCnt, FailCnt);
 return 0;
}
