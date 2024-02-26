#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <vector>

#include "clientbase.h"

class ClientHandle
{
    //디폴트(private) 멤버
    static const unsigned int BUF_SIZE=1024;

    unsigned int friendNum=0; //친구 수 저장
    unsigned int posforF=0; //친구창 커서 위치

public:
    std::string nickName; //닉네임을 저장할 문자열
    
    //수신 오류 --서버의 종료
    void ReceiveError(void) {std::cout<<"서버 종료 \n"; exit(1);};
    //서버 접속(로그인) 프로세스
    bool LoginProcess(SOCKET&);
    //친구 정보 요청
    void RequestFriendList(SOCKET&);
    //친구 창 프로세스
    void ProcessFriendScreen(ClientBase&);
    //친구 목록 출력
    void PrintFriendList(std::vector<std::string>);
    //친구 추가
    void AddorFindMyFriend(SOCKET&,std::string);
    //친구 찾기
    void PrintFoundFriend(std::vector<std::string>);
    //친구 연결 --posforF로 친구 인덱스 읽을 수 있음
    void ConnectMyFriend(SOCKET&);
};
#endif