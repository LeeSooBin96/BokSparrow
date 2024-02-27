#ifndef CHAT_HANDLE_H
#define CHAT_HANDLE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>


struct MemInfo
{
    std::string nick; //닉네임
    bool connect; //접속상태
};
class ChatHandle
{
    //디폴트(private) 멤버
    //채팅창 관련
    std::string title; //채팅방 이름
    std::vector<MemInfo> memList; //채팅방 유저 목록
    std::string chat[10]; //채팅 로그


public:
    //서버 통신 관련
    std::string myNick; //닉네임
    std::string code; //채팅 코드

    //세팅
    void SettingChat(std::string,std::vector<std::string>);
    //친구 상태정보 업데이트
    void UpdateMemState(std::vector<std::string>);
    //입장 멤버 상태 정보 업데이트
    void UpdateEnterMem(std::string);
    //퇴장 멤버 상태 정보 업데이트
    void UpdateQuitMem(std::string);
    //채팅창 출력
    void PrintChatScreen(void);
    //공지 수신
    void ReceiveNotic(std::string);
    //채팅 수신
    void ReceiveChat(std::string,std::string);
};
#endif