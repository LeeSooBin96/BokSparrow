//�����зο� Ŭ���̾�Ʈ
#include <string>

#include "clientbase.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

int main()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    //������ ����
    ClientBase clnt(SERVER_IP,PORT_NUM);
    std::cout<<"==================================================\n"
                "�г����� �Է����ּ��� (20������)>";
    std::string nickName; //�г����� ������ ���ڿ�
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
    send(clnt.sock,msg.c_str(),msg.size(),0);

    recv(clnt.sock,buffer,BUF_SIZE,0);


    return 0;
}