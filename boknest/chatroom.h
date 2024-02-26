#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#include <string>
#include <vector>

struct ChatRoomInfo
{
    std::string title;
    std::string code; //1:1 P, 1:N M
    std::vector<std::string> list;
};

class ChatRoom
{
    //디폴트(private) 멤버
    std::vector<ChatRoomInfo> roomList;
    
public:

};
#endif