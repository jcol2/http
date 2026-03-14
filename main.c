#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

#define HttpStrLn(Str) (sizeof(Str) / sizeof(Str[0]) - 1)

#define Http100 "100"
#define Http101 "101"
#define Http102 "102"
#define Http200 "200"
#define Http201 "201"
#define Http202 "202"
#define Http203 "203"
#define Http204 "204"
#define Http205 "205"
#define Http206 "206"
#define Http207 "207"
#define Http208 "208"
#define Http226 "226"
#define Http300 "300"
#define Http301 "301"
#define Http302 "302"
#define Http303 "303"
#define Http304 "304"
#define Http305 "305"
#define Http307 "307"
#define Http308 "308"
#define Http400 "400"
#define Http401 "401"
#define Http402 "402"
#define Http403 "403"
#define Http404 "404"
#define Http405 "405"
#define Http406 "406"
#define Http407 "407"
#define Http408 "408"
#define Http409 "409"
#define Http410 "410"
#define Http411 "411"
#define Http412 "412"
#define Http413 "413"
#define Http414 "414"
#define Http415 "415"
#define Http416 "416"
#define Http417 "417"
#define Http418 "418"
#define Http421 "421"
#define Http422 "422"
#define Http423 "423"
#define Http424 "424"
#define Http425 "425"
#define Http426 "426"
#define Http428 "428"
#define Http429 "429"
#define Http431 "431"
#define Http451 "451"
#define Http500 "500"
#define Http501 "501"
#define Http502 "502"
#define Http503 "503"
#define Http504 "504"
#define Http505 "505"
#define Http506 "506"
#define Http507 "507"
#define Http508 "508"
#define Http510 "510"
#define Http511 "511"

// typedef struct http_fpath http_fpath;
// struct http_fpath
// {
//  wchar_t Path[MAX_PATH];
//  uint32_t Ln;
// };

// static http_fpath
// HttpResolveFpathFromWStr(wchar_t *Path)
// {
//  http_fpath Ret = {0};
//  Ret.Ln = GetFullPathNameW(Path, MAX_PATH, Ret.Path, 0);
//  return Ret;
// }

// static void
// HttpResolveReqFpath(http_ctx *Ctx, http_fpath *BaseDir, http_fpath *Out)
// {
//  wchar_t CatStr[MAX_PATH] = {0};
//  wcscat_s(CatStr, MAX_PATH, BaseDir->Path);
//  wcsncat_s(CatStr, MAX_PATH, Ctx->Req->CookedUrl.pAbsPath, Ctx->Req->CookedUrl.AbsPathLength);

//  Out->Ln = GetFullPathNameW(CatStr, MAX_PATH, Out->Path, 0);
//  if (Out->Ln < BaseDir->Ln)
//  {
//   *Out = (http_fpath){0};
//  }
// }

static void
HttpMemcpyAdvance(uint32_t *Run, void **Dst, size_t *DstLn, void *Src, size_t SrcLn)
{
 if (*Run)
 {
  *Run = *DstLn >= SrcLn;
  if (*Run)
  {
   memcpy_s(*Dst, *DstLn, Src, SrcLn);
   *(char **)Dst += SrcLn;
   *DstLn -= SrcLn;
  }
 }
}

static size_t
CreateResponse(char * Status, char *MimeType, uint32_t MimeTypeLn, char *Body, size_t BodyLn, char *Out, size_t OutLn)
{
 char *FirstLine = "HTTP/1.1 ";
 char *NewLine = "\r\n";
 char *ContentType = "Content-Type: ";
 char *ConnectionClose = "Connection: close";
 uint32_t Run = 1;
 void *OutView = Out;
 size_t OutViewLn = OutLn;
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, FirstLine, strlen(FirstLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, Status, 3);
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, ContentType, strlen(ContentType));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, MimeType, MimeTypeLn);
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, ConnectionClose, strlen(ConnectionClose));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, NewLine, strlen(NewLine));
 HttpMemcpyAdvance(&Run, &OutView, &OutViewLn, Body, BodyLn);
 if (Run)
 {
  return (char *)OutView - Out;
 }
 else
 {
  return 0;
 }
}

int
wmain(int32_t ArgC, void **ArgV)
{
 uint16_t Port = 3000;

 WORD VersionRequested = MAKEWORD(2, 2);
 WSADATA WsaData = {0};
 int32_t Err = 0;

 Err = WSAStartup(VersionRequested, &WsaData);
 if (Err != 0)
 {
  printf("WSAStartup failed with error: %d\n", Err);
  return 1;
 }

 if (LOBYTE(WsaData.wVersion) != 2 || HIBYTE(WsaData.wVersion) != 2)
 {
  printf("Could not find a usable version of Winsock.dll\n");
  WSACleanup();
  return 1;
 }

 SOCKET ServerSck = socket(AF_INET, SOCK_STREAM, 0);
 SOCKADDR_IN Server = {
  .sin_family = AF_INET,
  .sin_addr.s_addr = INADDR_ANY,
  .sin_port = htons(Port),
 };
 bind(ServerSck, (SOCKADDR *)&Server, sizeof(Server));
 listen(ServerSck, 3);

 for (;;)
 {
  char RecArr[8192] = {0};
  int32_t ServerLn = sizeof(Server);
  SOCKET ClientSck = accept(ServerSck, (SOCKADDR *)&Server, &ServerLn);
  int32_t WriteLn = recv(ClientSck, RecArr, sizeof(RecArr) - 1, 0);
  printf("%s", RecArr);

  char *MimeType = "text/html";
  char *Body = "<html><body><h1>Hello from C HTTP Server</h1></body></html>";
  char ResArr[8192] = {0};
  size_t ResWriteLn = CreateResponse(Http200, MimeType, (uint32_t)strlen(MimeType), Body, strlen(Body), ResArr, sizeof(ResArr));

  if (ResWriteLn)
  {
   send(ClientSck, ResArr, (int32_t)ResWriteLn, 0);
  }
  else
  {
   char *Http400Response =
    "HTTP/1.1 400\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html>"
    "<html>"
    "<head><title>400 Bad Request</title></head>"
    "<body><h1>Bad Request</h1></body>"
    "</html>";
    send(ClientSck, Http400Response, (int32_t)strlen(Http400Response), 0);
  }

  closesocket(ClientSck);
 }

 WSACleanup();
}