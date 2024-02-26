#include "chathandle.h"

//세팅
void ChatHandle::SettingChat(std::string nick,std::vector<std::string> slist)
{
    myNick=nick; //닉네임 저장
    code=slist[2]; //채팅 코드 저장
    // other=slist[3]; //상대 닉네임 저장 --지금은 1:1기준
    //채팅방 멤버 초기화 및 정보 저장
    memList.clear();
    MemInfo mem;
    mem.nick=slist[3];
    memList.push_back(mem);
    //채팅창 출력
    title=slist[3]+"님과의 채팅방"; //그래도 아직까지 1:1기준 나중에 1:N도 작동하도록 수정
    PrintChatScreen();
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
    for(auto c: chat) std::cout<<c<<std::endl; //일단은 내용 상관업이 출력되게함
    std::cout<<"==================================================\n";
    //채팅 입력창
}
//채팅 송신 스레드 --메시지 입력 부분
unsigned WINAPI ChatHandle::InputChat(void* arg)
{
    SOCKET sock=*((SOCKET*)arg);

    return 0;
}
//채팅 수신 스레드 --메시지 출력 부분
unsigned WINAPI ChatHandle::OuputChat(void* arg)
{
    SOCKET sock=*((SOCKET*)arg);

    return 0;
}