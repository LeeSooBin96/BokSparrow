#ifndef CHAT_HANDLE_H
#define CHAT_HANDLE_H

#include <iostream>
#include <vector>
#include <string>

#include "Windows.h"

struct MemInfo
{
    std::string nick; //�г���
    bool connect; //���ӻ���
};
class ChatHandle
{
    //����Ʈ(private) ���
    //���� ��� ����
    std::string myNick; //�г���
    std::string code; //ä�� �ڵ�
    //ä��â ����
    std::string title; //ä�ù� �̸�
    std::vector<MemInfo> memList; //ä�ù� ���� ���
    std::string chat[10]; //ä�� �α�


public:
    //����
    void SettingChat(std::string,std::vector<std::string>);
    //ä��â ���
    void PrintChatScreen(void);
    //ä�� �۽� ������
    static unsigned WINAPI InputChat(void* arg);
    //ä�� ���� ������
    static unsigned WINAPI OuputChat(void* arg);
};
#endif