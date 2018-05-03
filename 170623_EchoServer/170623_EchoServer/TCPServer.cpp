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

	// 윈속 초기화
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
	InetPton(AF_INET, INADDR_ANY, &serveraddr.sin_addr); // INADDR_ANY = "0.0.0.0" 전체 Open
	// serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);


	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit(L"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit(L"listen()");

	// 데이터 통신에 사용할 변수
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
		
		// IP(숫자→문자열)
		if (WSAAddressToString((struct sockaddr*)&clientaddr, sizeof(SOCKADDR_IN), NULL, szAddrBuf, &dwAddrBufSize) == SOCKET_ERROR)
		{
			err_display(L"WSAAddressToString()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		///printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port)); 
		// IP(숫자→문자열) inet_ntoa(clientaddr.sin_addr) 함수는 더 이상 사용할 수 없다
		printf("\n[TCP 서버] 클라이언트 접속: %ls\n", szAddrBuf);
		// 클라이언트와 데이터 통신
		while (1)
		{
			// 데이터 받기
			retval = recv(client_sock, szDataBuf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(L"recv()");
				break;
			}
			else if (retval == 0)
				break;
			// 받은 데이터 출력
			szDataBuf[retval] = '\0';
			printf("[TCP/%ls]	%s\n", szAddrBuf, szDataBuf);
			//printf("[TCP/%ls]	%d\n", szAddrBuf, szDataBuf);
			printf("[TCP/%ls]	받은 크기 %d\n", szAddrBuf, retval);

			// 데이터 보내기
			retval = send(client_sock, szDataBuf, retval, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(L"send()");
				break;
			}
		}
		// closesocket()
		closesocket(client_sock);
		//printf("[TCP 서버] 클라이언트 종료: IP 주소 = %s, 포트 번호 = %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		printf("[TCP 서버] 클라이언트 종료: %ls\n", szAddrBuf); //ntohs(clientaddr.sin_port));
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(WCHAR *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%ls] %s\n", msg, lpMsgBuf);
	//printf("[%ls] %0xd\n", msg, (int)lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}