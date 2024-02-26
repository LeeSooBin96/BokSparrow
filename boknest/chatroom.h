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
    //����Ʈ(private) ���
    static const unsigned int BUF_SIZE=1024;
    std::vector<ChatRoomInfo> roomList;

public:
    //1:1ä�ù� ����(�ڵ������Ҽ���,�������Ʈ)
    void EnterOtOchatRoom(SOCKET&,std::vector<std::string>);
};
#endif