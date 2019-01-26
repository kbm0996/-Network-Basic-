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

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int iRecvLen, int flags)
{
	int iLeftLen = iRecvLen;
	while (iLeftLen > 0)	// 메시지를 모두 받을 때까지
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
	//문자열을 프로토콜(IPV4, IPV6 등)에 해당하는 네트워크데이터(빅엔디언 방식의 이진데이터)로 변경 (inet_ntoa는 IPV4만 지원)
	//ServerAddr.sin_addr.s_addr = inet_addr(df_SERVER_IP); // TODO: inet_addr()지원 중단
	InetPton(AF_INET, df_SERVER_IP, &ServerAddr.sin_addr);

	if (connect(Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		err_quit(L"connect()");

	// 데이터 통신에 사용할 변수
	char szDataBuf[df_BUF_SIZE + 1];
	int len;

	//////////////////////////////////////////////////////////////
	// 서버와 데이터 통신
	//
	//////////////////////////////////////////////////////////////
	while (1)
	{
		// 데이터 입력
		printf("[보낼 데이터] ");
		if (fgets(szDataBuf, df_BUF_SIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(szDataBuf);
		if (szDataBuf[len - 1] == '\n')
			szDataBuf[len - 1] = '\0';
		if (strlen(szDataBuf) == 0)
			break;

		// 데이터 보내기
		size_t iSendLen = send(Socket, szDataBuf, strlen(szDataBuf), 0);
		if (iSendLen == SOCKET_ERROR)
		{
			err_display(L"send()");
			break;
		}

		// 데이터 받기
		int iRecvLen = recvn(Socket, szDataBuf, iSendLen, 0);
		if (iRecvLen == SOCKET_ERROR)
		{
			err_display(L"recv()");
			break;
		}
		else if (iRecvLen == 0)
			break;

		// 받은 데이터 출력
		szDataBuf[iRecvLen] = '\0';
		printf("[받은 데이터] %s (%d bytes)\n", szDataBuf, iRecvLen);
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