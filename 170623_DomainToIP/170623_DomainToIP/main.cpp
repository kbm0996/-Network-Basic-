#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERDOMAIN "procademyserver.iptime.org"
#define SERVERPORT 10099

// ���� �Լ� ���� ��� �� ����
void err_quit(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%ls] %s\n", msg, lpMsgBuf);
	//printf("[%ls] %0xd\n", msg, (int)lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}

BOOL DomainToIP(WCHAR *szDomain, IN_ADDR *pAddr)
{
	ADDRINFOW	*pAddrInfo;
	SOCKADDR_IN	*pSockAddr;
	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo) != 0)
	{
		return FALSE;
	}
	pSockAddr = (SOCKADDR_IN *)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);
	return TRUE;
}


int main()
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit(L"socket()");

	// bind()
	SOCKADDR_IN 	SockAddr;
	IN_ADDR 	Addr;
	memset(&SockAddr, 0, sizeof(SockAddr));

	DomainToIP(TEXT(SERVERDOMAIN), &Addr);

	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr = Addr;					// ��Ʈ��ũ ����Ʈ ����
	SockAddr.sin_port = htons(SERVERPORT);		// ��Ʈ��ũ ����Ʈ ����

	// ������ ��ſ� ����� ����
	TCHAR szAddrBuf[INET_ADDRSTRLEN];
	DWORD dwAddrBufSize = sizeof(szAddrBuf);
	
	// IP(���ڡ湮�ڿ�)
	if (WSAAddressToString((struct sockaddr*)&SockAddr, sizeof(SOCKADDR_IN), NULL, szAddrBuf, &dwAddrBufSize) == SOCKET_ERROR)
	{
		err_display(L"WSAAddressToString()");
		return 1;
	}

	// ������ Ŭ���̾�Ʈ ���� ���
	printf(SERVERDOMAIN);
	printf(" : %ls\n", szAddrBuf);

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}