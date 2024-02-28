#include "chatroom.h"

//채팅방 목록 조회
void ChatRoom::ShowChatRoomList(SOCKET& sock,std::string nick)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":clist:";
    int count=0;
    std::vector<std::string> codeList,titleList;
    for(auto room: roomList)
    {
        for(auto mem: room.list)
        {
            if(mem.nick==nick)
            {
                count++; 
                titleList.push_back(room.title);
                codeList.push_back(room.code);
                break;
            }
        }
    }
    if(count==0) //방하나도 없으면
    { 
        send(sock,"8:clist:0",10,0);
        return;
    }
    itoa(count,buffer,10);
    msg.append(buffer);
    msg.append(":");
    // for(auto c:codeList) msg.append(c+":");
    for(int i=0;i<count;i++)
    {
        msg.append(titleList[i]+":"+codeList[i]+":");
    }
    msg.pop_back();
    //여기까지 보낼 메시지 생성 완료
    memset(buffer,0,BUF_SIZE);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//1:1채팅방 입장(코드전달할소켓,멤버리스트) --일단 여기부터 수정
//1:N이어도 작동하게끔 지금 소켓이랑 채팅방 멤버 리스트 전달되고 있음!
std::string ChatRoom::EnterChatRoom(SOCKET& sock,std::vector<std::string> mlist)
{ //그럼 이 멤버리스트 반환하게 하면 채팅방 멤버 모두에게 접속 메시지도 날릴 수 있겠다!
    srand(time(NULL));
    //사용할 버퍼
    char buffer[BUF_SIZE];
    std::string code;
    //코드 시작 구분자
    if(mlist.size()==2) code="P";
    else code="M";
    //기존방있는 경우
    int count=0; 
    for(auto room: roomList)
    {
        if(room.list.size()==mlist.size()) //멤버수 같은 방만 검사
        {
            for(auto mem:room.list)
            {
                // if(mem.nick==mlist[0]) count++;
                // if(mem.nick==mlist[1]) count++;
                for(auto m: mlist)
                {
                    if(m==mem.nick)
                    { 
                        count++;
                        break;
                    }
                }
            }
        }
        if(count==mlist.size())
        {
            code=room.code; //기존 방코드 가져오기
            break;
        }
        else count=0; //카운트 초기화
    }
    if(count!=mlist.size()) //무사 통과했다면
    {
        //채팅방 생성
        ChatRoomInfo room;
        for(auto m: mlist)
        {
            MemInfo mem;
            mem.nick=m;
            room.list.push_back(mem);
        }
        //채팅 코드 생성
        while(true)
        {
            itoa(rand()%900+100,buffer,10);
            code.append(buffer);
            if(roomList.size()==0) //방이 하나도 없다면
            {
                break;
            }
            else //방이 하나라도 있다면
            { //채팅방 코드 중복되는지 검사
                bool ck=true;
                for(auto r:roomList)
                {
                    if(r.code==code)
                    {
                        ck=false;
                        break;
                    }
                }
                if(ck) break;
            }
        }
        room.code=code;
        roomList.push_back(room);
    }
    //클라이언트에게 보낼 메시지) 메시지총길이:enter:채팅코드:상대닉네임
    std::string msg=":enter:"+code;
    for(int i=0;i<mlist.size()-1;i++)
    {
        msg.append(":"+mlist[1+i]);
    }
    memset(buffer,0,BUF_SIZE);
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
    return code;
}
//채팅방 멤버 상태정보 전송
void ChatRoom::SendMemConnect(SOCKET& sock,std::string code,std::string nick)
{
    //여기서 내 상태정보 수정 -->채팅방 멤버들에게 접속했음을 알려야함
    std::string msg=":"+code+":connect:";
    for(auto& room: roomList)
    {
        if(room.code==code)
        {
            for(auto& mem:room.list)
            {
                if(mem.nick!=nick)
                {
                    if(mem.connect) msg.append("T:");
                    else msg.append("F:");
                }
                else
                {
                    mem.connect=true;
                }
            }
            break;
        }
    }msg.pop_back();
    char buffer[BUF_SIZE]={0};
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//채팅방 멤버 리스트 반환(채팅 코드 전달받음)
std::vector<std::string> ChatRoom::BringMemList(std::string Ccode)
{
    std::vector<std::string> memList;
    for(auto room: roomList)
    {
        if(room.code==Ccode)
        {
            for(auto m: room.list)
            {
                memList.push_back(m.nick);
            }
        }
    }
    return memList;
}
//채팅방 퇴장
void ChatRoom::QuitChatRoom(std::string code,std::string nick)
{
    //여기서는 상태 변경만
    for(auto room: roomList)
    {
        if(room.code==code)
        {
            for(auto mem: room.list)
            {
                if(mem.nick==nick) mem.connect=false;
            }
        }
    }
}