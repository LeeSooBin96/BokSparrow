//복스패로우 클라이언트
#include <conio.h>
#include <string>
#include <vector>
#include <process.h>

#include "clientbase.h"

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;

ClientBase clnt(SERVER_IP,PORT_NUM); //이걸 전역으로 빼면?

std::string nickName; //닉네임을 저장할 문자열
unsigned int friendNum=0; //친구 수 저장
unsigned int posforF=0; //친구창 커서 위치

//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//수신 오류 --서버의 종료
void ReceiveError(void) {std::cout<<"서버 종료 \n"; exit(1);};
//메시지 수신 스레드
unsigned WINAPI ReceiveMSG(void* arg);
//서버 접속(로그인) 프로세스
bool LoginProcess();
//친구 정보 요청
void RequestFriendList();
//친구 목록 출력
void PrintFriendList(std::vector<std::string>);
//친구 추가
void AddorFindMyFriend(std::string);
//친구 찾기
void PrintFoundFriend(std::vector<std::string>);

int main()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //서버에 접속
    // ClientBase clnt(SERVER_IP,PORT_NUM);
    //로그인 --닉네임 등록
    while(!LoginProcess()); 
    //수신 스레드 생성
    HANDLE hRecvThread;
    hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);

    //친구 목록 창 메뉴 선택
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
            //친구 선택한것
            std::cout<<"아마 친구를 선택하지 않았을까? \n";
        }
        else if(posforF==friendNum)
        {
            //친구 찾기 --닉네임 입력-서버 송신-수신-결과 출력
            AddorFindMyFriend("Find");
        }
        else if(posforF==friendNum+1)
        {
            //친구 추가 
            AddorFindMyFriend("Add");
        }
        Sleep(2000); //임시
    }
    
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
    // SOCKET tcpSock=*((SOCKET*)arg); //이것만하면 오류나네 --전역으로 뺌;; 스택에 있는거라 못읽는듯
    // //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string bufString;
    int recvLen, strLen; //수신메시지 길이, 문자열 길이

    while(recv(clnt.sock,buffer,BUF_SIZE,0)>0)
    {
        //메시지길이+메시지내용 전달 받음
        bufString=buffer;
        recvLen=bufString.size(); //수신된 문자열 길이 저장
        //수신될 문자열 길이 저장
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //길이가 일치하지 않으면 더 읽기
        {
            memset(buffer,0,BUF_SIZE); //버퍼 초기화
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) ReceiveError(); //수신오류나면 스레드 종료
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"전달된 메시지: "<<bufString<<std::endl; //임시
        std::string msg=split(bufString,':')[1];
        if(msg=="friend")
        {
            //친구 목록 출력
            PrintFriendList(split(bufString,':'));
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
        }
        else if(msg=="find")
        {
            //친구 찾기 결과
            PrintFoundFriend(split(bufString,':'));
        }

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    //수신 오류 나면 스레드 종료될것 이기에
    ReceiveError();
}
//서버 접속(로그인) 프로세스
bool LoginProcess()
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
    nickName.erase(nickName.find(':'),6); //닉네임 저장
    //서버로부터 결과 수신
    if(recv(clnt.sock,buffer,BUF_SIZE,0)<0) ReceiveError();
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
                // RequestFriendList(clnt);
                return true;
                break;
            }
            else if(answer=="n"||answer=="N")
            {
                //닉네임 다시 입력
                return false;
                break;
            }
            else
            {
                std::cout<<"다시 입력해주세요 >";
            }   
        }
    }
    else if(!strncmp(buffer,"Success",8))
    {
        //닉네임 등록 및 접속 완료되면 친구 정보 요청
        // RequestFriendList(clnt);
        return true;
    }
    return false;
}
//친구 정보 요청
void RequestFriendList()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    //친구 정보 요청
    msg=":FriendList:"+nickName;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
}
//친구 목록 출력
void PrintFriendList(std::vector<std::string> flist)
{
    system("cls");
    std::cout<<"==================================================\n";
    if(flist[2]=="none")
    {
        std::cout<<"아직 친구가 없습니다. \n";
    }
    else
    {
        //친구 닉네임 접속상태 목록형식으로 출력할 것
        friendNum=atoi(flist[2].c_str()); //친구 수 저장
        for(int i=0;i<friendNum*2;i+=2)
        {
            if(posforF==i/2) std::cout<<"\x1b[33m"+flist[3+i]+"\x1b[m\t";
            else std::cout<<flist[3+i]+"\t";
            if(flist[4+i]=="F")
            {
                std::cout<<"\x1b[31m●\x1b[m\n";
            }
            else
            {
                std::cout<<"\x1b[32m●\x1b[m\n";
            }
        }
    }
    std::cout<<"==================================================\n";
    if(posforF==0+friendNum) std::cout<<"\x1b[33m친구 찾기 > \x1b[m\n";
    else std::cout<<"친구 찾기 > \n";
    if(posforF==1+friendNum) std::cout<<"\x1b[33m친구 추가 > \x1b[m\n";
    else std::cout<<"친구 추가 > \n";
}
//친구 추가
void AddorFindMyFriend(std::string AorF)
{ //--닉네임 입력-서버 송신-수신-친구 목록 업데이트
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    std::string friendNick;
    std::cout<<"친구의 닉네임을 입력하세요 >";
    std::getline(std::cin,friendNick);
    while(friendNick==nickName) //본인 닉네임 입력했을경우...이런것까지 생각해야해?
    {
        std::cout<<"본인 닉네임입니다. 다시 입력하세요 >";
        std::getline(std::cin,friendNick);
    }
    msg=":"+AorF+":"+friendNick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
}
//찾은 친구 목록 출력
void PrintFoundFriend(std::vector<std::string> flist)
{
    std::cout<<"==================================================\n";
    if(flist[2]=="none")
    {
        std::cout<<"해당하는 친구가 없습니다. \n";
        return;
    }
    for(int i=0;i<atoi(flist[2].c_str())*2;i+=2)
    {
        std::cout<<flist[3+i]<<"\t";
        if(flist[4+i]=="F")
        {
            std::cout<<"\x1b[31m●\x1b[m\n";
        }
        else
        {
            std::cout<<"\x1b[32m●\x1b[m\n";
        }
    }
    
}