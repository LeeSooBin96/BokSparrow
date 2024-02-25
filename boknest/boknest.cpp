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
//친구 추가
void AddFriend(SOCKET&,std::string);
//친구 찾기
void FindFriend(SOCKET&,std::string);
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
    std::string bufString, nickname;
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
        std::cout<<"전달된 메시지: "<<bufString<<std::endl; //임시
        std::string msg=split(bufString,':')[1];
        if(msg=="Nick")
        {
            UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            nickname=split(bufString,':')[2];
            SendFriendList(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Add")
        {
            //친구 추가
            AddFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Find")
        {
            //친구 찾기
            FindFriend(clntSock,split(bufString,':')[2]);
        }

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    std::cout<<"접속이 종료되었습니다 : "<<nickname<<std::endl;
    //접속 종료 시 유저 접속 상태 변화
    for(auto& mem: userList) //종료된 이후 플래그 조정
    {
        if(mem.nick==nickname) //소켓이 아닌 닉네임으로 구분하는게 정확
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
    for(auto& mem: userList) //값 변경할때는 참조로!!! --잊지 말자 ㅠㅠ
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
                msg=":friend:"+msg+":";
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
                msg.pop_back();
                //:친구수:친구이름:상태... 이제 길이 추가하자
                memset(tmp,0,BUF_SIZE);
                itoa(msg.size(),tmp,10);
                msg=tmp+msg;
                send(sock,msg.c_str(),msg.size(),0);
            }
            else
            {
                send(sock,"12:friend:none",15,0);
            }
            break;
        }
    }
}
//친구 추가
void AddFriend(SOCKET& sock,std::string nick)
{
    for(auto& mem: userList)
    {
        if(mem.sock==sock)
        {
            bool ck=false;
            for(auto m: mem.friendList)
            {
                if(m==nick) //기존 친구 리스트에 있으면
                {
                    ck=true; break;
                }
            }
            if(!ck) //친구리스트에 없으면
            { //사실 바로 추가가 아니라 유저 목록에 있는지 확인하고 상대방것에도 나를 추가해야함
                //작업이 복잡하군
                std::string msg;
                for(auto& m: userList)
                {
                    if(m.nick==nick)//유저 목록에 있다면
                    {
                        m.friendList.push_back(mem.nick);//친구것에도 추가해야함
                        mem.friendList.push_back(nick); //이게 왜 안먹히지--해결 하...
                        send(sock,"12:add:Success",15,0);
                        return;
                    }
                }
            }
            break;
        }
    }
    send(sock,"11:add:Failed",14,0);
}
//친구 찾기
void FindFriend(SOCKET& sock,std::string keyword)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":";
    int count=0;
    for(auto mem: userList)
    {   //자기자신 제외하고
        if(sock!=mem.sock&&mem.nick.find(keyword)<mem.nick.size()) //키워드 포함하는 닉네임 있으면
        {
            count++;
            msg.append(mem.nick+":");
            if(mem.conflag)
            {
                msg.append("T:");
            }
            else
            {
                msg.append("F:");
            }
        }
    }
    msg.pop_back();
    if(count==0) send(sock,"10:find:none",13,0);
    itoa(count,buffer,10);
    msg=buffer+msg; msg=":find:"+msg;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
    std::cout<<msg<<std::endl;
}