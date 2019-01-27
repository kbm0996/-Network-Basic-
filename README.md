# 네트워크 프로그래밍 기초
## 📢 개요
  간단한 일대일 에코 서버/클라이언트

## 📌 주요 레퍼런스
### 1. 서버/클라이언트 공통
#### ☑ 윈도우 소켓 준비

1. 윈속 DLL 실행 : WSAStartup(wVersionRequested, lpWSAData)

        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) // 매크로 함수 MAKEWORD(a, b) 인자의 두 BYTE를 하나의 WORD로 병합
          err_quit(L"WSAStartup()"); // 예외 처리

https://m.blog.naver.com/PostView.nhn?blogId=tnsehf12345&logNo=220421015688&proxyReferer=https%3A%2F%2Fwww.google.com%2F

        SOCKET Socket = socket(AF_INET, SOCK_STREAM, 0);
        if (Socket == INVALID_SOCKET) 
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
