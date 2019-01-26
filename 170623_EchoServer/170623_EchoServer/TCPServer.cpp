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
	//문자열을 프로토콜(IPV4, IPV6 등)에 해당하는 네트워크데이터(빅엔디언 방식의 이진데이터)로 변경 (inet_ntoa는 IPV4만 지원)
	///InetPton(AF_INET, L"0.0.0.0", &ServerAddr.sin_addr); //"127.0.0.1" = 루프백 주소
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY = "0.0.0.0" 전체 Open
	// TODO: 호스트 바이트 정렬(little endian), 네트워크 바이트 정렬(big endian) 상호 변환
	// 호스트 바이트 정렬 방식의 4바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
	// u_long htonl(u_long hostlong); 
	// 호스트 바이트 정렬 방식의 2바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
	// u_short htons(u_short hostshort);
	// 네트워크 바이트 정렬 방식의 4바이트 데이터를 호스트 바이트 정렬 방식으로 변환
	// u_long ntohl(u_long netlong);
	// 네트워크 바이트 정렬 방식의 2바이트 데이터를 호스트 바이트 정렬 방식으로 변환
	// u_short ntohs(u_short netshort);
	// 호스트 바이트 정렬 방식의 4바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
	// u_long htonl(u_long hostlong); 
	// 호스트 바이트 정렬 방식의 2바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
	// u_short htons(u_short hostshort);
	// 네트워크 바이트 정렬 방식의 4바이트 데이터를 호스트 바이트 정렬 방식으로 변환
	// u_long ntohl(u_long netlong);
	// 네트워크 바이트 정렬 방식의 2바이트 데이터를 호스트 바이트 정렬 방식으로 변환
	// u_short ntohs(u_short netshort);
	
	if (bind(ListenSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
		err_quit(L"bind()");

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) 
		err_quit(L"listen()");

	
	while (1)
	{
		//////////////////////////////////////////////////////////////
		// 클라이언트 접속 대기(Block)
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
		// 접속한 클라이언트 정보 출력
		//
		//////////////////////////////////////////////////////////////
		TCHAR szAddrBuf[INET_ADDRSTRLEN];
		DWORD dwAddrBuf_Size = sizeof(szAddrBuf);
		// IP(숫자→문자열)
		if (WSAAddressToString((struct sockaddr*)&clientaddr, sizeof(SOCKADDR_IN), NULL, szAddrBuf, &dwAddrBuf_Size) == SOCKET_ERROR)
		{
			err_display(L"WSAAddressToString()");
			closesocket(ClientSocket);
			continue;
		}
	
		///printf("\n[TCP 서버] 클라이언트 접속: IP 주소 = %s, 포트 번호 = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port)); 
		// IP(숫자→문자열) inet_ntoa(clientaddr.sin_addr) 함수는 더 이상 사용할 수 없다
		printf("\n[TCP 서버] 클라이언트 접속: %ls\n", szAddrBuf);

		//////////////////////////////////////////////////////////////
		// 클라이언트와 데이터 통신
		//
		//////////////////////////////////////////////////////////////
		char szDataBuf[df_BUF_SIZE + 1];
		while (1)
		{
			// 데이터 받기
			int iRecvlen = recv(ClientSocket, szDataBuf, df_BUF_SIZE, 0);
			if (iRecvlen == SOCKET_ERROR)	// 비정상적 접속 종료
			{
				err_display(L"recv()");
				break;
			}
			else if (iRecvlen == 0)	// 정상적 접속 종료
				break;

			// 받은 데이터 출력
			szDataBuf[iRecvlen] = '\0';
			printf("[TCP/%ls]	%s\n", szAddrBuf, szDataBuf);
			printf("[TCP/%ls]	받은 크기 %d\n", szAddrBuf, iRecvlen);

			// 데이터 보내기
			if (send(ClientSocket, szDataBuf, iRecvlen, 0) == SOCKET_ERROR)
			{
				err_display(L"send()");
				break;
			}
		}

		closesocket(ClientSocket);
		
		printf("[TCP 서버] 클라이언트 종료: %ls\n", szAddrBuf); //ntohs(clientaddr.sin_port));
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