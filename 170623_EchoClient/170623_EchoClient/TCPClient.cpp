#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define df_SERVER_IP	L"127.0.0.1"
#define df_SERVER_PORT	10099
#define df_BUF_SIZE		1500

void err_quit(WCHAR *msg);
void err_display(WCHAR *msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int iRecvLen, int flags)
{
	int iLeftLen = iRecvLen;
	while (iLeftLen > 0)	// �޽����� ��� ���� ������
	{
		int iRecvLen_Temp = recv(s, buf, iLeftLen, flags);
		if (iRecvLen_Temp == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (iRecvLen_Temp == 0)
			break;
		iLeftLen -= iRecvLen_Temp;
		buf += iRecvLen_Temp;
	}

	return (iRecvLen - iLeftLen);
}

int main(int argc, char *argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit(L"WSAStartup()");

	SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET) 
		err_quit(L"socket()");

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(df_SERVER_PORT);

	// TODO: InetPton
	//���ڿ��� ��������(IPV4, IPV6 ��)�� �ش��ϴ� ��Ʈ��ũ������(�򿣵�� ����� ����������)�� ���� (inet_ntoa�� IPV4�� ����)
	//ServerAddr.sin_addr.s_addr = inet_addr(df_SERVER_IP); // TODO: inet_addr()���� �ߴ�
	InetPton(AF_INET, df_SERVER_IP, &ServerAddr.sin_addr);

	if (connect(Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		err_quit(L"connect()");

	// ������ ��ſ� ����� ����
	char szDataBuf[df_BUF_SIZE + 1];
	int len;

	//////////////////////////////////////////////////////////////
	// ������ ������ ���
	//
	//////////////////////////////////////////////////////////////
	while (1)
	{
		// ������ �Է�
		printf("[���� ������] ");
		if (fgets(szDataBuf, df_BUF_SIZE + 1, stdin) == NULL)
			break;

		// '\n' ���� ����
		len = strlen(szDataBuf);
		if (szDataBuf[len - 1] == '\n')
			szDataBuf[len - 1] = '\0';
		if (strlen(szDataBuf) == 0)
			break;

		// ������ ������
		size_t iSendLen = send(Socket, szDataBuf, strlen(szDataBuf), 0);
		if (iSendLen == SOCKET_ERROR)
		{
			err_display(L"send()");
			break;
		}

		// ������ �ޱ�
		int iRecvLen = recvn(Socket, szDataBuf, iSendLen, 0);
		if (iRecvLen == SOCKET_ERROR)
		{
			err_display(L"recv()");
			break;
		}
		else if (iRecvLen == 0)
			break;

		// ���� ������ ���
		szDataBuf[iRecvLen] = '\0';
		printf("[���� ������] %s (%d bytes)\n", szDataBuf, iRecvLen);
	}

	closesocket(Socket);

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