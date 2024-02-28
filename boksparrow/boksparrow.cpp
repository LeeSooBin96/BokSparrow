//복스패로우 클라이언트
#include <conio.h>
#include <fstream>
#include <process.h>
#include <vector>
#include <string>

#include "clientbase.h"
#include "clienthandle.h"
#include "chathandle.h" 

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const char* FTP_PORT_NUM="90320";
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
//파일 업로드 스레드
unsigned WINAPI UploadFile(void* arg);
//파일 다운로드 스레드
unsigned WINAPI DownloadFile(void* arg);



int main()
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //로그인 --닉네임 등록
    system("cls");
    std::ifstream rFile("./img/logo.txt");
    rFile.seekg(0,std::ios::end);
    int fSize=rFile.tellg();
    rFile.seekg(0,std::ios::beg);
    char* fptr=new char[fSize];
    rFile.read(fptr,fSize);
    std::cout<<fptr<<std::endl;
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
        // else if(msg=="clist")
        // {
        //     //서버에서) 메시지총길이:clist:채팅방개수:채팅방이름:채팅방코드...
        //     //채팅방 목록 선택화면으로 가자~
        //     //함수는 종료되었고 여기서 채팅방 입장 혹은 이전으로가기를 할텐데
        //     //이전으로 가기를 하면 함수 다시 호출
        //     //채팅방 입장 선택하면 이 스레드도 종료시키면 됨
        //     //해보자
        //     hChat.ChooseChatRoom(split(bufString,':'));
        // }
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
        if(chat=="/Q"||chat=="/q") 
        {
            //서버한테 퇴장메시지 )메시지총길이:Chat:Quit:채팅코드:닉네임
            msg=":Chat:Quit:"+hChat.code+":"+hClient.nickName;
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
            break;
        }
        else if(chat=="/P"||chat=="/p")
        {
            std::string fPath, fName; //파일 경로, 파일 이름 저장
            std::cout<<"==================================================\n";
            std::cout<<"파일 경로를 입력하세요 >";
            std::getline(std::cin,fPath);
            std::cout<<"파일명을 입력하세요 >";
            std::getline(std::cin,fName);
            //채팅 서버에 파일명 업로드 사실 알리기 --채팅서버는 메시지만 띄울거니까 채팅 메시지 보내듯 하면 된다.
            msg=":Chat:Send:"+hChat.code+":"+hClient.nickName+":"+fName+"를 업로드하였습니다.";
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
            //그리고 스레드로 가서 ftp서버에 파일 전송해야함
            std::string f[2]={fPath,fName};
            HANDLE hUthread;
            hUthread=(HANDLE)_beginthreadex(NULL,0,UploadFile,(void*)&f,0,NULL);
            // std::cout<<"대충 파일전송...\n";
            //서버에 파일 전송함을 알리고 응답 받으면 FTP클라이언트 켜기
            //서버 자식 프로세스 생성이 윈도우는 안된다고 하니까... 여기서 바로 서버 접속
            // ClientBase ftpClient(SERVER_IP,FTP_PORT_NUM); //ftp 서버 접속
            //여기서 작동할 함수 필요하다 --파일 업로드는 채팅이랑 동시 동작해야하니 
            //스레드 하나 연결해서 거기서 서버에 접속 파일 업로드 하게 하자
            //파일 선택하고 전송하는 부분만 스레드로
        }
        else if(chat.substr(0,2)=="/D"||chat.substr(0,2)=="/d")
        {
            //이름 받아오니까 그거 서버에 보내서 해당하는 파일 받도록
            std::string fName=chat.substr(3,chat.size()-3);
            //여기는 파일만 바로 받으면 되니까 바로 스레드로
            HANDLE hDthread;
            hDthread=(HANDLE)_beginthreadex(NULL,0,DownloadFile,(void*)&fName,0,NULL);
            //서버에 파일 받음을 알리고 응답 받으면 FTP클라이언트 켜기
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
    /*서버 측에서도 채팅 코드 함께 보내줘야 내가 들어간 채팅방의 메시지만 받을 수 있다.*/
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
        // std::cout<<"전달된 메시지: "<<bufString<<std::endl; //임시
        std::string msg=split(bufString,':')[2];
        if(split(bufString,':')[1]!=hChat.code)
        {
            //다른방 메시지 왔을때 화면 멈춰버림
            hChat.PrintChatScreen();
            // continue; //현재 있는 채팅방이 아니면
        }
        else if(msg=="connect")
        {
            //채팅방 멤버 접속상태 수신
            hChat.UpdateMemState(split(bufString,':'));
        }
        else if(msg=="enterM")
        {
            //들어온 멤버 닉네임 수신 -->상태 변경해야함
            hChat.UpdateEnterMem(split(bufString,':')[3]);
        }
        else if(msg=="notic")
        {
            //공지 메시지 수신 --여기서 퇴장일때 상태 변경 넣어야겠다~
            if(split(bufString,':')[4]=="Q") hChat.UpdateQuitMem(split(bufString,':')[5]);
            hChat.ReceiveNotic(split(bufString,':')[3]);
        }
        else if(msg=="chat")
        {
            //채팅 메시지 수신
            hChat.ReceiveChat(split(bufString,':')[3],split(bufString,':')[4]);
        }
        else if(msg=="quit") break; //수신 스레드 종료


        memset(buffer,0,BUF_SIZE); //버퍼 초기화
    }
    if(recvLen<0) hClient.ReceiveError();
    return 0;
}
//파일 업로드 스레드
unsigned WINAPI UploadFile(void* arg)
{
    //파일경로 오나 확인해보자 --잘 전달됨
    std::string fPath=*((std::string*)arg);
    std::string fName=*((std::string*)arg+1);
    // Sleep(1000); //스레드 작동 확인용 임시
    //순서 한번 적어보자
    //1. 파일 명, 파일 경로 입력 --여기 오기전 하자 --완료
    //2. chat서버에 파일 업로드 사실 전송(파일명) --이것도 여기 오기전 --완료
    //3. ftp서버에 파일 전송 --이것만 여기서
    //일단 파일 오픈
    std::ifstream rfile(fPath,std::ios::in|std::ios::binary); //읽기, 바이너리모드로
    if(!rfile.is_open()) //파일 오픈 에러
    {
        std::cout<<"파일 오픈 에러! \n";
        return 0;
    }
    //파일 전체 사이즈 알아야하고? --여기부터 차근차근
    // int fSize; //파일 크기
    rfile.seekg(0,std::ios::end);
    int fSize=rfile.tellg();
    rfile.seekg(0,std::ios::beg);
    //파일 읽기
    char* fptr=new char[fSize]; //배열 생성
    rfile.read(fptr,fSize);
    rfile.close();

    ClientBase ftpClient(SERVER_IP,FTP_PORT_NUM); //ftp 서버 접속
    //보낼 메시지) 메시지총길이:File:파일사이즈
    std::string msg=":File:";
    char buf[BUF_SIZE]={0};
    itoa(fSize,buf,10); msg.append(buf);
    memset(buf,0,BUF_SIZE);
    itoa(msg.size(),buf,10);
    msg=buf+msg;
    send(ftpClient.sock,msg.c_str(),msg.size(),0); //파일 사이즈 전송

    if(recv(ftpClient.sock,buf,BUF_SIZE,0)<0) return 0; //수신 오류나면 종료
    // for(int i=0;i<fSize;i++) std::cout<<*(fptr+i);
    send(ftpClient.sock,fptr,fSize,0); //파일 데이터 전송
    recv(ftpClient.sock,buf,BUF_SIZE,0); 

    msg=":"+fName+":"+split(fPath,'.')[split(fPath,'.').size()-1]; //파일 이름이랑 확장자 전송
    memset(buf,0,BUF_SIZE);
    itoa(msg.size(),buf,10);
    msg=buf+msg;
    send(ftpClient.sock,msg.c_str(),msg.size(),0);
    // std::cout<<std::endl;
    //전송 완료(완료메시지 수신)되면 스레드 종료
    recv(ftpClient.sock,buf,BUF_SIZE,0); 
    delete[] fptr;
    std::cout<<fName<<"의 업로드가 완료되었습니다. \n";
}
//파일 다운로드 스레드
unsigned WINAPI DownloadFile(void* arg)
{
    //순서
    //1. 다운할 파일명 입력
    std::string fName=*((std::string*)arg);
    Sleep(1000); //스레드 작동 확인용 임시
    std::cout<<fName<<std::endl;
    //2. 다운로드 폴더 생성 --없으면 자동으로 되려나--안되네 --아 몰라.. 라이브러리 지원 안되니까 힘들어..
    // const wchar_t* dir=new const wchar_t("./downloads");
    // if(_waccess(dir,0)<0)
    //     CreateDirectory(dir,NULL);
    //3. ftp서버 연결
    ClientBase ftpClient(SERVER_IP,FTP_PORT_NUM); //ftp 서버 접속
    //4. 파일명 송신
    char buffer[BUF_SIZE]={0};
    std::string msg=":Download:"+fName;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(ftpClient.sock,msg.c_str(),msg.size(),0); //파일 요청
    //5. 데이터 수신
    memset(buffer,0,BUF_SIZE);
    recv(ftpClient.sock,buffer,BUF_SIZE,0); //파일 크기 수신
    int size=atoi(buffer);
    send(ftpClient.sock,"y",2,0); //확인메시지

    //6. 파일 생성 및 데이터 담기
    char* fptr=new char[size];
    recv(ftpClient.sock,fptr,size,0); //파일 데이터 전송
    send(ftpClient.sock,"y",2,0);
    
    memset(buffer,0,BUF_SIZE);
    recv(ftpClient.sock,buffer,BUF_SIZE,0); //파일 확장자
    send(ftpClient.sock,"y",2,0);
    std::string ext=buffer;

    std::ofstream wfile("./downloads/"+fName+"."+ext,std::ios::out|std::ios::binary);
    wfile.write(fptr,size);
    wfile.close();
    std::cout<<fName<<"의 다운로드가 완료되었습니다. \n";
    delete[] fptr;
}