//�����зο� ����(����)
#include <process.h>
#include <vector>

#include "serverbase.h"

const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//Ŭ���̾�Ʈ ��û ó��
unsigned WINAPI HandlingClient(void* arg); 
//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
/* �ʹ� ���ž����� ���߿� �� �и� ��Ű��.. */
/*���� ���� ���� ����ü, ����, �Լ�*/
struct UserInfo //���� ���� ���� ����ü
{
    std::string nick;
    SOCKET sock;
    bool conflag=true; //���� �߰��� �ڵ� ���� ���·� ������.
    std::vector<std::string> friendList; //ģ�� ���(�г������� ����)
};
std::vector<UserInfo> userList; //������ ���� ���� ����
//���� ����(����� ����,�г���)
void UserLogin(SOCKET&,std::string);
//ģ�� ���� ����(������ ����)
void SendFriendList(SOCKET&,std::string);
//ģ�� �߰�
void AddFriend(SOCKET&,std::string);
//ģ�� ã��
void FindFriend(SOCKET&,std::string);
/*���� ���� ���� ����ü, ����, �Լ�  ----end*/

int main()
{
    //�켱�� ���� ����
    ServerBase serv(PORT_NUM);
    serv.openServer();
    //Ŭ���̾�Ʈ ��û ����
    SOCKET clntSock;
    SOCKADDR_IN clntAdr;
    int clntAdrSz=sizeof(clntAdr);

    while(true)
    { //Ŭ���̾�Ʈ ��� �޾ƾ���
        clntSock=accept(serv.servSock,(SOCKADDR*)&clntAdr,&clntAdrSz);
        std::cout<<"������ ������: "<<inet_ntoa(clntAdr.sin_addr)<<std::endl;
      //���� Ŭ���̾�Ʈ�� �ޱ� ���� ��û�� ó���� ������� ���� ����
        HANDLE hThread; //������ �ڵ�
        hThread=(HANDLE)_beginthreadex(NULL,0,HandlingClient,(void*)&clntSock,0,NULL);
    }
    return 0;
}
//Ŭ���̾�Ʈ ��û ó��
unsigned WINAPI HandlingClient(void* arg)
{
    SOCKET clntSock=*((SOCKET*)arg);
    //�޽��� ���Ž� ����ϴ� �͵�
    char buffer[BUF_SIZE]={0};
    std::string bufString, nickname;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����

    while(recv(clntSock,buffer,BUF_SIZE,0)>0) //���Ż��� ����
    { //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clntSock,buffer,BUF_SIZE,0)<=0) return 0; //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[1];
        if(msg=="Nick")
        {
            UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            nickname=split(bufString,':')[2];
            SendFriendList(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Add")
        {
            //ģ�� �߰�
            AddFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Find")
        {
            //ģ�� ã��
            FindFriend(clntSock,split(bufString,':')[2]);
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    std::cout<<"������ ����Ǿ����ϴ� : "<<nickname<<std::endl;
    //���� ���� �� ���� ���� ���� ��ȭ
    for(auto& mem: userList) //����� ���� �÷��� ����
    {
        if(mem.nick==nickname) //������ �ƴ� �г������� �����ϴ°� ��Ȯ
        {
            mem.conflag=false;
        }
    }
    return 0;
}
std::vector<std::string> split(std::string string,const char D)
{ //�̰� �ϼ���Ű�� �ʹ� --�ϼ�!
    std::vector<std::string> result;
    while(true) //ã�� ��ġ�� ���ڿ� ũ�⺸�� ���� ����
    {
        result.push_back(string.substr(0,string.find(D)));
        if(string.find(D)>string.size()) break; //���⿡ �־�� �������͵� ����
        string.erase(0,string.find(D)+1); //���� ���ڿ� �����
    }
    return result;
}
//���� ����(����� ����,�г���)
void UserLogin(SOCKET& sock,std::string nick)
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
void SendFriendList(SOCKET& sock,std::string nick)
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
void AddFriend(SOCKET& sock,std::string nick)
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
void FindFriend(SOCKET& sock,std::string keyword)
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