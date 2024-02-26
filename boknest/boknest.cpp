//�����зο� ����(����)
#include <process.h>
#include <vector>

#include "serverbase.h"
#include "userhandle.h"

const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//Ŭ���̾�Ʈ ��û ó��
unsigned WINAPI HandlingClient(void* arg); 
//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//���� ���� ���� ó�� Ŭ����
UserHandle hUser; 


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
            hUser.UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            nickname=split(bufString,':')[2];
            hUser.SendFriendList(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Add")
        {
            //ģ�� �߰�
            hUser.AddFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Find")
        {
            //ģ�� ã��
            hUser.FindFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Whisper")
        {
            //�ӼӸ� ������
            hUser.sendWhisper(clntSock,split(bufString,':'));
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    std::cout<<"������ ����Ǿ����ϴ� : "<<nickname<<std::endl;
    //���� ���� �� ���� ���� ���� ��ȭ
    hUser.UserLogout(nickname);
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
