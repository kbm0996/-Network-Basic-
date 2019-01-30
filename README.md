# 네트워크 프로그래밍 기초
## 📢 개요
  간단한 일대일 에코 서버/클라이언트

## 📌 주요 레퍼런스
### 1. 서버/클라이언트 공통
#### ☑ 윈도우 소켓 프로그램 준비

1. 윈속  : WSAStartup(wVersionRequested, lpWSAData)

>        WSADATA wsa;
>        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
>        // 매크로 함수 MAKEWORD(a, b) 인자의 두 BYTE를 하나의 WORD로 병합
>          err_quit(L"WSAStartup()"); // 예외 처리

2. 소켓 생성 : socket(af, type, protocol)

>        SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
>        if (Socket == INVALID_SOCKET) // 예외 처리
>          err_quit(L"socket()");

  * 주소 체계 (Af;Address family) 
  
>   | Af | Meaning |
>   |:--------|:--------|
>   | AF_UNSPEC 0	| 미정의	| 
>   | AF_INET 2	| IPv4 주소 체계 | 
>   | AF_IPX 6	| IPX/SPX 주소 체계. 해당 프로토콜 설치 필요. Windows Vista 이상 미지원	| 
>   | AF_APPLETALK 16 | AppleTalk 주소 체계. 해당 프로토콜 설치 필요. Windows Vista 이상 미지원	|
>   | AF_NETBIOS 17	| NetBIOS 주소 체계	|
>   | AF_INET6 23	| IPv6 주소 체계	|
>   | AF_IRDA 26	| Infrared Data Association (IrDA) 주소 체계. 적외선 포트 및 드라이버 필요 |
>   | AF_BTH 32	| 블루투스 주소 체계. 블루투스 어댑터 및 드라이버 설치 필요. Windows XP SP2 이상 지원	|
   
   **table 1. Address family*

  * 형식
  
>   | Type | Meaning |
>   |:--------|:--------|
>   | SOCK_STREAM 1	| 인터넷 주소 체계(AF_INET/ AF_INET6)에 대해 **전송 제어 프로토콜(TCP)** 사용. 시퀀스 방식의 신뢰성있는 쌍방향 연결 기반 바이트 스트림 제공	| 
>   | SOCK_DGRAM 2	| 인터넷 주소 체계(AF_INET/ AF_INET6)에 대해 **사용자 데이터그램 프로토콜(UDP)** 사용. 비연결성 및 불안정적 고정 크기 버퍼 데이터그램 제공 | 
   
   **table 2. Socket Type*
   
  * 프로토콜
  
>  protocol 매개 변수에 대해 가능한 옵션은 지정된 주소 체계와 소켓 유형에 따라 다르다.
  
>   | Protocol | Meaning |
>   |:--------|:--------|
>   | 0	|  서비스 공급자가 형식 인자 등을 참조하여 알맞은 프로토콜 자동 선택	| 
>   | IPPROTO_ICMP 1	| 인터넷 제어 메시지 프로토콜 (ICMP)	| 
>   | IPPROTO_IGMP 2	| 인터넷 그룹 관리 프로토콜 (IGMP).	| 
>   | BTHPROTO_RFCOMM 3	| Bluetooth 무선 주파수 통신 (Bluetooth RFCOMM) 프로토콜	| 
>   | **IPPROTO_TCP** 6	| 전송 제어 프로토콜 (TCP)	| 
>   | **IPPROTO_UDP** 17	| 사용자 데이터 그램 프로토콜 (UDP)	| 
>   | IPPROTO_ICMPV6 58	| 인터넷 제어 메시지 프로토콜 버전 6 (ICMPv6). Windows XP 이상 지원	| 
>   | IPPROTO_RM 113	| 안정적인 멀티 캐스트를위한 PGM 프로토콜. 해당 프로토콜 설치 필요	| 
   
   **table 3. Protocol*
   
  * 리턴값
  
>  소켓 반환. 실패 시 INVALID_SOCKET 반환. 실패 직후에 WSAGetLastError() 함수를 호출하여 윈속 오류 코드 검색 가능
>  (오류 코드 목록 : https://docs.microsoft.com/ko-kr/windows/desktop/WinSock/windows-sockets-error-codes-2)
  
3. 소켓 주소 구조체 설정 : SOCKADDR_IN, SOCKADDR(AF_UNIX의 경우)

>        SOCKADDR_IN ServerAddr;
>        ServerAddr.sin_family = AF_INET; 
>        ServerAddr.sin_port = htons(df_SERVER_PORT); 
>        // htons() 호스트 바이트 정렬 방식의 2바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
>        ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY = "0.0.0.0" 전체 Open
>        // htonl() 호스트 바이트 정렬 방식의 4바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
>        // == InetPton(AF_INET, L"0.0.0.0", &ServerAddr.sin_addr); 
        
  * ServerAddr.sin_family : 주소 체계 (table 1 참조)
  * ServerAddr.sin_port   : 클라이언트의 경우 접속할 서버의 포트. 서버의 경우 열고자 할 포트
  * ServerAddr.sin_addr   : IP 주소
  * ServerADdr.sin_zero[8] : 구조체 정렬을 위해 구조체 크기를 맞추는 용도의 더미 

### 2-a. 서버

>  ![TCP/IP Server Network Function Order](https://github.com/kbm0996/Network-Programming-Basic/blob/master/Server%20Sequence.gif)
  
  **figure 1. TCP/IP Server Network Function Order*


1. 소켓 프로그램에 소켓 및 소켓 주소 전달 : bind(sock, *sockaddr, addrlen)

>        if (bind(ListenSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
>          err_quit(L"bind()");  // 예외 처리
          
  * sock : 소켓
  * sockaddr : 소켓 주소
  * addrlen : 소켓 주소 구조체 크기 
  

#### ☑ 클라이언트 접속 처리

2. 클라이언트 접속 요청을 받을 수 있는 상태로 전환: listen(sock, iBacklog)

>        if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) 
>          err_quit(L"listen()"); // 예외 처리

  * sock : 소켓
  * iBacklog : 최대 접속 대기 클라이언트 수
  
3. 클라이언트 접속 요청을 받을 수 있도록 설정 및 블록 상태로 대기 및 수락 : accept(sock, *addr, *addrlen)

>        while (1)
>        {
>          //////////////////////////////////////////////////////////////
>          // 클라이언트 접속 대기(Block)
>          //
>          //////////////////////////////////////////////////////////////
>          SOCKADDR_IN clientaddr;
>          int addrlen = sizeof(clientaddr);
>          SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR *)&clientaddr, &addrlen);
>          if (ClientSocket == INVALID_SOCKET) // 예외 처리
>          {
>            err_display(L"accpet()");
>            closesocket(ClientSocket);
>            continue;
>          }


  * sock : 소켓
  * addr : 클라이언트 주소가 저장될 소켓 구조체 포인터(out 변수)
  * addrlen : 클라이언트 주소의 크기
  
4. 서버 & 클라이언트 통신

>          //////////////////////////////////////////////////////////////
>          // 클라이언트와 데이터 통신
>          //
>          //////////////////////////////////////////////////////////////
>         
>          // 생략
>          
>          closesocket(ClientSocket); // 통신 종료 후에는 소켓 정리
>
>        } // end of while(1)
	
### 2-b. 클라이언트

>  ![TCP/IP Client Network Function Order](https://github.com/kbm0996/Network-Programming-Basic/blob/master/Client%20Sequence.gif)
  
  **figure 2. TCP/IP Client Network Function Order*


#### ☑ 서버 접속 요청

1. 서버 접속 요청 : connect(sock, *sockaddr, addrlen)

>        if (connect(Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
>		      err_quit(L"connect()");  // 예외 처리
          
  * sock : 소켓
  * sockaddr : 소켓 주소
  * addrlen : 소켓 주소 구조체 크기 
  
2. 서버 & 클라이언트 통신

>        while(1)
>        {
>          //////////////////////////////////////////////////////////////
>          // 서버와 데이터 통신
>          //
>          //////////////////////////////////////////////////////////////
>
>          // 생략
>
>        } // end of while(1)

### 3. 서버/클라이언트 공통
#### ☑ 통신 종료 후 소켓 정리

>        closesocket(Socket);       
>        WSACleanup();

