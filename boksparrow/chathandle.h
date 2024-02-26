#ifndef CHAT_HANDLE_H
#define CHAT_HANDLE_H

#include <iostream>
#include <vector>
#include <string>

#include "Windows.h"

struct MemInfo
{
    std::string nick; //닉네임
    bool connect; //접속상태
};
class ChatHandle
{
    //디폴트(private) 멤버
    //서버 통신 관련
    std::string myNick; //닉네임
    std::string code; //채팅 코드
    //채팅창 관련
    std::string title; //채팅방 이름
    std::vector<MemInfo> memList; //채팅방 유저 목록
    std::string chat[10]; //채팅 로그


public:
    //세팅
    void SettingChat(std::string,std::vector<std::string>);
    //채팅창 출력
    void PrintChatScreen(void);
    //채팅 송신 스레드
    static unsigned WINAPI InputChat(void* arg);
    //채팅 수신 스레드
    static unsigned WINAPI OuputChat(void* arg);
};
#endif