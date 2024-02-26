#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <ctime>

struct ChatRoomInfo
{
    std::string title;
    std::string code; //1:1 P, 1:N M
    std::vector<std::string> list;
};

class ChatRoom
{
    //디폴트(private) 멤버
    static const unsigned int BUF_SIZE=1024;
    std::vector<ChatRoomInfo> roomList;

public:
    //1:1채팅방 입장(코드전달할소켓,멤버리스트)
    void EnterOtOchatRoom(SOCKET&,std::vector<std::string>);
};
#endif