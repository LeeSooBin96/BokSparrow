#ifndef CHAT_ROOM_H
#define CHAT_ROOM_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <ctime>

struct MemInfo
{
    std::string nick; //�г���
    bool connect=false; //���ӻ���
};
struct ChatRoomInfo
{
    std::string title;
    std::string code; //1:1 P, 1:N M
    std::vector<MemInfo> list; //�������Ʈ...���ӻ��µ� Ȯ�εǾ����
};

class ChatRoom
{
    //����Ʈ(private) ���
    static const unsigned int BUF_SIZE=1024;
    std::vector<ChatRoomInfo> roomList;

public:
    //1:1ä�ù� ����(�ڵ������Ҽ���,�������Ʈ)
    void EnterChatRoom(SOCKET&,std::vector<std::string>);
    //ä�ù� ��� �������� ����(����,ä���ڵ�,���г���)
    void SendMemConnect(SOCKET&,std::string,std::string);
    //ä�ù� ��� ����Ʈ ��ȯ(ä�� �ڵ� ���޹���)
    std::vector<std::string> BringMemList(std::string);
    //ä�ù� ����(ä�ù� �ڵ�, �г���)
    void QuitChatRoom(std::string,std::string);
};
#endif