#include "chatroom.h"

//1:1채팅방 입장(코드전달할소켓,멤버리스트)
void ChatRoom::EnterOtOchatRoom(SOCKET& sock,std::vector<std::string> mlist)
{
    srand(time(NULL));
    //사용할 버퍼
    char buffer[BUF_SIZE];
    std::string code;
    //채팅방 생성
    ChatRoomInfo room;
    room.list=mlist;
    //채팅 코드 생성
    while(true)
    {
        code="P";
        itoa(rand()%900+100,buffer,10);
        code.append(buffer);
        if(roomList.size()==0) //방이 하나도 없다면
        {
            break;
        }
        else //방이 하나라도 있다면
        { //채팅방 코드 중복되는지 검사
            bool ck=true;
            for(auto r:roomList)
            {
                if(r.code==code)
                {
                    ck=false;
                    break;
                }
            }
            if(ck) break;
        }
    }
    room.code=code;
    roomList.push_back(room);
    //클라이언트에게 보낼 메시지) 메시지총길이:enter:채팅코드:상대닉네임
    std::string msg=":enter:"+room.code+":"+mlist[1];
    memset(buffer,0,BUF_SIZE);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}