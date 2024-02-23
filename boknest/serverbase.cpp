#include "serverbase.h"

//(생성자)윈속 라이브러리 초기화 및 소켓 생성 -- 서버 소켓 초기화
ServerBase::ServerBase(const char* port)
{
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
    {
        ErrorHandling("WSAStartup() error!"); //윈속 라이브러리 구조체 초기화 오류
        return;
    }
    servSock=socket(PF_INET,SOCK_STREAM,0); //TCP 소켓 생성
    //주소 정보 초기화
    memset(&servAdr,0,sizeof(servAdr));
    servAdr.sin_family=AF_INET; //IPv4
    servAdr.sin_addr.s_addr=htonl(INADDR_ANY); //호스트 주소 설정 
    servAdr.sin_port=htons(atoi(port));
    //주소 할당
    if(bind(servSock,(SOCKADDR*)&servAdr,sizeof(servAdr))==SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }
}
//(소멸자) 서버 종료
ServerBase::~ServerBase()
{
    closesocket(servSock); //서버 소켓 종료
    WSACleanup(); //윈속 라이브러리 해제
}

//서버 오픈(대기 큐 크기)
void ServerBase::openServer(int waitnum)
{
    if(listen(servSock,waitnum)==SOCKET_ERROR) //클라이언트 연결 대기
    {
        ErrorHandling("listen() error");
    }
    else
    {
        std::cout<<"서버 오픈 \n";
    }
}
//에러 메시지 출력 후 종료
void ServerBase::ErrorHandling(std::string msg)
{
    std::cerr<<msg<<std::endl;
    exit(1);
}