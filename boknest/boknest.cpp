//복스패로우 둥지(서버)
#include <process.h>
#include <vector>

#include "serverbase.h"

const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//클라이언트 요청 처리
unsigned WINAPI HandlingClient(void* arg); 
//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
/* 너무 정신없으면 나중에 다 분리 시키자.. */
/*유저 정보 관련 구조체, 벡터, 함수*/
struct UserInfo //유저 정보 저장 구조체
{
    std::string nick;
    SOCKET sock;
    bool conflag=true; //정보 추가시 자동 접속 상태로 설정됨.
    std::vector<std::string> friendList; //친구 목록(닉네임으로 저장)
};
std::vector<UserInfo> userList; //접속한 유저 정보 저장
//유저 접속(등록할 소켓,닉네임)
void UserLogin(SOCKET&,std::string);
//친구 정보 전송(전송할 소켓)
void SendFriendList(SOCKET&,std::string);
/*유저 정보 관련 구조체, 벡터, 함수  ----end*/

int main()
{
    //우선은 서버 오픈
    ServerBase serv(PORT_NUM);
    serv.openServer();
    //클라이언트 요청 수락
    SOCKET clntSock;
    SOCKADDR_IN clntAdr;
    int clntAdrSz=sizeof(clntAdr);

    while(true)
    { //클라이언트 계속 받아야함
        clntSock=accept(serv.servSock,(SOCKADDR*)&clntAdr,&clntAdrSz);
        std::cout<<"접속한 아이피: "<<inet_ntoa(clntAdr.sin_addr)<<std::endl;
      //다음 클라이언트를 받기 위해 요청을 처리할 스레드로 소켓 전달
        HANDLE hThread; //스레드 핸들
        hThread=(HANDLE)_beginthreadex(NULL,0,HandlingClient,(void*)&clntSock,0,NULL);
    }
    return 0;
}
//클라이언트 요청 처리
unsigned WINAPI HandlingClient(void* arg)
{
    SOCKET clntSock=*((SOCKET*)arg);
    //메시지 수신시 사용하는 것들
    char buffer[BUF_SIZE]={0};
    std::string bufString;
    int recvLen, strLen; //수신메시지 길이, 문자열 길이

    while(recv(clntSock,buffer,BUF_SIZE,0)>0) //수신상태 유지
    { //메시지길이+메시지내용 전달 받음
        bufString=buffer;
        recvLen=bufString.size(); //수신된 문자열 길이 저장
        //수신될 문자열 길이 저장
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //길이가 일치하지 않으면 더 읽기
        {
            memset(buffer,0,BUF_SIZE); //버퍼 초기화
            if(recv(clntSock,buffer,BUF_SIZE,0)<=0) return 0; //수신오류나면 스레드 종료
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"전달된 메시지: "<<bufString<<std::endl;
        std::string msg=split(bufString,':')[1];
        if(msg=="Nick")
        {
            UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            std::cout<<"친구 정보 어떻게 줄까 \n";
            SendFriendList(clntSock,split(bufString,':')[2]);
        }

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    std::cout<<"접속이 종료되었습니다. \n";
    //접속 종료 시 유저 접속 상태 변화
    for(auto mem: userList)
    {
        if(mem.sock==clntSock)
        {
            mem.conflag=false;
        }
    }
    return 0;
}
std::vector<std::string> split(std::string string,const char D)
{ //이거 완성시키고 싶다 --완성!
    std::vector<std::string> result;
    while(true) //찾은 위치가 문자열 크기보다 작은 동안
    {
        result.push_back(string.substr(0,string.find(D)));
        if(string.find(D)>string.size()) break; //여기에 넣어야 마지막것도 들어가나
        string.erase(0,string.find(D)+1); //넣은 문자열 지우기
    }
    return result;
}
//유저 접속(등록할 소켓,닉네임)
void UserLogin(SOCKET& sock,std::string nick)
{
    //닉네임 중복 여부 검사
    for(auto mem: userList)
    {
        if(mem.nick==nick) //중복인 닉네임이 있다면
        {
            send(sock,"Existed",8,0);
            return;
        }
    }
    //중복 없는 새 회원이면
    UserInfo user;
    user.sock=sock;
    user.nick=nick;
    userList.push_back(user); //유저 리스트에 추가
    //접속 성공여부 전송
    send(sock,"Success",8,0);
}
//친구 정보 전송(전송할 소켓)
void SendFriendList(SOCKET& sock,std::string nick)
{
    char tmp[BUF_SIZE]={0};
    std::string msg;
    //우선 해당하는 유저 정보 구조체 찾아야함
    for(auto mem: userList)
    {
        if(mem.nick==nick)
        {
            if(mem.sock!=sock)
            {
                mem.sock=sock; //재접속인 경우 소켓정보 업데이트
                mem.conflag=true; //접속상태 업데이트
            }
            //이제 친구 정보 보내줘야함
            /*형태:문자열길이:친구 수:친구닉네임:접속상태:...*/
            if(mem.friendList.size()>0)
            {
                itoa(mem.friendList.size(),tmp,10); //친구수
                msg.append(tmp);
                msg=":"+msg+":";
                for(int i=0;i<mem.friendList.size();i++)
                {
                    msg.append(mem.friendList[i]+":"); //닉네임 추가
                    for(auto m:userList)
                    {
                        if(mem.friendList[i]==m.nick)
                        { //접속상태 추가
                            if(m.conflag)
                            {
                                msg.append("T:");
                            }
                            else
                            {
                                msg.append("F:");
                            }
                            break;
                        }
                    }
                }
                std::cout<<msg<<std::endl;
                //복잡해서 친구 기능 넣고 테스트하면서 돌리자
            }
            else
            {
                send(sock,"5:none",7,0);
            }
            break;
        }
    }
}