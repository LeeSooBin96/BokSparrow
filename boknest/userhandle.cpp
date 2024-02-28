#include "userhandle.h"

//���� ���� ����
void UserHandle::UserLogout(std::string nick)
{
    for(auto& mem: userList) //����� ���� �÷��� ����
    {
        if(mem.nick==nick) //������ �ƴ� �г������� �����ϴ°� ��Ȯ
        {
            mem.conflag=false;
        }
    }
}
//���� ����(����� ����,�г���)
void UserHandle::UserLogin(SOCKET& sock,std::string nick)
{
    //�г��� �ߺ� ���� �˻�
    for(auto mem: userList)
    {
        if(mem.nick==nick) //�ߺ��� �г����� �ִٸ�
        {
            send(sock,"Existed",8,0);
            return;
        }
    }
    //�ߺ� ���� �� ȸ���̸�
    UserInfo user;
    user.sock=sock;
    user.nick=nick;
    userList.push_back(user); //���� ����Ʈ�� �߰�
    //���� �������� ����
    send(sock,"Success",8,0);
}
//ģ�� ���� ����(������ ����)
void UserHandle::SendFriendList(SOCKET& sock,std::string nick)
{
    char tmp[BUF_SIZE]={0};
    std::string msg;
    //�켱 �ش��ϴ� ���� ���� ����ü ã�ƾ���
    for(auto& mem: userList) //�� �����Ҷ��� ������!!! --���� ���� �Ф�
    {
        if(mem.nick==nick)
        {
            if(mem.sock!=sock)
            {
                mem.sock=sock; //�������� ��� �������� ������Ʈ
                mem.conflag=true; //���ӻ��� ������Ʈ
            }
            //���� ģ�� ���� ���������
            /*����:���ڿ�����:ģ�� ��:ģ���г���:���ӻ���:...*/
            if(mem.friendList.size()>0)
            {
                itoa(mem.friendList.size(),tmp,10); //ģ����
                msg.append(tmp);
                msg=":friend:"+msg+":";
                for(int i=0;i<mem.friendList.size();i++)
                {
                    msg.append(mem.friendList[i]+":"); //�г��� �߰�
                    for(auto m:userList)
                    {
                        if(mem.friendList[i]==m.nick)
                        { //���ӻ��� �߰�
                            if(m.conflag)
                            {
                                msg.append("T:");
                            }
                            else
                            {
                                msg.append("F:");
                            }
                            break;
                        }
                    }
                }
                msg.pop_back();
                //:ģ����:ģ���̸�:����... ���� ���� �߰�����
                memset(tmp,0,BUF_SIZE);
                itoa(msg.size(),tmp,10);
                msg=tmp+msg;
                send(sock,msg.c_str(),msg.size(),0);
            }
            else
            {
                send(sock,"12:friend:none",15,0);
            }
            break;
        }
    }
}
//ģ�� �߰�
void UserHandle::AddFriend(SOCKET& sock,std::string nick)
{
    for(auto& mem: userList)
    {
        if(mem.sock==sock)
        {
            bool ck=false;
            for(auto m: mem.friendList)
            {
                if(m==nick) //���� ģ�� ����Ʈ�� ������
                {
                    ck=true; break;
                }
            }
            if(!ck) //ģ������Ʈ�� ������
            { //��� �ٷ� �߰��� �ƴ϶� ���� ��Ͽ� �ִ��� Ȯ���ϰ� ����Ϳ��� ���� �߰��ؾ���
                //�۾��� �����ϱ�
                std::string msg;
                for(auto& m: userList)
                {
                    if(m.nick==nick)//���� ��Ͽ� �ִٸ�
                    {
                        m.friendList.push_back(mem.nick);//ģ���Ϳ��� �߰��ؾ���
                        mem.friendList.push_back(nick); //�̰� �� �ȸ�����--�ذ� ��...
                        send(sock,"12:add:Success",15,0);
                        return;
                    }
                }
            }
            break;
        }
    }
    send(sock,"11:add:Failed",14,0);
}
//ģ�� ã��
void UserHandle::FindFriend(SOCKET& sock,std::string keyword)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":";
    int count=0;
    for(auto mem: userList)
    {   //�ڱ��ڽ� �����ϰ�
        if(sock!=mem.sock&&mem.nick.find(keyword)<mem.nick.size()) //Ű���� �����ϴ� �г��� ������
        {
            count++;
            msg.append(mem.nick+":");
            if(mem.conflag)
            {
                msg.append("T:");
            }
            else
            {
                msg.append("F:");
            }
        }
    }
    msg.pop_back();
    if(count==0) send(sock,"10:find:none",13,0);
    itoa(count,buffer,10);
    msg=buffer+msg; msg=":find:"+msg;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
    std::cout<<msg<<std::endl;
}
//�ӼӸ� ������
void UserHandle::sendWhisper(SOCKET& sock,std::vector<std::string> mlist)
{
    //���޵� �޽���) �޽��� ��ü ����:Whisper:�� �г���:ģ�� �ε���:�޽���
    for(auto mem: userList)
    {
        if(mem.nick==mlist[2])
        {
            for(auto m: userList)
            {
                if(m.nick==mem.friendList[atoi(mlist[3].c_str())])
                {
                    //���� �޽���) �޽��� ��ü ����:whisper:��������г���:�޽���
                    std::string msg=":whisper:"+mem.nick+":"+mlist[4];
                    char buf[BUF_SIZE];
                    itoa(msg.size(),buf,10);
                    msg=buf+msg;
                    send(m.sock,msg.c_str(),msg.size(),0);
                    break;
                }
            }
            break;
        }
    }
}
//ģ�� �г��� ��������
std::string UserHandle::BringMyFriend(std::string myNick,int index)
{
    std::string friendNick;
    for(auto mem:userList)
    {
        if(mem.nick==myNick)
        {
            friendNick=mem.friendList[index];
            break;
        }
    }
    return friendNick;
}
//ä�� ��� ���� ���� ����(�������Ʈ,���º����Ѵг���)
void UserHandle::SendEnterMem(std::string code,std::vector<std::string> mlist,std::string nick)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":"+code+":enterM:"+nick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;

    for(auto mem: mlist)
    {
        for(auto user: userList)
        {
            if(user.nick!=nick&&user.nick==mem) //���� ������ ���� ������ ����鿡�� ����
            {
                send(user.sock,msg.c_str(),msg.size(),0);
            }
        }
    }
}
//ä�� ������� ���� �޽��� ������
void UserHandle::SendNoticMSG(std::string code,std::vector<std::string> mlist,std::string msg)
{
    char buffer[BUF_SIZE]={0};
    std::string notic=":"+code+":notic:"+msg+":";
    itoa(notic.size(),buffer,10);
    notic=buffer+notic;

    for(auto mem: mlist)
    {
        for(auto user: userList)
        {
            if(user.nick==mem)
            {
                send(user.sock,notic.c_str(),notic.size(),0);
            }
        }
    }
}
//ä�� �޽��� ������
void UserHandle::SendChatMSG(std::string code,std::vector<std::string> mlist,std::string nick,std::string msg)
{ 
    std::string chat;
    char buffer[BUF_SIZE]={0};
    chat=":"+code+":chat:"+nick+":"+msg;
    itoa(chat.size(),buffer,10);
    chat=buffer+chat;
    for(auto mem: mlist)
    {
        for(auto user: userList)
        {
            if(user.nick==mem)
            {
                send(user.sock,chat.c_str(),chat.size(),0);
            }
        }
    }
}