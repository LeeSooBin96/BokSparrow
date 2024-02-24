//�����зο� Ŭ���̾�Ʈ
#include <string>
#include <vector>

#include "clientbase.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

std::string nickName; //�г����� ������ ���ڿ�

//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//���� ����(�α���) ���μ���
void LoginProcess(ClientBase&);
//ģ�� ���� ��û �� ��� ���
void RequestFriendList(ClientBase&);

int main()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //������ ����
    ClientBase clnt(SERVER_IP,PORT_NUM);
    //�α��� --�г��� ���
    LoginProcess(clnt);
    


    Sleep(20000); //�ӽ�
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
//���� ����(�α���) ���μ���
void LoginProcess(ClientBase& clnt)
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
    //�����κ��� ��� ����
    recv(clnt.sock,buffer,BUF_SIZE,0);
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
                RequestFriendList(clnt);
                break;
            }
            else if(answer=="n"||answer=="N")
            {
                //�г��� �ٽ� �Է�
                LoginProcess(clnt);
                break;
            }
            else
            {
                std::cout<<"�ٽ� �Է����ּ��� >";
            }   
        }
    }
    else
    {
        //�г��� ��� �� ���� �Ϸ�Ǹ� ģ�� ���� ��û
        RequestFriendList(clnt);
    }
}
//ģ�� ���� ��û �� ��� ���
void RequestFriendList(ClientBase& clnt)
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    int recvLen, strLen;

    //ģ�� ���� ��û
    msg=":FriendList:"+nickName.erase(nickName.find(':'),6);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
    //ģ�� ���� �����ؾ���
    memset(buffer,0,BUF_SIZE);
    recv(clnt.sock,buffer,BUF_SIZE,0);
    //����� �־ ���� �Բ� �����ϱ�� ��
    msg=buffer;
    recvLen=msg.size(); //���ŵ� ���ڿ� ���� ����
    //���ŵ� ���ڿ� ���� ����
    strLen=atoi(buffer)+msg.substr(0,msg.find(':')).size();
    while(recvLen!=strLen)
    {
        memset(buffer,0,BUF_SIZE);
        if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) return; //���ſ���(���� ������� ����)
        msg.append(buffer); recvLen=msg.size();
    }
    std::cout<<split(msg,':')[1]<<std::endl; //���ŵ� �� �޽���
    if(split(msg,':')[1]=="none") //ģ�� ����� ������
    {
        std::cout<<"���� ģ���� �����ϴ�. \n";
    }
    else
    {
        //ģ�� �г��� ���ӻ��� ����������� ����� ��
    }
}