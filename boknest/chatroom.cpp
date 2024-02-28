#include "chatroom.h"

//ä�ù� ��� ��ȸ
void ChatRoom::ShowChatRoomList(SOCKET& sock,std::string nick)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":clist:";
    int count=0;
    std::vector<std::string> codeList,titleList;
    for(auto room: roomList)
    {
        for(auto mem: room.list)
        {
            if(mem.nick==nick)
            {
                count++; 
                titleList.push_back(room.title);
                codeList.push_back(room.code);
                break;
            }
        }
    }
    if(count==0) //���ϳ��� ������
    { 
        send(sock,"8:clist:0",10,0);
        return;
    }
    itoa(count,buffer,10);
    msg.append(buffer);
    msg.append(":");
    // for(auto c:codeList) msg.append(c+":");
    for(int i=0;i<count;i++)
    {
        msg.append(titleList[i]+":"+codeList[i]+":");
    }
    msg.pop_back();
    //������� ���� �޽��� ���� �Ϸ�
    memset(buffer,0,BUF_SIZE);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//1:1ä�ù� ����(�ڵ������Ҽ���,�������Ʈ) --�ϴ� ������� ����
//1:N�̾ �۵��ϰԲ� ���� �����̶� ä�ù� ��� ����Ʈ ���޵ǰ� ����!
std::string ChatRoom::EnterChatRoom(SOCKET& sock,std::vector<std::string> mlist)
{ //�׷� �� �������Ʈ ��ȯ�ϰ� �ϸ� ä�ù� ��� ��ο��� ���� �޽����� ���� �� �ְڴ�!
    srand(time(NULL));
    //����� ����
    char buffer[BUF_SIZE];
    std::string code;
    //�ڵ� ���� ������
    if(mlist.size()==2) code="P";
    else code="M";
    //�������ִ� ���
    int count=0; 
    for(auto room: roomList)
    {
        if(room.list.size()==mlist.size()) //����� ���� �游 �˻�
        {
            for(auto mem:room.list)
            {
                // if(mem.nick==mlist[0]) count++;
                // if(mem.nick==mlist[1]) count++;
                for(auto m: mlist)
                {
                    if(m==mem.nick)
                    { 
                        count++;
                        break;
                    }
                }
            }
        }
        if(count==mlist.size())
        {
            code=room.code; //���� ���ڵ� ��������
            break;
        }
        else count=0; //ī��Ʈ �ʱ�ȭ
    }
    if(count!=mlist.size()) //���� ����ߴٸ�
    {
        //ä�ù� ����
        ChatRoomInfo room;
        for(auto m: mlist)
        {
            MemInfo mem;
            mem.nick=m;
            room.list.push_back(mem);
        }
        //ä�� �ڵ� ����
        while(true)
        {
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
    }
    //Ŭ���̾�Ʈ���� ���� �޽���) �޽����ѱ���:enter:ä���ڵ�:���г���
    std::string msg=":enter:"+code;
    for(int i=0;i<mlist.size()-1;i++)
    {
        msg.append(":"+mlist[1+i]);
    }
    memset(buffer,0,BUF_SIZE);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
    return code;
}
//ä�ù� ��� �������� ����
void ChatRoom::SendMemConnect(SOCKET& sock,std::string code,std::string nick)
{
    //���⼭ �� �������� ���� -->ä�ù� ����鿡�� ���������� �˷�����
    std::string msg=":"+code+":connect:";
    for(auto& room: roomList)
    {
        if(room.code==code)
        {
            for(auto& mem:room.list)
            {
                if(mem.nick!=nick)
                {
                    if(mem.connect) msg.append("T:");
                    else msg.append("F:");
                }
                else
                {
                    mem.connect=true;
                }
            }
            break;
        }
    }msg.pop_back();
    char buffer[BUF_SIZE]={0};
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//ä�ù� ��� ����Ʈ ��ȯ(ä�� �ڵ� ���޹���)
std::vector<std::string> ChatRoom::BringMemList(std::string Ccode)
{
    std::vector<std::string> memList;
    for(auto room: roomList)
    {
        if(room.code==Ccode)
        {
            for(auto m: room.list)
            {
                memList.push_back(m.nick);
            }
        }
    }
    return memList;
}
//ä�ù� ����
void ChatRoom::QuitChatRoom(std::string code,std::string nick)
{
    //���⼭�� ���� ���游
    for(auto room: roomList)
    {
        if(room.code==code)
        {
            for(auto mem: room.list)
            {
                if(mem.nick==nick) mem.connect=false;
            }
        }
    }
}