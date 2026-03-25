#include "http.c"
#include <stdio.h>
#include <assert.h>
#pragma comment(lib, "httpapi.lib")
#pragma comment(lib, "pathcch.lib")

#define AllocMem(cb) HeapAlloc(GetProcessHeap(), 0, (cb))
#define FreeMem(ptr) HeapFree(GetProcessHeap(), 0, (ptr))

static void *
DebugReadFile(LPCWSTR Name, uint32_t *BytesRead)
{
 void *Ret = 0;
 HANDLE FileHandle = CreateFileW(Name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
 if (FileHandle)
 {
  LARGE_INTEGER FileSize;
  if (GetFileSizeEx(FileHandle, &FileSize))
  {
   assert(FileSize.QuadPart < 0xffffffff);
   void *FileMemory = VirtualAlloc(0, FileSize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
   if (FileMemory)
   {
    if (ReadFile(FileHandle, FileMemory, (uint32_t)FileSize.QuadPart, BytesRead, 0) && *BytesRead == (uint32_t)FileSize.QuadPart)
    {
     Ret = FileMemory;
    }
    else
    {
     VirtualFree(FileMemory, 0, MEM_RELEASE);
     FileMemory = 0;
     *BytesRead = 0;
    }
   }
  }
  CloseHandle(FileHandle);
 }

 return Ret;
}

static wchar_t *
GetInputDir(int Argc, wchar_t *Argv[])
{
 wchar_t *InputDir;
 if (Argc == 2)
 {
  InputDir = Argv[1];
 }
 if (Argc < 2)
 {
  InputDir = L".";
 }
 else if (Argc > 2)
 {
  printf("Too many args.");
  InputDir = 0;
 }
 return InputDir;
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
  http_parse_ctx Ctx = {0};
  char ResArr[8192] = {0};
  size_t ResWriteLn = 0;
  if (HttpParseRequest(RecArr, WriteLn, &Ctx))
  {
   printf("Received request at: %.*s:%d%.*s\n", (int)Ctx.HostDomainNameLn, Ctx.HostDomainName, Ctx.HostPort, (int)Ctx.PathLn, Ctx.Path);
   char *MimeType = "text/html";
   char *Body = "<html><body><h1>Hello from C HTTP Server</h1></body></html>";
   ResWriteLn = HttpCreateResponse(Http200, MimeType, (uint32_t)strlen(MimeType), Body, strlen(Body), ResArr, sizeof(ResArr));
  }

  // todo use the path to create full file path
  // todo read the file
  // todo extract mime

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
