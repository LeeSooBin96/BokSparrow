#include "chathandle.h"

//세팅
void ChatHandle::SettingChat(std::string nick,std::vector<std::string> slist)
{
    //초기화할것
    for(auto c: chat) c.clear();
    
    myNick=nick; //닉네임 저장
    code=slist[2]; //채팅 코드 저장
    // other=slist[3]; //상대 닉네임 저장 --지금은 1:1기준
    //채팅방 멤버 초기화 및 정보 저장
    memList.clear();
    for(int i=3;i<slist.size();i++)
    {
        MemInfo mem;
        mem.nick=slist[i];
        memList.push_back(mem);                     
    }
    //채팅창 제목 설정
    if(slist.size()==4)
        title=slist[3]+"님과의 채팅방"; 
    else
        title="복스패로우 채팅방";
}
//친구 상태정보 업데이트
void ChatHandle::UpdateMemState(std::vector<std::string> slist)
{
    for(int i=0;i<memList.size();i++)
    {
        if(slist[2+i]=="T") memList[i].connect=true;
        else memList[i].connect=false;
    }
    PrintChatScreen(); //채팅창 출력
}
//입장 멤버 상태 정보 업데이트
void ChatHandle::UpdateEnterMem(std::string nick)
{
    for(auto& mem: memList)
    {
        if(mem.nick==nick)
        {
            mem.connect=true;
            break;
        }
    }
    PrintChatScreen(); //채팅창 출력
}
//퇴장 멤버 상태 정보 업데이트
void ChatHandle::UpdateQuitMem(std::string nick)
{
    for(auto& mem: memList)
    {
        if(mem.nick==nick)
        {
            mem.connect=false;
            break;
        }
    }
}
//채팅창 출력
void ChatHandle::PrintChatScreen(void)
{
    system("cls");
    //채팅방 이름
    std::cout<<title<<std::endl;
    std::cout<<"==================================================\n";
    //채팅방 유저 목록
    for(auto mem: memList)
    {
        std::cout<<mem.nick<<"\t";
        if(mem.connect) std::cout<<"\x1b[32m●\x1b[m\n";
        else std::cout<<"\x1b[31m●\x1b[m\n";
    }
    std::cout<<"==================================================\n";
    //채팅 내용
    for(auto c: chat)
    {
        if(!c.empty()&&!c.compare(1,myNick.size(),myNick))
        {
            //내 대화 내용이면
            std::cout<<std::setw(55-c.size())<<std::right<<"\x1b[33m"<<c<<"\x1b[m"<<std::endl;
        }
        else std::cout<<c<<std::endl; //일단은 내용 상관없이 출력되게함
        //나중에 칸 넘어갔을때 개행도 넣어주자~~
    } 
    std::cout<<"==================================================\n";
    //채팅 입력창
    std::cout<<"(나가기: /Q, 파일 전송: /P, 파일 다운로드: /D/번호) \n";
    std::cout<<">>";
}
//공지 수신
void ChatHandle::ReceiveNotic(std::string notic)
{
    for(int i=0;i<9;i++)
    {
        chat[i]=chat[i+1];
    }
    chat[9]="\x1b[34m[공지] "+notic+"\x1b[m";
    PrintChatScreen();
}
//채팅 수신
void ChatHandle::ReceiveChat(std::string nick,std::string msg)
{
    for(int i=0;i<9;i++)
    {
        chat[i]=chat[i+1];
    }
    chat[9]="["+nick+"] "+msg;
    PrintChatScreen();
}