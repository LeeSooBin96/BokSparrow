#include "chathandle.h"

//����
void ChatHandle::SettingChat(std::string nick,std::vector<std::string> slist)
{
    myNick=nick; //�г��� ����
    code=slist[2]; //ä�� �ڵ� ����
    // other=slist[3]; //��� �г��� ���� --������ 1:1����
    //ä�ù� ��� �ʱ�ȭ �� ���� ����
    memList.clear();
    MemInfo mem;
    mem.nick=slist[3];
    memList.push_back(mem);
    //ä��â ���
    title=slist[3]+"�԰��� ä�ù�"; //�׷��� �������� 1:1���� ���߿� 1:N�� �۵��ϵ��� ����
    PrintChatScreen();
}
//ä��â ���
void ChatHandle::PrintChatScreen(void)
{
    system("cls");
    //ä�ù� �̸�
    std::cout<<title<<std::endl;
    std::cout<<"==================================================\n";
    //ä�ù� ���� ���
    for(auto mem: memList)
    {
        std::cout<<mem.nick<<"\t";
        if(mem.connect) std::cout<<"\x1b[32m��\x1b[m\n";
        else std::cout<<"\x1b[31m��\x1b[m\n";
    }
    std::cout<<"==================================================\n";
    //ä�� ����
    for(auto c: chat) std::cout<<c<<std::endl; //�ϴ��� ���� ������� ��µǰ���
    std::cout<<"==================================================\n";
    //ä�� �Է�â
}
//ä�� �۽� ������ --�޽��� �Է� �κ�
unsigned WINAPI ChatHandle::InputChat(void* arg)
{
    SOCKET sock=*((SOCKET*)arg);

    return 0;
}
//ä�� ���� ������ --�޽��� ��� �κ�
unsigned WINAPI ChatHandle::OuputChat(void* arg)
{
    SOCKET sock=*((SOCKET*)arg);

    return 0;
}