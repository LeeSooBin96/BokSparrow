#include "chathandle.h"

//����
void ChatHandle::SettingChat(std::string nick,std::vector<std::string> slist)
{
    //�ʱ�ȭ�Ұ�
    for(auto c: chat) c.clear();
    
    myNick=nick; //�г��� ����
    code=slist[2]; //ä�� �ڵ� ����
    // other=slist[3]; //��� �г��� ���� --������ 1:1����
    //ä�ù� ��� �ʱ�ȭ �� ���� ����
    memList.clear();
    for(int i=3;i<slist.size();i++)
    {
        MemInfo mem;
        mem.nick=slist[i];
        memList.push_back(mem);                     
    }
    //ä��â ���� ����
    if(slist.size()==4)
        title=slist[3]+"�԰��� ä�ù�"; 
    else
        title="�����зο� ä�ù�";
}
//ģ�� �������� ������Ʈ
void ChatHandle::UpdateMemState(std::vector<std::string> slist)
{
    for(int i=0;i<memList.size();i++)
    {
        if(slist[2+i]=="T") memList[i].connect=true;
        else memList[i].connect=false;
    }
    PrintChatScreen(); //ä��â ���
}
//���� ��� ���� ���� ������Ʈ
void ChatHandle::UpdateEnterMem(std::string nick)
{
    for(auto& mem: memList)
    {
        if(mem.nick==nick)
        {
            mem.connect=true;
            break;
        }
    }
    PrintChatScreen(); //ä��â ���
}
//���� ��� ���� ���� ������Ʈ
void ChatHandle::UpdateQuitMem(std::string nick)
{
    for(auto& mem: memList)
    {
        if(mem.nick==nick)
        {
            mem.connect=false;
            break;
        }
    }
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
    for(auto c: chat)
    {
        if(!c.empty()&&!c.compare(1,myNick.size(),myNick))
        {
            //�� ��ȭ �����̸�
            std::cout<<std::setw(55-c.size())<<std::right<<"\x1b[33m"<<c<<"\x1b[m"<<std::endl;
        }
        else std::cout<<c<<std::endl; //�ϴ��� ���� ������� ��µǰ���
        //���߿� ĭ �Ѿ���� ���൵ �־�����~~
    } 
    std::cout<<"==================================================\n";
    //ä�� �Է�â
    std::cout<<"(������: /Q, ���� ����: /P, ���� �ٿ�ε�: /D/��ȣ) \n";
    std::cout<<">>";
}
//���� ����
void ChatHandle::ReceiveNotic(std::string notic)
{
    for(int i=0;i<9;i++)
    {
        chat[i]=chat[i+1];
    }
    chat[9]="\x1b[34m[����] "+notic+"\x1b[m";
    PrintChatScreen();
}
//ä�� ����
void ChatHandle::ReceiveChat(std::string nick,std::string msg)
{
    for(int i=0;i<9;i++)
    {
        chat[i]=chat[i+1];
    }
    chat[9]="["+nick+"] "+msg;
    PrintChatScreen();
}