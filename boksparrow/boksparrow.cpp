//복스패로우 클라이언트
#include <conio.h>
#include <string>
#include <vector>
#include <process.h>

#include "clientbase.h"
#include "clienthandle.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//서버에 접속
ClientBase clnt(SERVER_IP,PORT_NUM); //이걸 전역으로 빼면?

//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//메시지 수신 스레드
unsigned WINAPI ReceiveMSG(void* arg);
//클라이언트 메인 진행 클래스
ClientHandle hClient;

int main()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //로그인 --닉네임 등록
    while(!hClient.LoginProcess(clnt.sock)); 
    //수신 스레드 생성
    HANDLE hRecvThread;
    hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);
    //친구 목록 창
    hClient.ProcessFriendScreen(clnt);
    
    
    //스레드 종료하면 메인 종료되어야함
    WaitForSingleObject(hRecvThread,INFINITE);
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
//메시지 수신 스레드 --왜 동작안하지?
unsigned WINAPI ReceiveMSG(void* arg)
{
    // //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string bufString;
    int recvLen, strLen; //수신메시지 길이, 문자열 길이

    while(recvLen=recv(clnt.sock,buffer,BUF_SIZE,0)>0)
    {
        //메시지길이+메시지내용 전달 받음
        bufString=buffer;
        recvLen=bufString.size(); //수신된 문자열 길이 저장
        //수신될 문자열 길이 저장
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //길이가 일치하지 않으면 더 읽기
        {
            memset(buffer,0,BUF_SIZE); //버퍼 초기화
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) hClient.ReceiveError(); //수신오류나면 스레드 종료
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"전달된 메시지: "<<bufString<<std::endl; //임시
        std::string msg=split(bufString,':')[1];
        if(msg=="friend")
        {
            //친구 목록 출력
            hClient.PrintFriendList(split(bufString,':'));
        }
        else if(msg=="add")
        {
            //친구 추가 결과
            if(!strncmp(split(bufString,':')[2].c_str(),"Failed",7))
            {
                std::cout<<"존재하지 않는 계정이거나 이미 추가된 계정입니다. \n";
            }
            else if(!strncmp(split(bufString,':')[2].c_str(),"Success",8))
            {
                std::cout<<"성공적으로 추가되었습니다. \n";
            }
            Sleep(1000);
        }
        else if(msg=="find")
        {
            //친구 찾기 결과
            hClient.PrintFoundFriend(split(bufString,':'));
        }
        else if(msg=="whisper")
        {
            //귓속말 수신
            std::cout<<"==================================================\n";
            std::cout<<split(bufString,':')[2]<<"님으로부터: "<<split(bufString,':')[3]<<std::endl;
            std::cout<<"==================================================\n";
            Sleep(1000);
        }
        else if(msg=="enter")
        {
            //채팅방 입장) 메시지 총길이:enter:채팅코드:상대닉네임(1:1)
            std::cout<<bufString<<std::endl;
            Sleep(2000);
            //채팅방 입장하면 이 스레드는 종료되어야함
        }

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    //수신 오류
    if(recvLen<0) hClient.ReceiveError();
}