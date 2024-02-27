#ifndef CLIENT_HANDLE_H
#define CLIENT_HANDLE_H

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <vector>

#include "clientbase.h"

class ClientHandle
{
    //����Ʈ(private) ���
    static const unsigned int BUF_SIZE=1024;

    unsigned int friendNum=0; //ģ�� �� ����
    unsigned int posforF=0; //ģ��â Ŀ�� ��ġ
    std::vector<int> Cflist; //ä�ù� �ʴ� ģ�� �ε��� ����Ʈ
    bool chooseFriend=false; //ä�ù� �ʴ� ��� ���� ����

public:
    std::string nickName; //�г����� ������ ���ڿ�
    
    //���� ���� --������ ����
    void ReceiveError(void) {std::cout<<"���� ���� \n"; exit(1);};
    //���� ����(�α���) ���μ���
    bool LoginProcess(SOCKET&);
    //ģ�� ���� ��û
    void RequestFriendList(SOCKET&);
    //ģ�� â ���μ���
    bool ProcessFriendScreen(ClientBase&);
    //ģ�� ��� ���
    void PrintFriendList(std::vector<std::string>);
    //ģ�� �߰�
    void AddorFindMyFriend(SOCKET&,std::string);
    //ģ�� ã��
    void PrintFoundFriend(std::vector<std::string>);
    //ģ�� ���� --posforF�� ģ�� �ε��� ���� �� ����
    bool ConnectMyFriend(SOCKET&);
    //ä�ù� ����
    bool EnterChatRoom(SOCKET&);
};
#endif