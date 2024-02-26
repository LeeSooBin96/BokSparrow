//복스패로우 둥지(서버)
#include <process.h>
#include <vector>

#include "serverbase.h"
#include "userhandle.h"

const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//클라이언트 요청 처리
unsigned WINAPI HandlingClient(void* arg); 
//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//유저 관련 정보 처리 클래스
UserHandle hUser; 


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
            hUser.UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            nickname=split(bufString,':')[2];
            hUser.SendFriendList(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Add")
        {
            //친구 추가
            hUser.AddFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Find")
        {
            //친구 찾기
            hUser.FindFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Whisper")
        {
            //귓속말 보내기
            hUser.sendWhisper(clntSock,split(bufString,':'));
        }

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    std::cout<<"접속이 종료되었습니다 : "<<nickname<<std::endl;
    //접속 종료 시 유저 접속 상태 변화
    hUser.UserLogout(nickname);
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
