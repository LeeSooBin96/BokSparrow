//복스패로우 둥지(서버)
#include <process.h>

#include "serverbase.h"

const char* PORT_NUM="91016";
const unsigned int BUF_SIZE=1024;
//클라이언트 요청 처리
unsigned WINAPI HandlingClient(void* arg); 

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
            if(recv(clntSock,buffer,BUF_SIZE,0)<=0) return; //수신오류나면 스레드 종료
            bufString.append(buffer); recvLen=bufString.size();
        }
        

        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    std::cout<<"접속이 종료되었습니다. \n";
}