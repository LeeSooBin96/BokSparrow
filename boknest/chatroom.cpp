#include "chatroom.h"

//1:1ä�ù� ����(�ڵ������Ҽ���,�������Ʈ)
void ChatRoom::EnterOtOchatRoom(SOCKET& sock,std::vector<std::string> mlist)
{
    srand(time(NULL));
    //����� ����
    char buffer[BUF_SIZE];
    std::string code;
    //ä�ù� ����
    ChatRoomInfo room;
    room.list=mlist;
    //ä�� �ڵ� ����
    while(true)
    {
        code="P";
        itoa(rand()%900+100,buffer,10);
        code.append(buffer);
        if(roomList.size()==0) //���� �ϳ��� ���ٸ�
        {
            break;
        }
        else //���� �ϳ��� �ִٸ�
        { //ä�ù� �ڵ� �ߺ��Ǵ��� �˻�
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
    //Ŭ���̾�Ʈ���� ���� �޽���) �޽����ѱ���:enter:ä���ڵ�:���г���
    std::string msg=":enter:"+room.code+":"+mlist[1];
    memset(buffer,0,BUF_SIZE);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}