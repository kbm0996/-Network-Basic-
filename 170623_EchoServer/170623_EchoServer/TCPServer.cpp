#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT	10099
#define BUFSIZE		1500

void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

int main(int argc, char *argv[])
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit(L"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));


	serveraddr.sin_family = AF_INET;
	InetPton(AF_INET, INADDR_ANY, &serveraddr.sin_addr); // INADDR_ANY = "0.0.0.0" ��ü Open
	// serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);


	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit(L"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit(L"listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	TCHAR szAddrBuf[INET_ADDRSTRLEN];
	DWORD dwAddrBufSize = sizeof(szAddrBuf);
	int addrlen;

	char szDataBuf[BUFSIZE + 1];

	while (1)
	{
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display(L"accpet()");
			break;
		}
		
		// IP(���ڡ湮�ڿ�)
		if (WSAAddressToString((struct sockaddr*)&clientaddr, sizeof(SOCKADDR_IN), NULL, szAddrBuf, &dwAddrBufSize) == SOCKET_ERROR)
		{
			err_display(L"WSAAddressToString()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		///printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port)); 
		// IP(���ڡ湮�ڿ�) inet_ntoa(clientaddr.sin_addr) �Լ��� �� �̻� ����� �� ����
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: %ls\n", szAddrBuf);
		// Ŭ���̾�Ʈ�� ������ ���
		while (1)
		{
			// ������ �ޱ�
			retval = recv(client_sock, szDataBuf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;
			// ���� ������ ���
			szDataBuf[retval] = '\0';
			printf("[TCP/%ls]	%s\n", szAddrBuf, szDataBuf);
			//printf("[TCP/%ls]	%d\n", szAddrBuf, szDataBuf);
			printf("[TCP/%ls]	���� ũ�� %d\n", szAddrBuf, retval);

			// ������ ������
			retval = send(client_sock, szDataBuf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(L"send()");
				break;
			}
		}
		// closesocket()
		closesocket(client_sock);
		//printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		printf("[TCP ����] Ŭ���̾�Ʈ ����: %ls\n", szAddrBuf); //ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

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