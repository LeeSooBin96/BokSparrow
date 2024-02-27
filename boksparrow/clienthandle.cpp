#include "clienthandle.h"

//서버 접속(로그인) 프로세스
bool ClientHandle::LoginProcess(SOCKET& sock)
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    std::cout<<"==================================================\n"
                "닉네임을 입력해주세요 (20자이하)>";
    
    std::getline(std::cin,nickName);
    while(nickName.size()>20||nickName.size()==0)
    {
        std::cout<<"==================================================\n"
                "다시 입력해주세요 (20자이하)>";
        std::getline(std::cin,nickName);
    }
    //서버에 전송해서 중복여부 검사(전달 형태> 문자열 총길이:Nick:nickName)
    nickName=":Nick:"+nickName;
    itoa(nickName.size(),buffer,10);
    msg=buffer+nickName;
    send(sock,msg.c_str(),msg.size(),0); //서버에 전송
    nickName.erase(nickName.find(':'),6); //닉네임 저장
    //서버로부터 결과 수신
    if(recv(sock,buffer,BUF_SIZE,0)<0) ReceiveError();
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
void ClientHandle::RequestFriendList(SOCKET& sock)
{
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    //친구 정보 요청
    msg=":FriendList:"+nickName;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//친구 목록 출력
void ClientHandle::PrintFriendList(std::vector<std::string> flist)
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
            bool ck=false;
            if(Cflist.size()!=0)
            {
                for(int index: Cflist)
                {
                    if(index==i/2)
                    {
                        ck=true;
                        break;
                    }
                }
            }
            if(ck) std::cout<<"\x1b[34m"+flist[3+i]+"\x1b[m\t";
            else if(posforF==i/2) std::cout<<"\x1b[33m"+flist[3+i]+"\x1b[m\t";
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
    if(posforF==2+friendNum) std::cout<<"\x1b[33m채팅방 입장 > \x1b[m\n";
    else std::cout<<"채팅방 입장 > \n";
    if(posforF==3+friendNum) std::cout<<"\x1b[33m종료 > \x1b[m\n";
    else std::cout<<"종료 > \n";
    if(chooseFriend) std::cout<<"\x1b[34m함께할 친구를 선택하세요 (선택종료는 x)>\x1b[m";
}
//친구 창 프로세스
bool ClientHandle::ProcessFriendScreen(ClientBase& clnt)
{
    //친구 목록 창 메뉴 선택
    while(true)
    {
        int input;
        bool quit=false;
        RequestFriendList(clnt.sock);
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
                    RequestFriendList(clnt.sock);
                }
                break;
            case 80: 
            case 115:
                if(posforF!=3+friendNum)
                {
                    posforF++;
                    RequestFriendList(clnt.sock);
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
            //친구 선택한것 --1:1 채팅방
            if(ConnectMyFriend(clnt.sock)) return true; //채팅 고르면 나오게
            //여기서 대기 되게 하거나
        }
        else if(posforF==friendNum)
        {
            //친구 찾기 --닉네임 입력-서버 송신-수신-결과 출력
            AddorFindMyFriend(clnt.sock,"Find");
        }
        else if(posforF==friendNum+1)
        {
            //친구 추가 
            AddorFindMyFriend(clnt.sock,"Add");
        }
        else if(posforF==friendNum+2)
        {
            // //채팅방 입장 --1:N 채팅방
            if(EnterChatRoom(clnt.sock)) return true;
            // //서버에 채팅방 목록 정보 요청
            // //보낼메시지) 메시지총길이:Clist:닉네임 --> 서버) 메시지총길이:clist:채팅방개수:채팅방이름:채팅코드...
            // std::string msg=":Clist:"+nickName;
            // char buf[BUF_SIZE]={0};
            // itoa(msg.size(),buf,10);
            // msg=buf+msg;
            // send(clnt.sock,msg.c_str(),msg.size(),0);
            // //얘도 여기서 대기 시킬게 필요할듯
            // //그냥 나가게 하자
            // return true;
        }
        else if(posforF==friendNum+3)
        {
            //종료
            std::cout<<"종료됩니다. \n";
            clnt.~ClientBase();
            break;
        }
    }
    return false;
}
//친구 추가 또는 찾기
void ClientHandle::AddorFindMyFriend(SOCKET& sock,std::string AorF)
{ //--닉네임 입력-서버 송신-수신-친구 목록 업데이트
    //버퍼로 사용할 문자열
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    std::string friendNick;
    std::cout<<"==================================================\n";
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
    send(sock,msg.c_str(),msg.size(),0);
}
//찾은 친구 목록 출력
void ClientHandle::PrintFoundFriend(std::vector<std::string> flist)
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
    Sleep(1000);
}
//친구 연결 --posforF로 친구 인덱스 읽을 수 있음
bool ClientHandle::ConnectMyFriend(SOCKET& sock)
{
    //선택한 시점의 posforF가 친구 리스트 인덱스 넘버
    //귓속말 혹은 채팅방 입장 선택하게 해야함
    std::cout<<"==================================================\n";
    std::cout<<"귓속말 보내기(W) 1:1 채팅방 입장(C) 이전으로(X)>\n";
    bool quit=false;
    int input;
    while(!quit)
    {
        input=getch();
        switch (input)
        {
        case 119: //귓속말 보내기
        {
            std::cout<<"보낼 메시지 >";
            std::string msg;
            std::getline(std::cin,msg);
            //입력받은 메시지 서버에 전송해야한다.
            //보낼 내용) 메시지 전체 길이:Whisper:내 닉네임:친구 인덱스:메시지
            char buf[BUF_SIZE]; //길이 문자열로 바꿀 버퍼
            itoa(posforF,buf,10);
            msg=":Whisper:"+nickName+":"+buf+":"+msg;
            itoa(msg.size(),buf,10);
            msg=buf+msg;
            send(sock,msg.c_str(),msg.size(),0);
            std::cout<<"==================================================\n"
                        "귓속말 보내기(W) 1:1 채팅방 입장(C) 이전으로(X)>\n";
        }
            break;
        case 99: //1:1 채팅방 입장
        {
            //여기서 채팅방 다루는 함수로 넘겨야 할 듯.
            //우선 1:1채팅방에 입장한다는걸 서버에 알려야함
            //있는 방이면 기존 방에 없는방이면 새로 생성한 방으로
            //그렇담 보낼 메시지는
            //메시지총길이:Enter:OtO:내닉네임:친구인덱스
            char buf[BUF_SIZE]; //길이 문자열로 바꿀 버퍼
            itoa(posforF,buf,10);
            std::string msg=":Enter:OtO:"+nickName+":"+buf;
            itoa(msg.size(),buf,10);
            msg=buf+msg;
            send(sock,msg.c_str(),msg.size(),0);
            //여기서 대기 되게 하거나
            return true;
        }
            quit=true; //위에는 없앴는데 예는 있어야할듯
            break;
        case 120: //이전으로
            quit=true;
            break;
        default:
            break;
        }
    }
    return false;
    //119 ,99 ,120
}
//채팅방 입장
bool ClientHandle::EnterChatRoom(SOCKET& sock)
{
    chooseFriend=true;
    Cflist.clear();
    // std::vector<int> flist;
    // std::cout<<"()"
    // while(true)
    // {
    //     std::string friendNick;
    //     std::cout<<"함께할 친구의 닉네임을 입력하세요 >";
    //     std::getline(std::cin,friendNick);
    // }
    if(friendNum==0) return false;
    posforF=0;
    while(true)
    {
        int input;
        bool quit=false;
        RequestFriendList(sock);
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
                    RequestFriendList(sock);
                }
                break;
            case 80: 
            case 115:
                if(posforF!=friendNum-1)
                {
                    posforF++;
                    RequestFriendList(sock);
                }
                break;
            case 13:
                quit=true;
                break;
            case 120:
            {
                chooseFriend=false;
                if(Cflist.size()==0) return false;
                char buf[BUF_SIZE]={0};
                std::string msg=":Enter:OtM:"+nickName;
                for(int index: Cflist)
                {
                    itoa(index,buf,10);
                    msg.append(":");
                    msg.append(buf);
                    memset(buf,0,BUF_SIZE);
                }
                itoa(msg.size(),buf,10);
                msg=buf+msg;
                send(sock,msg.c_str(),msg.size(),0);
                Cflist.clear();
                return true;
            }
                break;
            default:
                continue;
            }
        }
        if(Cflist.size()!=0) //중복선택 검사
        {
            for(int index : Cflist)
            {
                if(index==posforF) continue;
            }
        }
        Cflist.push_back(posforF);
    }
    Cflist.clear();
    chooseFriend=false;
    return false;
}