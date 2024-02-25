//�����зο� Ŭ���̾�Ʈ
#include <conio.h>
#include <string>
#include <vector>
#include <process.h>

#include "clientbase.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

ClientBase clnt(SERVER_IP,PORT_NUM); //�̰� �������� ����?

std::string nickName; //�г����� ������ ���ڿ�
unsigned int friendNum=0; //ģ�� �� ����
unsigned int posforF=0; //ģ��â Ŀ�� ��ġ

//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//���� ���� --������ ����
void ReceiveError(void) {std::cout<<"���� ���� \n"; exit(1);};
//�޽��� ���� ������
unsigned WINAPI ReceiveMSG(void* arg);
//���� ����(�α���) ���μ���
bool LoginProcess();
//ģ�� ���� ��û
void RequestFriendList();
//ģ�� ��� ���
void PrintFriendList(std::vector<std::string>);
//ģ�� �߰�
void AddorFindMyFriend(std::string);
//ģ�� ã��
void PrintFoundFriend(std::vector<std::string>);

int main()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //������ ����
    // ClientBase clnt(SERVER_IP,PORT_NUM);
    //�α��� --�г��� ���
    while(!LoginProcess()); 
    //���� ������ ����
    HANDLE hRecvThread;
    hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);

    //ģ�� ��� â �޴� ����
    while(true)
    {
        int input;
        bool quit=false;
        RequestFriendList();
        while(!quit)
        {
            input=getch();
            switch (input)
            {
            case 72:  
            case 119:
                if(posforF!=0) 
                {
                    posforF--;
                    RequestFriendList();
                }
                break;
            case 80: 
            case 115:
                if(posforF!=1+friendNum)
                {
                    posforF++;
                    RequestFriendList();
                }
                break;
            case 13:
                quit=true;
                break;
            default:
                continue;
            }
        }
        if(posforF<friendNum)
        {
            //ģ�� �����Ѱ�
            std::cout<<"�Ƹ� ģ���� �������� �ʾ�����? \n";
        }
        else if(posforF==friendNum)
        {
            //ģ�� ã�� --�г��� �Է�-���� �۽�-����-��� ���
            AddorFindMyFriend("Find");
        }
        else if(posforF==friendNum+1)
        {
            //ģ�� �߰� 
            AddorFindMyFriend("Add");
        }
        Sleep(2000); //�ӽ�
    }
    
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
    // SOCKET tcpSock=*((SOCKET*)arg); //�̰͸��ϸ� �������� --�������� ��;; ���ÿ� �ִ°Ŷ� ���дµ�
    // //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string bufString;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����

    while(recv(clnt.sock,buffer,BUF_SIZE,0)>0)
    {
        //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) ReceiveError(); //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[1];
        if(msg=="friend")
        {
            //ģ�� ��� ���
            PrintFriendList(split(bufString,':'));
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
        }
        else if(msg=="find")
        {
            //ģ�� ã�� ���
            PrintFoundFriend(split(bufString,':'));
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    //���� ���� ���� ������ ����ɰ� �̱⿡
    ReceiveError();
}
//���� ����(�α���) ���μ���
bool LoginProcess()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    std::cout<<"==================================================\n"
                "�г����� �Է����ּ��� (20������)>";
    
    std::getline(std::cin,nickName);
    while(nickName.size()>20)
    {
        std::cout<<"==================================================\n"
                "�ٽ� �Է����ּ��� (20������)>";
        std::getline(std::cin,nickName);
    }
    //������ �����ؼ� �ߺ����� �˻�(���� ����> ���ڿ� �ѱ���:Nick:nickName)
    nickName=":Nick:"+nickName;
    itoa(nickName.size(),buffer,10);
    msg=buffer+nickName;
    send(clnt.sock,msg.c_str(),msg.size(),0); //������ ����
    nickName.erase(nickName.find(':'),6); //�г��� ����
    //�����κ��� ��� ����
    if(recv(clnt.sock,buffer,BUF_SIZE,0)<0) ReceiveError();
    if(!strncmp(buffer,"Existed",8))
    {
        std::cout<<"==================================================\n"
                "�̹� ��ϵ� �����Դϴ�. ������ �½��ϱ�? (Y/N)";
        while(true)
        {
            std::string answer;
            std::getline(std::cin,answer);
            if(answer=="y"||answer=="Y")
            {
                //ģ�� ���� ������ ���� ������ ������ ��û
                //������ ������ �г���-���� ������ �ȵ� �켱 ������ ���� ��û�ؾ���
                // RequestFriendList(clnt);
                return true;
                break;
            }
            else if(answer=="n"||answer=="N")
            {
                //�г��� �ٽ� �Է�
                return false;
                break;
            }
            else
            {
                std::cout<<"�ٽ� �Է����ּ��� >";
            }   
        }
    }
    else if(!strncmp(buffer,"Success",8))
    {
        //�г��� ��� �� ���� �Ϸ�Ǹ� ģ�� ���� ��û
        // RequestFriendList(clnt);
        return true;
    }
    return false;
}
//ģ�� ���� ��û
void RequestFriendList()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    //ģ�� ���� ��û
    msg=":FriendList:"+nickName;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
}
//ģ�� ��� ���
void PrintFriendList(std::vector<std::string> flist)
{
    system("cls");
    std::cout<<"==================================================\n";
    if(flist[2]=="none")
    {
        std::cout<<"���� ģ���� �����ϴ�. \n";
    }
    else
    {
        //ģ�� �г��� ���ӻ��� ����������� ����� ��
        friendNum=atoi(flist[2].c_str()); //ģ�� �� ����
        for(int i=0;i<friendNum*2;i+=2)
        {
            if(posforF==i/2) std::cout<<"\x1b[33m"+flist[3+i]+"\x1b[m\t";
            else std::cout<<flist[3+i]+"\t";
            if(flist[4+i]=="F")
            {
                std::cout<<"\x1b[31m��\x1b[m\n";
            }
            else
            {
                std::cout<<"\x1b[32m��\x1b[m\n";
            }
        }
    }
    std::cout<<"==================================================\n";
    if(posforF==0+friendNum) std::cout<<"\x1b[33mģ�� ã�� > \x1b[m\n";
    else std::cout<<"ģ�� ã�� > \n";
    if(posforF==1+friendNum) std::cout<<"\x1b[33mģ�� �߰� > \x1b[m\n";
    else std::cout<<"ģ�� �߰� > \n";
}
//ģ�� �߰�
void AddorFindMyFriend(std::string AorF)
{ //--�г��� �Է�-���� �۽�-����-ģ�� ��� ������Ʈ
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    std::string friendNick;
    std::cout<<"ģ���� �г����� �Է��ϼ��� >";
    std::getline(std::cin,friendNick);
    while(friendNick==nickName) //���� �г��� �Է��������...�̷��ͱ��� �����ؾ���?
    {
        std::cout<<"���� �г����Դϴ�. �ٽ� �Է��ϼ��� >";
        std::getline(std::cin,friendNick);
    }
    msg=":"+AorF+":"+friendNick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
}
//ã�� ģ�� ��� ���
void PrintFoundFriend(std::vector<std::string> flist)
{
    std::cout<<"==================================================\n";
    if(flist[2]=="none")
    {
        std::cout<<"�ش��ϴ� ģ���� �����ϴ�. \n";
        return;
    }
    for(int i=0;i<atoi(flist[2].c_str())*2;i+=2)
    {
        std::cout<<flist[3+i]<<"\t";
        if(flist[4+i]=="F")
        {
            std::cout<<"\x1b[31m��\x1b[m\n";
        }
        else
        {
            std::cout<<"\x1b[32m��\x1b[m\n";
        }
    }
    
}