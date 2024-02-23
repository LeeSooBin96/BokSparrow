#include "clientbase.h"

//(생성자) 서버 아이피, 포트번호 전달 받음 --서버에 연결
ClientBase::ClientBase(const char* ip,const char* port)
{
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0) //윈속 라이브러리 구조체 초기화
    {
        ErrorHandling("WSAStartup() error!");
    }
    sock=socket(PF_INET,SOCK_STREAM,0); //TCP 소켓 생성
    //주소 정보 초기화
    memset(&servAdr,0,sizeof(servAdr));
    servAdr.sin_family=AF_INET;
    servAdr.sin_addr.s_addr=inet_addr(ip); //입력된 아이피로 설정
    servAdr.sin_port=htons(atoi(port)); //입력된 포트번호로 설정
    //서버에 연결
    if(connect(sock,(SOCKADDR*)&servAdr,sizeof(servAdr))==SOCKET_ERROR)
    {
        ErrorHandling("connect() error");
    }
    else
    {
        std::cout<<"서버에 연결되었습니다. \n";
    }
}
ClientBase::~ClientBase()
{
    closesocket(sock);
    WSACleanup();
}
//에러 메시지 출력 후 종료
void ClientBase::ErrorHandling(std::string msg)
{
    std::cerr<<msg<<std::endl;
    exit(1);
}