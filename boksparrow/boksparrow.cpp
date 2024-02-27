//복스패로우 클라이언트
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

//서버에 접속
ClientBase clnt(SERVER_IP,PORT_NUM); //이걸 전역으로 빼면?
//클라이언트 메인 진행 클래스
ClientHandle hClient;
//클라이언트 채팅 진행 클래스
ChatHandle hChat;

//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//메시지 수신 스레드
unsigned WINAPI ReceiveMSG(void* arg);
//채팅 송신 스레드
unsigned WINAPI InputChat(void* arg);
//채팅 수신 스레드
unsigned WINAPI OuputChat(void* arg);



int main()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //로그인 --닉네임 등록
    while(!hClient.LoginProcess(clnt.sock)); 
    while(true) //여기를 묶자
    {
        //수신 스레드 생성
        HANDLE hRecvThread; 
        hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);
        //친구 목록 창
        if(hClient.ProcessFriendScreen(clnt)) //이게 그냥 종료인지 채팅인지 구분해야함
        {
            //스레드 종료 대기
            WaitForSingleObject(hRecvThread,INFINITE);

            //채팅으로 넘기는걸 여기서 하자
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
        // std::cout<<"전달된 메시지: "<<bufString<<std::endl; //임시
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
        else if(msg=="clist")
        {
            //서버에서) 메시지총길이:clist:채팅방개수:채팅방이름:채팅방코드...
            std::cout<<bufString<<std::endl;
            //채팅방 목록 선택화면으로 가자~
        }
        else if(msg=="enter")
        {
            //채팅방 입장) 메시지 총길이:enter:채팅코드:상대닉네임(1:1)
            //채팅방 입장하면 이 스레드는 종료되어야함 --안해도됨!!!
            //채팅 초기 세팅 --1:N도 여기로 올것
            hChat.SettingChat(hClient.nickName,split(bufString,':'));

            break;
        }

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    //수신 오류
    if(recvLen<0) hClient.ReceiveError();
}
//채팅 송신 스레드 --메시지 입력 부분 (즉 채팅방 메인 진행부)
unsigned WINAPI InputChat(void* arg)
{ //서버에 메시지 요청시 규약 : 메시지 총길이:Chat:요청메시지:채팅방코드:요청자닉네임
    //사용할 버퍼
    char buffer[BUF_SIZE];
    std::string msg;
    //채팅 멤버 접속 상태 확인해야함
    msg=":Chat:Connect:"+hChat.code+":"+hChat.myNick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
    
    //채팅 입력 시작 부분
    std::string chat;
    while(true)
    {
        memset(buffer,0,BUF_SIZE);
        std::getline(std::cin,chat);
        //입력한 메시지 채팅 서버로
        //서버로 보낼 메시지)메시지총길이:Chat:Send:채팅코드:닉네임:메시지
        if(chat=="q"||chat=="Q") 
        {
            //서버한테 퇴장메시지 )메시지총길이:Chat:Quit:채팅코드:닉네임
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
//채팅 수신 스레드 --메시지 출력 부분
unsigned WINAPI OuputChat(void* arg)
{
    //메시지 수신시 사용하는 것들
    char buffer[BUF_SIZE]={0};
    std::string bufString, nickname;
    int recvLen, strLen; //수신메시지 길이, 문자열 길이
    while(recvLen=recv(clnt.sock,buffer,BUF_SIZE,0)>0) //수신상태 유지
    { //메시지길이+메시지내용 전달 받음
        bufString=buffer;
        recvLen=bufString.size(); //수신된 문자열 길이 저장
        //수신될 문자열 길이 저장
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //길이가 일치하지 않으면 더 읽기
        {
            memset(buffer,0,BUF_SIZE); //버퍼 초기화
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) return 0; //수신오류나면 스레드 종료
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"전달된 메시지: "<<bufString<<std::endl; //임시
        std::string msg=split(bufString,':')[1];
        if(msg=="connect")
        {
            //채팅방 멤버 접속상태 수신
            hChat.UpdateMemState(split(bufString,':'));
        }
        else if(msg=="enterM")
        {
            //들어온 멤버 닉네임 수신 -->상태 변경해야함
            hChat.UpdateEnterMem(split(bufString,':')[2]);
        }
        else if(msg=="notic")
        {
            //공지 메시지 수신 --여기서 퇴장일때 상태 변경 넣어야겠다~
            if(split(bufString,':')[3]=="Q") hChat.UpdateQuitMem(split(bufString,':')[4]);
            hChat.ReceiveNotic(split(bufString,':')[2]);
        }
        else if(msg=="chat")
        {
            //채팅 메시지 수신
            hChat.ReceiveChat(split(bufString,':')[2],split(bufString,':')[3]);
        }
        else if(msg=="quit") break; //수신 스레드 종료


        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    if(recvLen<0) hClient.ReceiveError();
    return 0;
}