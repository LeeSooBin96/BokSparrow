//복스패로우 클라이언트
#include <string>

#include "clientbase.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

int main()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    //서버에 접속
    ClientBase clnt(SERVER_IP,PORT_NUM);
    std::cout<<"==================================================\n"
                "닉네임을 입력해주세요 (20자이하)>";
    std::string nickName; //닉네임을 저장할 문자열
    std::getline(std::cin,nickName);
    while(nickName.size()>20)
    {
        std::cout<<"==================================================\n"
                "다시 입력해주세요 (20자이하)>";
        std::getline(std::cin,nickName);
    }
    //서버에 전송해서 중복여부 검사(전달 형태> 문자열 총길이:Nick:nickName)
    nickName=":Nick:"+nickName;
    itoa(nickName.size(),buffer,10);
    msg=buffer+nickName;
    send(clnt.sock,msg.c_str(),msg.size(),0);

    recv(clnt.sock,buffer,BUF_SIZE,0);


    return 0;
}