# 네트워크 프로그래밍 기초
## 📢 개요
  간단한 일대일 에코 서버/클라이언트

## 📌 주요 레퍼런스
### 1. 서버/클라이언트 공통
#### ☑ 윈도우 소켓 프로그램 준비

1. 윈속  : WSAStartup(wVersionRequested, lpWSAData)

        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // 매크로 함수 MAKEWORD(a, b) 인자의 두 BYTE를 하나의 WORD로 병합
          err_quit(L"WSAStartup()"); // 예외 처리

2. 소켓 생성 : socket(af, type, protocol)

        SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
        if (Socket == INVALID_SOCKET) // 예외 처리
          err_quit(L"socket()");

  * 주소 체계 (Af;Address family) 
  
   | Af | Meaning |
   |:--------|:--------|
   | AF_UNSPEC 0	| 미정의	| 
   | AF_INET 2	| IPv4 주소 체계 | 
   | AF_IPX 6	| IPX/SPX 주소 체계. 해당 프로토콜 설치 필요. Windows Vista 이상 미지원	| 
   | AF_APPLETALK 16 | AppleTalk 주소 체계. 해당 프로토콜 설치 필요. Windows Vista 이상 미지원	|
   | AF_NETBIOS 17	| NetBIOS 주소 체계	|
   | AF_INET6 23	| IPv6 주소 체계	|
   | AF_IRDA 26	| Infrared Data Association (IrDA) 주소 체계. 적외선 포트 및 드라이버 필요 |
   | AF_BTH 32	| 블루투스 주소 체계. 블루투스 어댑터 및 드라이버 설치 필요. Windows XP SP2 이상 지원	|
   
   **table 1. Address family*

  * 형식
  
   | Type | Meaning |
   |:--------|:--------|
   | SOCK_STREAM 1	| 인터넷 주소 체계(AF_INET/ AF_INET6)에 대해 **전송 제어 프로토콜(TCP)** 사용. 시퀀스 방식의 신뢰성있는 쌍방향 연결 기반 바이트 스트림 제공	| 
   | SOCK_DGRAM 2	| 인터넷 주소 체계(AF_INET/ AF_INET6)에 대해 **사용자 데이터그램 프로토콜(UDP)** 사용. 비연결성 및 불안정적 고정 크기 버퍼 데이터그램 제공 | 
   
   **table 2. Socket Type*
   
  * 프로토콜
  
  protocol 매개 변수에 대해 가능한 옵션은 지정된 주소 체계와 소켓 유형에 따라 다르다.
  
   | Protocol | Meaning |
   |:--------|:--------|
   | 0	|  서비스 공급자가 형식 인자 등을 참조하여 알맞은 프로토콜 자동 선택	| 
   | IPPROTO_ICMP 1	| 인터넷 제어 메시지 프로토콜 (ICMP)	| 
   | IPPROTO_IGMP 2	| 인터넷 그룹 관리 프로토콜 (IGMP).	| 
   | BTHPROTO_RFCOMM 3	| Bluetooth 무선 주파수 통신 (Bluetooth RFCOMM) 프로토콜	| 
   | **IPPROTO_TCP** 6	| 전송 제어 프로토콜 (TCP)	| 
   | **IPPROTO_UDP** 17	| 사용자 데이터 그램 프로토콜 (UDP)	| 
   | IPPROTO_ICMPV6 58	| 인터넷 제어 메시지 프로토콜 버전 6 (ICMPv6). Windows XP 이상 지원	| 
   | IPPROTO_RM 113	| 안정적인 멀티 캐스트를위한 PGM 프로토콜. 해당 프로토콜 설치 필요	| 
   
   **table 3. Protocol*
   
  * 리턴값
  
  소켓 반환. 실패 시 INVALID_SOCKET 반환. 실패 직후에 WSAGetLastError() 함수를 호출하여 윈속 오류 코드 검색 가능
  (오류 코드 목록 : https://docs.microsoft.com/ko-kr/windows/desktop/WinSock/windows-sockets-error-codes-2)
  
3. 소켓 주소 구조체 설정 : SOCKADDR_IN

        SOCKADDR_IN ServerAddr;
        ServerAddr.sin_family = AF_INET;      // 주소 체계 삽입 (table 1 참조)
        ServerAddr.sin_port = htons(df_SERVER_PORT);  // 접속할 서버의 포트, 혹은 서버측에서 열고자 할 포트 입력
        // u_short htons(u_short hostshort); : 호스트 바이트 정렬 방식의 2바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
        ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY = "0.0.0.0" 전체 Open
        // u_long htonl(u_long hostlong); : 호스트 바이트 정렬 방식의 4바이트 데이터를 네트워크 바이트 정렬 방식으로 변환
        // == InetPton(AF_INET, L"0.0.0.0", &ServerAddr.sin_addr); 

### 2-a. 서버
#### ☑ 통신 준비

1. Bind : 

        if (bind(ListenSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
          err_quit(L"bind()");  // 예외 처리

https://m.blog.naver.com/tnsehf12345/220426207208
/////////////////

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

        closesocket(Socket);
        
        WSACleanup();



#### ☑ 난수 생성

 1~45 사이의 무작위 숫자를 생성하기 위해 rand() 함수 사용

 - 함수 원형
 >int rand(void);

 rand() 함수는 `의사 난수(pseudo-random number)`를 생성한다. 이 값은 몇 번을 호출해도 처음 호출했을 때 값이 그대로 반환되는 문제점이 있다.

 >void srand(unsigned int seed);

 srand() 함수는 rand() 함수의 문제점을 해결하기 위해 난수 생성을 위한 씨드(seed) 값을 제공한다.


 - 소스 코드

         ...

         srand((int)time(NULL));

         ...
  
         iInNumber[x] = rand() % 44 + 1;

         ...


 #### ☑ 키보드 눌림 감지
 
  자동 모드에서 키를 입력받을때 이를 논-블록(non-block)으로 감지하기 위해 사용
 
  - 함수 원형
  >int _kbhit(void);

 키보드가 눌리면 0이 아닌 값을 반환한다.

  #### ☑ 눌린 키 값 반환
  
   - 함수 원형
   >int _getch(void);
 
  _kbhit()에서 감지한 키를 아스키코드 정수형으로 반환한다.
  

 
  - 소스 코드
 
          ...
 
          if (_kbhit())			// Checking if the keyboard is pressed
          {
            switch (_getch())	// Checking pressed key
            {
            case '0':
              g_iMode = 0;
              break;
            case '1':
              g_iMode = 1;
              break;
            default:
              g_iMode = 2;
              break;
            }
          }
 
          ...
