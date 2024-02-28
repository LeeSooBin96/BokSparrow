#include "userhandle.h"

//유저 접속 종료
void UserHandle::UserLogout(std::string nick)
{
    for(auto& mem: userList) //종료된 이후 플래그 조정
    {
        if(mem.nick==nick) //소켓이 아닌 닉네임으로 구분하는게 정확
        {
            mem.conflag=false;
        }
    }
}
//유저 접속(등록할 소켓,닉네임)
void UserHandle::UserLogin(SOCKET& sock,std::string nick)
{
    //닉네임 중복 여부 검사
    for(auto mem: userList)
    {
        if(mem.nick==nick) //중복인 닉네임이 있다면
        {
            send(sock,"Existed",8,0);
            return;
        }
    }
    //중복 없는 새 회원이면
    UserInfo user;
    user.sock=sock;
    user.nick=nick;
    userList.push_back(user); //유저 리스트에 추가
    //접속 성공여부 전송
    send(sock,"Success",8,0);
}
//친구 정보 전송(전송할 소켓)
void UserHandle::SendFriendList(SOCKET& sock,std::string nick)
{
    char tmp[BUF_SIZE]={0};
    std::string msg;
    //우선 해당하는 유저 정보 구조체 찾아야함
    for(auto& mem: userList) //값 변경할때는 참조로!!! --잊지 말자 ㅠㅠ
    {
        if(mem.nick==nick)
        {
            if(mem.sock!=sock)
            {
                mem.sock=sock; //재접속인 경우 소켓정보 업데이트
                mem.conflag=true; //접속상태 업데이트
            }
            //이제 친구 정보 보내줘야함
            /*형태:문자열길이:친구 수:친구닉네임:접속상태:...*/
            if(mem.friendList.size()>0)
            {
                itoa(mem.friendList.size(),tmp,10); //친구수
                msg.append(tmp);
                msg=":friend:"+msg+":";
                for(int i=0;i<mem.friendList.size();i++)
                {
                    msg.append(mem.friendList[i]+":"); //닉네임 추가
                    for(auto m:userList)
                    {
                        if(mem.friendList[i]==m.nick)
                        { //접속상태 추가
                            if(m.conflag)
                            {
                                msg.append("T:");
                            }
                            else
                            {
                                msg.append("F:");
                            }
                            break;
                        }
                    }
                }
                msg.pop_back();
                //:친구수:친구이름:상태... 이제 길이 추가하자
                memset(tmp,0,BUF_SIZE);
                itoa(msg.size(),tmp,10);
                msg=tmp+msg;
                send(sock,msg.c_str(),msg.size(),0);
            }
            else
            {
                send(sock,"12:friend:none",15,0);
            }
            break;
        }
    }
}
//친구 추가
void UserHandle::AddFriend(SOCKET& sock,std::string nick)
{
    for(auto& mem: userList)
    {
        if(mem.sock==sock)
        {
            bool ck=false;
            for(auto m: mem.friendList)
            {
                if(m==nick) //기존 친구 리스트에 있으면
                {
                    ck=true; break;
                }
            }
            if(!ck) //친구리스트에 없으면
            { //사실 바로 추가가 아니라 유저 목록에 있는지 확인하고 상대방것에도 나를 추가해야함
                //작업이 복잡하군
                std::string msg;
                for(auto& m: userList)
                {
                    if(m.nick==nick)//유저 목록에 있다면
                    {
                        m.friendList.push_back(mem.nick);//친구것에도 추가해야함
                        mem.friendList.push_back(nick); //이게 왜 안먹히지--해결 하...
                        send(sock,"12:add:Success",15,0);
                        return;
                    }
                }
            }
            break;
        }
    }
    send(sock,"11:add:Failed",14,0);
}
//친구 찾기
void UserHandle::FindFriend(SOCKET& sock,std::string keyword)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":";
    int count=0;
    for(auto mem: userList)
    {   //자기자신 제외하고
        if(sock!=mem.sock&&mem.nick.find(keyword)<mem.nick.size()) //키워드 포함하는 닉네임 있으면
        {
            count++;
            msg.append(mem.nick+":");
            if(mem.conflag)
            {
                msg.append("T:");
            }
            else
            {
                msg.append("F:");
            }
        }
    }
    msg.pop_back();
    if(count==0) send(sock,"10:find:none",13,0);
    itoa(count,buffer,10);
    msg=buffer+msg; msg=":find:"+msg;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
    std::cout<<msg<<std::endl;
}
//귓속말 보내기
void UserHandle::sendWhisper(SOCKET& sock,std::vector<std::string> mlist)
{
    //전달된 메시지) 메시지 전체 길이:Whisper:내 닉네임:친구 인덱스:메시지
    for(auto mem: userList)
    {
        if(mem.nick==mlist[2])
        {
            for(auto m: userList)
            {
                if(m.nick==mem.friendList[atoi(mlist[3].c_str())])
                {
                    //보낼 메시지) 메시지 전체 길이:whisper:보낸사람닉네임:메시지
                    std::string msg=":whisper:"+mem.nick+":"+mlist[4];
                    char buf[BUF_SIZE];
                    itoa(msg.size(),buf,10);
                    msg=buf+msg;
                    send(m.sock,msg.c_str(),msg.size(),0);
                    break;
                }
            }
            break;
        }
    }
}
//친구 닉네임 가져오기
std::string UserHandle::BringMyFriend(std::string myNick,int index)
{
    std::string friendNick;
    for(auto mem:userList)
    {
        if(mem.nick==myNick)
        {
            friendNick=mem.friendList[index];
            break;
        }
    }
    return friendNick;
}
//채팅 멤버 상태 변경 전송(멤버리스트,상태변경한닉네임)
void UserHandle::SendEnterMem(std::string code,std::vector<std::string> mlist,std::string nick)
{
    char buffer[BUF_SIZE]={0};
    std::string msg=":"+code+":enterM:"+nick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;

    for(auto mem: mlist)
    {
        for(auto user: userList)
        {
            if(user.nick!=nick&&user.nick==mem) //상태 변경한 유저 제외한 멤버들에게 전송
            {
                send(user.sock,msg.c_str(),msg.size(),0);
            }
        }
    }
}
//채팅 멤버에게 공지 메시지 보내기
void UserHandle::SendNoticMSG(std::string code,std::vector<std::string> mlist,std::string msg)
{
    char buffer[BUF_SIZE]={0};
    std::string notic=":"+code+":notic:"+msg+":";
    itoa(notic.size(),buffer,10);
    notic=buffer+notic;

    for(auto mem: mlist)
    {
        for(auto user: userList)
        {
            if(user.nick==mem)
            {
                send(user.sock,notic.c_str(),notic.size(),0);
            }
        }
    }
}
//채팅 메시지 보내기
void UserHandle::SendChatMSG(std::string code,std::vector<std::string> mlist,std::string nick,std::string msg)
{ 
    std::string chat;
    char buffer[BUF_SIZE]={0};
    chat=":"+code+":chat:"+nick+":"+msg;
    itoa(chat.size(),buffer,10);
    chat=buffer+chat;
    for(auto mem: mlist)
    {
        for(auto user: userList)
        {
            if(user.nick==mem)
            {
                send(user.sock,chat.c_str(),chat.size(),0);
            }
        }
    }
}