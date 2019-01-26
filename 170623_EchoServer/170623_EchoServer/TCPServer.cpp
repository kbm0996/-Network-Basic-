#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define df_SERVER_PORT	10099
#define df_BUF_SIZE		1500

void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

int main(int argc, char *argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit(L"WSAStartup()");

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET) 
		err_quit(L"socket()");

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(df_SERVER_PORT);
	// TODO: InetPton
	//���ڿ��� ��������(IPV4, IPV6 ��)�� �ش��ϴ� ��Ʈ��ũ������(�򿣵�� ����� ����������)�� ���� (inet_ntoa�� IPV4�� ����)
	///InetPton(AF_INET, L"0.0.0.0", &ServerAddr.sin_addr); //"127.0.0.1" = ������ �ּ�
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY = "0.0.0.0" ��ü Open
	// TODO: ȣ��Ʈ ����Ʈ ����(little endian), ��Ʈ��ũ ����Ʈ ����(big endian) ��ȣ ��ȯ
	// ȣ��Ʈ ����Ʈ ���� ����� 4����Ʈ �����͸� ��Ʈ��ũ ����Ʈ ���� ������� ��ȯ
	// u_long htonl(u_long hostlong); 
	// ȣ��Ʈ ����Ʈ ���� ����� 2����Ʈ �����͸� ��Ʈ��ũ ����Ʈ ���� ������� ��ȯ
	// u_short htons(u_short hostshort);
	// ��Ʈ��ũ ����Ʈ ���� ����� 4����Ʈ �����͸� ȣ��Ʈ ����Ʈ ���� ������� ��ȯ
	// u_long ntohl(u_long netlong);
	// ��Ʈ��ũ ����Ʈ ���� ����� 2����Ʈ �����͸� ȣ��Ʈ ����Ʈ ���� ������� ��ȯ
	// u_short ntohs(u_short netshort);
	// ȣ��Ʈ ����Ʈ ���� ����� 4����Ʈ �����͸� ��Ʈ��ũ ����Ʈ ���� ������� ��ȯ
	// u_long htonl(u_long hostlong); 
	// ȣ��Ʈ ����Ʈ ���� ����� 2����Ʈ �����͸� ��Ʈ��ũ ����Ʈ ���� ������� ��ȯ
	// u_short htons(u_short hostshort);
	// ��Ʈ��ũ ����Ʈ ���� ����� 4����Ʈ �����͸� ȣ��Ʈ ����Ʈ ���� ������� ��ȯ
	// u_long ntohl(u_long netlong);
	// ��Ʈ��ũ ����Ʈ ���� ����� 2����Ʈ �����͸� ȣ��Ʈ ����Ʈ ���� ������� ��ȯ
	// u_short ntohs(u_short netshort);
	
	if (bind(ListenSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		err_quit(L"bind()");

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) 
		err_quit(L"listen()");

	
	while (1)
	{
		//////////////////////////////////////////////////////////////
		// Ŭ���̾�Ʈ ���� ���(Block)
		//
		//////////////////////////////////////////////////////////////
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR *)&clientaddr, &addrlen);
		if (ClientSocket == INVALID_SOCKET)
		{
			err_display(L"accpet()");
			closesocket(ClientSocket);
			continue;
		}
		
		//////////////////////////////////////////////////////////////
		// ������ Ŭ���̾�Ʈ ���� ���
		//
		//////////////////////////////////////////////////////////////
		TCHAR szAddrBuf[INET_ADDRSTRLEN];
		DWORD dwAddrBuf_Size = sizeof(szAddrBuf);
		// IP(���ڡ湮�ڿ�)
		if (WSAAddressToString((struct sockaddr*)&clientaddr, sizeof(SOCKADDR_IN), NULL, szAddrBuf, &dwAddrBuf_Size) == SOCKET_ERROR)
		{
			err_display(L"WSAAddressToString()");
			closesocket(ClientSocket);
			continue;
		}
	
		///printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ� = %s, ��Ʈ ��ȣ = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port)); 
		// IP(���ڡ湮�ڿ�) inet_ntoa(clientaddr.sin_addr) �Լ��� �� �̻� ����� �� ����
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: %ls\n", szAddrBuf);

		//////////////////////////////////////////////////////////////
		// Ŭ���̾�Ʈ�� ������ ���
		//
		//////////////////////////////////////////////////////////////
		char szDataBuf[df_BUF_SIZE + 1];
		while (1)
		{
			// ������ �ޱ�
			int iRecvlen = recv(ClientSocket, szDataBuf, df_BUF_SIZE, 0);
			if (iRecvlen == SOCKET_ERROR)	// �������� ���� ����
			{
				err_display(L"recv()");
				break;
			}
			else if (iRecvlen == 0)	// ������ ���� ����
				break;

			// ���� ������ ���
			szDataBuf[iRecvlen] = '\0';
			printf("[TCP/%ls]	%s\n", szAddrBuf, szDataBuf);
			printf("[TCP/%ls]	���� ũ�� %d\n", szAddrBuf, iRecvlen);

			// ������ ������
			if (send(ClientSocket, szDataBuf, iRecvlen, 0) == SOCKET_ERROR)
			{
				err_display(L"send()");
				break;
			}
		}

		closesocket(ClientSocket);
		
		printf("[TCP ����] Ŭ���̾�Ʈ ����: %ls\n", szAddrBuf); //ntohs(clientaddr.sin_port));
	}

	closesocket(ListenSocket);

	WSACleanup();

	return 0;
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
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}