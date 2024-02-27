#ifndef CHAT_HANDLE_H
#define CHAT_HANDLE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>


struct MemInfo
{
    std::string nick; //�г���
    bool connect; //���ӻ���
};
class ChatHandle
{
    //����Ʈ(private) ���
    //ä��â ����
    std::string title; //ä�ù� �̸�
    std::vector<MemInfo> memList; //ä�ù� ���� ���
    std::string chat[10]; //ä�� �α�


public:
    //���� ��� ����
    std::string myNick; //�г���
    std::string code; //ä�� �ڵ�

    //����
    void SettingChat(std::string,std::vector<std::string>);
    //ģ�� �������� ������Ʈ
    void UpdateMemState(std::vector<std::string>);
    //���� ��� ���� ���� ������Ʈ
    void UpdateEnterMem(std::string);
    //���� ��� ���� ���� ������Ʈ
    void UpdateQuitMem(std::string);
    //ä��â ���
    void PrintChatScreen(void);
    //���� ����
    void ReceiveNotic(std::string);
    //ä�� ����
    void ReceiveChat(std::string,std::string);
};
#endif