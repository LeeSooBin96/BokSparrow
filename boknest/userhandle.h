#ifndef USER_HANDLE_H
#define USER_HANDLE_H

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>

struct UserInfo //���� ���� ���� ����ü
{
    std::string nick;
    SOCKET sock;
    bool conflag=true; //���� �߰��� �ڵ� ���� ���·� ������.
    std::vector<std::string> friendList; //ģ�� ���(�г������� ����)
};
class UserHandle
{
    //����Ʈ(private) ���
    static const unsigned int BUF_SIZE=1024;
    std::vector<UserInfo> userList; //������ ���� ���� ����

public:
    //���� ���� ����
    void UserLogout(std::string);
    //���� ����(����� ����,�г���)
    void UserLogin(SOCKET&,std::string);
    //ģ�� ���� ����(������ ����)
    void SendFriendList(SOCKET&,std::string);
    //ģ�� �߰�
    void AddFriend(SOCKET&,std::string);
    //ģ�� ã��
    void FindFriend(SOCKET&,std::string);
    //�ӼӸ� ������
    void sendWhisper(SOCKET&,std::vector<std::string>);
};

#endif