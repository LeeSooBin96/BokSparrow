//�����зο� Ŭ���̾�Ʈ
#include <conio.h>
#include <process.h>
#include <vector>
#include <string>

#include "clientbase.h"
#include "clienthandle.h"
#include "chathandle.h" 

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

//������ ����
ClientBase clnt(SERVER_IP,PORT_NUM); //�̰� �������� ����?
//Ŭ���̾�Ʈ ���� ���� Ŭ����
ClientHandle hClient;
//Ŭ���̾�Ʈ ä�� ���� Ŭ����
ChatHandle hChat;

//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//�޽��� ���� ������
unsigned WINAPI ReceiveMSG(void* arg);
//ä�� �۽� ������
unsigned WINAPI InputChat(void* arg);
//ä�� ���� ������
unsigned WINAPI OuputChat(void* arg);



int main()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //�α��� --�г��� ���
    while(!hClient.LoginProcess(clnt.sock)); 
    while(true) //���⸦ ����
    {
        //���� ������ ����
        HANDLE hRecvThread; 
        hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);
        //ģ�� ��� â
        if(hClient.ProcessFriendScreen(clnt)) //�̰� �׳� �������� ä������ �����ؾ���
        {
            //������ ���� ���
            WaitForSingleObject(hRecvThread,INFINITE);

            //ä������ �ѱ�°� ���⼭ ����
            HANDLE hCthread[2];
            hCthread[1]=(HANDLE)_beginthreadex(NULL,0,OuputChat,nullptr,0,NULL);
            hCthread[0]=(HANDLE)_beginthreadex(NULL,0,InputChat,nullptr,0,NULL);
            WaitForMultipleObjects(2,hCthread,TRUE,INFINITE);
        }
        else break;
    }
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
        // std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
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
        else if(msg=="clist")
        {
            //��������) �޽����ѱ���:clist:ä�ù氳��:ä�ù��̸�:ä�ù��ڵ�...
            std::cout<<bufString<<std::endl;
            //ä�ù� ��� ����ȭ������ ����~
        }
        else if(msg=="enter")
        {
            //ä�ù� ����) �޽��� �ѱ���:enter:ä���ڵ�:���г���(1:1)
            //ä�ù� �����ϸ� �� ������� ����Ǿ���� --���ص���!!!
            //ä�� �ʱ� ���� --1:N�� ����� �ð�
            hChat.SettingChat(hClient.nickName,split(bufString,':'));

            break;
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    //���� ����
    if(recvLen<0) hClient.ReceiveError();
}
//ä�� �۽� ������ --�޽��� �Է� �κ� (�� ä�ù� ���� �����)
unsigned WINAPI InputChat(void* arg)
{ //������ �޽��� ��û�� �Ծ� : �޽��� �ѱ���:Chat:��û�޽���:ä�ù��ڵ�:��û�ڴг���
    //����� ����
    char buffer[BUF_SIZE];
    std::string msg;
    //ä�� ��� ���� ���� Ȯ���ؾ���
    msg=":Chat:Connect:"+hChat.code+":"+hChat.myNick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
    
    //ä�� �Է� ���� �κ�
    std::string chat;
    while(true)
    {
        memset(buffer,0,BUF_SIZE);
        std::getline(std::cin,chat);
        //�Է��� �޽��� ä�� ������
        //������ ���� �޽���)�޽����ѱ���:Chat:Send:ä���ڵ�:�г���:�޽���
        if(chat=="q"||chat=="Q") 
        {
            //�������� ����޽��� )�޽����ѱ���:Chat:Quit:ä���ڵ�:�г���
            msg=":Chat:Quit:"+hChat.code+":"+hClient.nickName;
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
            break;
        }
        else if(!chat.empty())
        {
            msg=":Chat:Send:"+hChat.code+":"+hClient.nickName+":"+chat;
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
        }
        else hChat.PrintChatScreen();
    }


    return 0;
}
//ä�� ���� ������ --�޽��� ��� �κ�
unsigned WINAPI OuputChat(void* arg)
{
    //�޽��� ���Ž� ����ϴ� �͵�
    char buffer[BUF_SIZE]={0};
    std::string bufString, nickname;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����
    while(recvLen=recv(clnt.sock,buffer,BUF_SIZE,0)>0) //���Ż��� ����
    { //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) return 0; //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[1];
        if(msg=="connect")
        {
            //ä�ù� ��� ���ӻ��� ����
            hChat.UpdateMemState(split(bufString,':'));
        }
        else if(msg=="enterM")
        {
            //���� ��� �г��� ���� -->���� �����ؾ���
            hChat.UpdateEnterMem(split(bufString,':')[2]);
        }
        else if(msg=="notic")
        {
            //���� �޽��� ���� --���⼭ �����϶� ���� ���� �־�߰ڴ�~
            if(split(bufString,':')[3]=="Q") hChat.UpdateQuitMem(split(bufString,':')[4]);
            hChat.ReceiveNotic(split(bufString,':')[2]);
        }
        else if(msg=="chat")
        {
            //ä�� �޽��� ����
            hChat.ReceiveChat(split(bufString,':')[2],split(bufString,':')[3]);
        }
        else if(msg=="quit") break; //���� ������ ����


        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    if(recvLen<0) hClient.ReceiveError();
    return 0;
}