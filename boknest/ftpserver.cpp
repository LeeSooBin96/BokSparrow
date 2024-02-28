//FTP 서버
#include <fstream>
#include <process.h>
#include <vector>

#include "serverbase.h"

const char* FTP_PORT_NUM="90320";
const unsigned int BUF_SIZE=10000;

//클라이언트 요청 처리
unsigned WINAPI HandlingClient(void* arg); 
//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//파일 저장
void SaveFile(SOCKET&,int);
//파일 전송
void SendFile(SOCKET&,std::string);

struct FileInfo //파일 정보 저장 구조체
{
    std::string name;
    std::string path;
};
std::vector<FileInfo> fileList;

int main() 
{
    ServerBase serv(FTP_PORT_NUM);
    serv.openServer();

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
    //파일 수신에 사용
    // int fSize;
    // std::cout<<"결국 켰는데 여기 접속 되니... \n"; //됨
    //그러면 이제 해야할것 클라이언트 요청받을 준비
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
        if(msg=="File")
        {
            //파일 업로드 요청 --파일 사이즈 받는다
            // fSize=atoi(split(bufString,':')[2].c_str());
            SaveFile(clntSock,atoi(split(bufString,':')[2].c_str()));
        }
        else if(msg=="Download")
        {
            SendFile(clntSock,split(bufString,':')[2]);
        }
    }
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
//파일 저장
void SaveFile(SOCKET& sock,int size)
{
    std::string data, name, ext;
    char buffer[BUF_SIZE]={0};  
    send(sock,"y",2,0); //확인 메시지 전송

    char* fptr=new char[size];
    // while(recv(sock,buffer,BUF_SIZE,0)) //데이터 모두 받을때까지
    // {
    //     data.append(buffer);
    //     if(data.size()>=size) break;
    //     memset(buffer,0,BUF_SIZE);
    // } //와 길이나 사이즈 알면 포인터로 한번에 받을 수 있다! 새롭게 하나를 앎
    recv(sock,fptr,size,0);
    send(sock,"f",2,0); //확인 메시지 전송
    memset(buffer,0,BUF_SIZE);

    std::string bufString;
    int recvLen, strLen;
    while(recv(sock,buffer,BUF_SIZE,0)) //파일이름 전송받기 --이거랑 경로랑 연결해야함
    {
        bufString=buffer;
        recvLen=bufString.size();
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //길이가 일치하지 않으면 더 읽기
        {
            memset(buffer,0,BUF_SIZE); //버퍼 초기화
            if(recv(sock,buffer,BUF_SIZE,0)<=0) return; //수신오류나면 스레드 종료
            bufString.append(buffer); recvLen=bufString.size();
        }
        if(recvLen==strLen) break;
        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    send(sock,"y",2,0);
    name=split(bufString,':')[1];
    ext=split(bufString,':')[2];
    //파일 이름 중복검사는 해야할듯


    //파일 저장해야함
    std::string path="./downloads/"+name+"."+ext;
    std::cout<<path<<std::endl;
    std::ofstream wfile(path,std::ios::out|std::ios::binary);
    // wfile.write(data.c_str(),size);
    wfile.write(fptr,size);
    wfile.close();

    //파일 구조체 배열에 등록
    FileInfo nfile;
    nfile.name=name;
    nfile.path=path;
    fileList.push_back(nfile);
    std::cout<<name<<"이 업로드 되었습니다. \n";
    delete[] fptr;
}
//파일 전송
void SendFile(SOCKET& sock,std::string fName)
{
    std::string path;
    for(auto f:fileList)
    {
        if(f.name==fName)
        {
            path=f.path;
        }
    }
    std::ifstream rfile(path,std::ios::in|std::ios::binary); //읽기, 바이너리모드로
    if(!rfile.is_open()) //파일 오픈 에러
    {
        std::cout<<"파일 오픈 에러! \n";
        return;
    }
    rfile.seekg(0,std::ios::end);
    int fSize=rfile.tellg();
    rfile.seekg(0,std::ios::beg);
    //파일 읽기
    char* fptr=new char[fSize]; //배열 생성
    rfile.read(fptr,fSize);
    rfile.close();

    char buffer[BUF_SIZE]={0};
    itoa(fSize,buffer,10);
    std::string bufString=buffer;
    send(sock,buffer,bufString.size(),0);
    recv(sock,buffer,BUF_SIZE,0);

    send(sock,fptr,fSize,0);
    recv(sock,buffer,BUF_SIZE,0);

    bufString=split(path,'.')[split(path,'.').size()-1];
    send(sock,bufString.c_str(),bufString.size(),0);
    recv(sock,buffer,BUF_SIZE,0); //확인 메시지 받고 스레드 종료

    std::cout<<fName<<" 전송 완료 \n";
    delete[] fptr;
}