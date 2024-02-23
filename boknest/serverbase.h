//서버 소켓 기본 세팅 클래스
#ifndef _SERVERBASE_H
#define _SERVERBASE_H

#include <iostream>
#include <Windows.h>

class ServerBase{
    //디폴트 private 멤버
    WSADATA wsaData; //윈속 라이브러리 구조체
    SOCKADDR_IN servAdr; //서버 주소 정보 구조체

public:
    SOCKET servSock; //서버 소켓 구조체
    
public:
    ServerBase(const char*); //윈속 라이브러리 초기화 및 소켓 생성
    ~ServerBase(); //서버 종료
    
    void ErrorHandling(std::string); //에러 메시지 출력 후 종료
    void openServer(int=SOMAXCONN); //서버 오픈(대기 큐 크기)
};
#endif