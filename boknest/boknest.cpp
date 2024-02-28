//복스패로우 둥지(서버)
#include <process.h>
#include <vector>

#include "serverbase.h"
#include "userhandle.h"
#include "chatroom.h"

const char* PORT_NUM="91016";
// const char* FTP_PORT_NUM="90320";
const unsigned int BUF_SIZE=1024;
//클라이언트 요청 처리
unsigned WINAPI HandlingClient(void* arg); 
//문자열 스플릿(split)
std::vector<std::string> split(std::string,const char);
//유저 관련 정보 처리 클래스
UserHandle hUser; 
//채팅 관련 처리 클래스
ChatRoom hChat;


int main()
{
    //파일 송수신 서버 오픈하기 위한 자식 프로세스 생성
    // fork(); //어디서 자식 프로세스를 생성해야할까~ 아 이거 리눅스에서만...아??
    // CreateProcess( )//오 이건 사용해본적이 없는데;; 별도의 실행파일이 필요..와..
    //그럼 그냥 소스파일 하나더 실행시키는거랑 뭐가 다르지.......................
    //그럼 아예 애초에 방법 2가 안되겠네 
    /*방법 1. 처음부터 자식프로세스 생성해서 FTP 서버 켜두기
      방법 2. 파일 송수신 관련 요청이 들어오면 서버 켜서 처리하고 프로세스 소멸
      방법 1은 자식 프로세스 하나면 되고 그 자식프로세스는 용량도 적겠지?
      방법 2는 자식 프로세스가 요청 들어올때마다 켜질꺼고? 기존 소켓들 닫아줘야하고?
               생성된 자식 프로세스 잘 소멸되고 있는지 좀비 프로세스 없는지 확인해야함
      아무래도 1이 낫겠지...해보자*/
    //우선은 서버 오픈
    ServerBase serv(PORT_NUM);
    // ServerBase FTPserv(FTP_PORT_NUM);
    serv.openServer();
    // FTPserv.openServer();
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
        // else if(msg=="Clist")
        // {
        //     //닉네임이 포함된 모든 채팅방코드 리스트로 보내주자
        //     hChat.ShowChatRoomList(clntSock,split(bufString,':')[2]);
        // }
        else if(msg=="Enter")
        {
            //채팅방 입장
            std::vector<std::string> mlist; //멤버 리스트 저장할 벡터
            mlist.push_back(split(bufString,':')[3]);
            if(split(bufString,':')[2]=="OtO") //1:1 채팅방
            {
            //수신 메시지)메시지총길이:Enter:OtO:내 닉네임:친구 인덱스
                //채팅방 입장 --채팅방 코드 송신해야함
                mlist.push_back(hUser.BringMyFriend(split(bufString,':')[3],atoi(split(bufString,':')[4].c_str())));
                // hChat.EnterOtOchatRoom(clntSock,{split(bufString,':')[3],hUser.BringMyFriend(split(bufString,':')[3],atoi(split(bufString,':')[4].c_str()))});
            }
            else if(split(bufString,':')[2]=="OtM") //멀티 채팅방
            {
            //수신 메시지)메시지총길이:Enter:OtO:내 닉네임:친구닉네임1:친구닉네임2...
                //1:N 일때는 닉네임 전달받아서 방입장하게하자 --인덱스로 변경
                for(int i=4;i<split(bufString,':').size();i++)
                {
                    // mlist.push_back(split(bufString,':')[i]);
                    mlist.push_back(hUser.BringMyFriend(split(bufString,':')[3],atoi(split(bufString,':')[i].c_str())));
                }
            }
            std::string code = hChat.EnterChatRoom(clntSock,mlist);
            std::string notic=split(bufString,':')[3]+"님이 입장하셨습니다. ";
            //그러면 이시점에서 채팅방의 다른 멤버들에게 나 입장한다는 메시지 보내줘야함
            hUser.SendNoticMSG(code,mlist,notic);
        }
        else if(msg=="Chat")
        {
            //채팅 관련 처리는 모두 여기서 하자
            std::string sep=split(bufString,':')[2];
            if(sep=="Connect")
            {
                //채팅방 멤버 접속상태 요청
                hChat.SendMemConnect(clntSock,split(bufString,':')[3],split(bufString,':')[4]);
                //이때 채팅방 멤버들한테 접속했음을 알려야함...
                //멤버들에게 접속한사람 닉네임을 알려주자 --성공
                hUser.SendEnterMem(split(bufString,':')[3],hChat.BringMemList(split(bufString,':')[3]),split(bufString,':')[4]);
            }
            else if(sep=="Send")
            {
                //채팅 메시지 수신
                //하려면 해당 채팅방 멤버 리스트를 가져와서
                //멤버에 해당하는 소켓에 메시지 보내야함
                hUser.SendChatMSG(split(bufString,':')[3],hChat.BringMemList(split(bufString,':')[3]),split(bufString,':')[4],split(bufString,':')[5]);
            }
            else if(sep=="Quit")
            {
                //채팅방 퇴장
                std::string notic=split(bufString,':')[4]+"님이 퇴장하셨습니다. :Q:"+split(bufString,':')[4];
                hUser.SendNoticMSG(split(bufString,':')[3],hChat.BringMemList(split(bufString,':')[3]),notic);
                //접속 상태 변경
                hChat.QuitChatRoom(split(bufString,':')[3],split(bufString,':')[4]);
                
                //채팅 수신 스레드 종료 시켜야함
                char buf[BUF_SIZE]={0};
                std::string quitM=":"+split(bufString,':')[3]+":quit";
                itoa(quitM.size(),buf,10);
                quitM=buf+quitM;
                send(clntSock,quitM.c_str(),quitM.size(),0);
            }
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
