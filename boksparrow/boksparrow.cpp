//�����зο� Ŭ���̾�Ʈ
#include <conio.h>
#include <string>
#include <vector>
#include <process.h>

#include "clientbase.h"
#include "clienthandle.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//������ ����
ClientBase clnt(SERVER_IP,PORT_NUM); //�̰� �������� ����?

//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//�޽��� ���� ������
unsigned WINAPI ReceiveMSG(void* arg);
//Ŭ���̾�Ʈ ���� ���� Ŭ����
ClientHandle hClient;

int main()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //�α��� --�г��� ���
    while(!hClient.LoginProcess(clnt.sock)); 
    //���� ������ ����
    HANDLE hRecvThread;
    hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);
    //ģ�� ��� â
    hClient.ProcessFriendScreen(clnt);
    
    
    //������ �����ϸ� ���� ����Ǿ����
    WaitForSingleObject(hRecvThread,INFINITE);
    return 0;
}
std::vector<std::string> split(std::string string,const char D)
{ 
    std::vector<std::string> result;
    while(true) //ã�� ��ġ�� ���ڿ� ũ�⺸�� ���� ����
    {
        result.push_back(string.substr(0,string.find(D)));
        if(string.find(D)>string.size()) break; //���⿡ �־�� �������͵� ����
        string.erase(0,string.find(D)+1); //���� ���ڿ� �����
    }
    return result;
}
//�޽��� ���� ������ --�� ���۾�����?
unsigned WINAPI ReceiveMSG(void* arg)
{
    // //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string bufString;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����

    while(recvLen=recv(clnt.sock,buffer,BUF_SIZE,0)>0)
    {
        //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) hClient.ReceiveError(); //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[1];
        if(msg=="friend")
        {
            //ģ�� ��� ���
            hClient.PrintFriendList(split(bufString,':'));
        }
        else if(msg=="add")
        {
            //ģ�� �߰� ���
            if(!strncmp(split(bufString,':')[2].c_str(),"Failed",7))
            {
                std::cout<<"�������� �ʴ� �����̰ų� �̹� �߰��� �����Դϴ�. \n";
            }
            else if(!strncmp(split(bufString,':')[2].c_str(),"Success",8))
            {
                std::cout<<"���������� �߰��Ǿ����ϴ�. \n";
            }
            Sleep(1000);
        }
        else if(msg=="find")
        {
            //ģ�� ã�� ���
            hClient.PrintFoundFriend(split(bufString,':'));
        }
        else if(msg=="whisper")
        {
            //�ӼӸ� ����
            std::cout<<"==================================================\n";
            std::cout<<split(bufString,':')[2]<<"�����κ���: "<<split(bufString,':')[3]<<std::endl;
            std::cout<<"==================================================\n";
            Sleep(1000);
        }
        else if(msg=="enter")
        {
            //ä�ù� ����) �޽��� �ѱ���:enter:ä���ڵ�:���г���(1:1)
            std::cout<<bufString<<std::endl;
            Sleep(2000);
            //ä�ù� �����ϸ� �� ������� ����Ǿ����
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    //���� ����
    if(recvLen<0) hClient.ReceiveError();
}