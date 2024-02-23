#ifndef _CLIENTBASE_H_
#define _CLIENTBASE_H_

#include <iostream>
#include <Windows.h>

class ClientBase
{
    WSADATA wsaData; //윈속 라이브러리 구조체
    SOCKADDR_IN servAdr; //소켓에 세팅될 주소 정보 저장 구조체

public:
    SOCKET sock; //서버와 연결될 소켓

public:
    ClientBase(const char*, const char*); //(생성자) 서버 아이피, 포트번호 전달 받음
    ~ClientBase(); //(소멸자) 소켓 연결 종료

    void ErrorHandling(std::string); //에러 메시지 출력 후 종료
};
#endif