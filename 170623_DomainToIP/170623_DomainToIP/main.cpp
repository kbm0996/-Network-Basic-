#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define df_SERVER_DOMAIN L"www.dcinside.com"
#define df_SERVER_PORT 10099

BOOL DomainToIP(WCHAR *szDomain, IN_ADDR *pOutAddr);
void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit(L"WSAStartup()");	//printf("%d", WSAGetLastError());

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit(L"socket()");		//printf("%d", WSAGetLastError());

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(df_SERVER_PORT);
	if (!DomainToIP(df_SERVER_DOMAIN, &SockAddr.sin_addr))
		err_quit(L"DomainToIP()");	//printf("%d", WSAGetLastError());
	
	// TODO: IP(숫자→문자열)
	WCHAR szAddrBuf[INET_ADDRSTRLEN];
	DWORD dwAddrBufSize = sizeof(szAddrBuf);
	if (WSAAddressToString((struct sockaddr*)&SockAddr, sizeof(SOCKADDR_IN), NULL, szAddrBuf, &dwAddrBufSize) == SOCKET_ERROR)
		err_display(L"WSAAddressToString()");	//printf("%d", WSAGetLastError());

	wprintf(df_SERVER_DOMAIN);
	wprintf(L" : %ls\n", szAddrBuf);

	closesocket(sock);

	WSACleanup();

	return 0;
}

BOOL DomainToIP(WCHAR *szDomain, IN_ADDR *pOutAddr)
{
	ADDRINFOW	*pAddrInfo;
	SOCKADDR_IN	*pSockAddr;

	// TODO: 도메인을 IP로 반환
	// 성공시 0, 실패시 Windows Socket 오류 코드 반환
	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo) != 0)
		return FALSE;

	pSockAddr = (SOCKADDR_IN *)pAddrInfo->ai_addr;
	*pOutAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);
	return TRUE;
}

void err_quit(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	wprintf(L"[%ls] %ws\n", msg, (WCHAR*)lpMsgBuf);
	//printf("[%ls] %0xd\n", msg, (int)lpMsgBuf);
	MessageBox(NULL, (LPCWSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}
