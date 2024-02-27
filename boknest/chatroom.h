#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <ctime>

struct MemInfo
{
    std::string nick; //닉네임
    bool connect=false; //접속상태
};
struct ChatRoomInfo
{
    std::string title;
    std::string code; //1:1 P, 1:N M
    std::vector<MemInfo> list; //멤버리스트...접속상태도 확인되어야함
};

class ChatRoom
{
    //디폴트(private) 멤버
    static const unsigned int BUF_SIZE=1024;
    std::vector<ChatRoomInfo> roomList;

public:
    //1:1채팅방 입장(코드전달할소켓,멤버리스트)
    void EnterChatRoom(SOCKET&,std::vector<std::string>);
    //채팅방 멤버 상태정보 전송(소켓,채팅코드,내닉네임)
    void SendMemConnect(SOCKET&,std::string,std::string);
    //채팅방 멤버 리스트 반환(채팅 코드 전달받음)
    std::vector<std::string> BringMemList(std::string);
    //채팅방 퇴장(채팅방 코드, 닉네임)
    void QuitChatRoom(std::string,std::string);
};
#endif