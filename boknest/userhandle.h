#ifndef USER_HANDLE_H
#define USER_HANDLE_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>

struct UserInfo //유저 정보 저장 구조체
{
    std::string nick;
    SOCKET sock;
    bool conflag=true; //정보 추가시 자동 접속 상태로 설정됨.
    std::vector<std::string> friendList; //친구 목록(닉네임으로 저장)
};
class UserHandle
{
    //디폴트(private) 멤버
    static const unsigned int BUF_SIZE=1024;
    std::vector<UserInfo> userList; //접속한 유저 정보 저장

public:
    //유저 접속 종료
    void UserLogout(std::string);
    //유저 접속(등록할 소켓,닉네임)
    void UserLogin(SOCKET&,std::string);
    //친구 정보 전송(전송할 소켓)
    void SendFriendList(SOCKET&,std::string);
    //친구 추가
    void AddFriend(SOCKET&,std::string);
    //친구 찾기
    void FindFriend(SOCKET&,std::string);
    //귓속말 보내기
    void sendWhisper(SOCKET&,std::vector<std::string>);
    //친구 닉네임 가져오기
    std::string BringMyFriend(std::string,int);
    //채팅 멤버 상태 변경 전송(멤버리스트,상태변경한닉네임)
    void SendEnterMem(std::vector<std::string>,std::string);
    //채팅 멤버에게 공지 메시지 보내기
    void SendNoticMSG(std::vector<std::string>,std::string);
    //채팅 메시지 보내기
    void SendChatMSG(std::vector<std::string>,std::string,std::string);
};

#endif