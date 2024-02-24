//복스패로우 클라이언트
#include <string>
#include <vector>

#include "clientbase.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

std::string nickName; //닉네임을 저장할 문자열

//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//서버 접속(로그인) 프로세스
void LoginProcess(ClientBase&);
//친구 정보 요청 및 목록 출력
void RequestFriendList(ClientBase&);

int main()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //서버에 접속
    ClientBase clnt(SERVER_IP,PORT_NUM);
    //로그인 --닉네임 등록
    LoginProcess(clnt);
    


    Sleep(20000); //임시
    return 0;
}
std::vector<std::string> split(std::string string,const char D)
{ 
    std::vector<std::string> result;
    while(true) //찾은 위치가 문자열 크기보다 작은 동안
    {
        result.push_back(string.substr(0,string.find(D)));
        if(string.find(D)>string.size()) break; //여기에 넣어야 마지막것도 들어가나
        string.erase(0,string.find(D)+1); //넣은 문자열 지우기
    }
    return result;
}
//서버 접속(로그인) 프로세스
void LoginProcess(ClientBase& clnt)
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    std::cout<<"==================================================\n"
                "닉네임을 입력해주세요 (20자이하)>";
    
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
    send(clnt.sock,msg.c_str(),msg.size(),0); //서버에 전송
    //서버로부터 결과 수신
    recv(clnt.sock,buffer,BUF_SIZE,0);
    if(!strncmp(buffer,"Existed",8))
    {
        std::cout<<"==================================================\n"
                "이미 등록된 계정입니다. 본인이 맞습니까? (Y/N)";
        while(true)
        {
            std::string answer;
            std::getline(std::cin,answer);
            if(answer=="y"||answer=="Y")
            {
                //친구 정보 수신을 위한 서버에 데이터 요청
                //문제점 서버에 닉네임-소켓 연결이 안됨 우선 서버에 연결 요청해야함
                RequestFriendList(clnt);
                break;
            }
            else if(answer=="n"||answer=="N")
            {
                //닉네임 다시 입력
                LoginProcess(clnt);
                break;
            }
            else
            {
                std::cout<<"다시 입력해주세요 >";
            }   
        }
    }
    else
    {
        //닉네임 등록 및 접속 완료되면 친구 정보 요청
        RequestFriendList(clnt);
    }
}
//친구 정보 요청 및 목록 출력
void RequestFriendList(ClientBase& clnt)
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    int recvLen, strLen;

    //친구 정보 요청
    msg=":FriendList:"+nickName.erase(nickName.find(':'),6);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
    //친구 정보 수신해야함
    memset(buffer,0,BUF_SIZE);
    recv(clnt.sock,buffer,BUF_SIZE,0);
    //길수도 있어서 길이 함께 전달하기로 함
    msg=buffer;
    recvLen=msg.size(); //수신된 문자열 길이 저장
    //수신될 문자열 길이 저장
    strLen=atoi(buffer)+msg.substr(0,msg.find(':')).size();
    while(recvLen!=strLen)
    {
        memset(buffer,0,BUF_SIZE);
        if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) return; //수신오류(서버 종료등의 이유)
        msg.append(buffer); recvLen=msg.size();
    }
    std::cout<<split(msg,':')[1]<<std::endl; //수신된 총 메시지
    if(split(msg,':')[1]=="none") //친구 목록이 없으면
    {
        std::cout<<"아직 친구가 없습니다. \n";
    }
    else
    {
        //친구 닉네임 접속상태 목록형식으로 출력할 것
    }
}