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
    std::string bufString;
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
        std::cout<<"���޵� �޽���: "<<bufString<<std::endl;
        std::string msg=split(bufString,':')[1];
        if(msg=="Nick")
        {
            UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            std::cout<<"ģ�� ���� ��� �ٱ� \n";
            SendFriendList(clntSock,split(bufString,':')[2]);
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    std::cout<<"������ ����Ǿ����ϴ�. \n";
    //���� ���� �� ���� ���� ���� ��ȭ
    for(auto mem: userList)
    {
        if(mem.sock==clntSock)
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
    for(auto mem: userList)
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
                msg=":"+msg+":";
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
                std::cout<<msg<<std::endl;
                //�����ؼ� ģ�� ��� �ְ� �׽�Ʈ�ϸ鼭 ������
            }
            else
            {
                send(sock,"5:none",7,0);
            }
            break;
        }
    }
}