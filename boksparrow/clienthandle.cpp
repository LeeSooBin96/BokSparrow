#include "clienthandle.h"

//���� ����(�α���) ���μ���
bool ClientHandle::LoginProcess(SOCKET& sock)
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    std::cout<<"==================================================\n"
                "�г����� �Է����ּ��� (20������)>";
    
    std::getline(std::cin,nickName);
    while(nickName.size()>20||nickName.size()==0)
    {
        std::cout<<"==================================================\n"
                "�ٽ� �Է����ּ��� (20������)>";
        std::getline(std::cin,nickName);
    }
    //������ �����ؼ� �ߺ����� �˻�(���� ����> ���ڿ� �ѱ���:Nick:nickName)
    nickName=":Nick:"+nickName;
    itoa(nickName.size(),buffer,10);
    msg=buffer+nickName;
    send(sock,msg.c_str(),msg.size(),0); //������ ����
    nickName.erase(nickName.find(':'),6); //�г��� ����
    //�����κ��� ��� ����
    if(recv(sock,buffer,BUF_SIZE,0)<0) ReceiveError();
    if(!strncmp(buffer,"Existed",8))
    {
        std::cout<<"==================================================\n"
                "�̹� ��ϵ� �����Դϴ�. ������ �½��ϱ�? (Y/N)";
        while(true)
        {
            std::string answer;
            std::getline(std::cin,answer);
            if(answer=="y"||answer=="Y")
            {
                //ģ�� ���� ������ ���� ������ ������ ��û
                //������ ������ �г���-���� ������ �ȵ� �켱 ������ ���� ��û�ؾ���
                // RequestFriendList(clnt);
                return true;
                break;
            }
            else if(answer=="n"||answer=="N")
            {
                //�г��� �ٽ� �Է�
                return false;
                break;
            }
            else
            {
                std::cout<<"�ٽ� �Է����ּ��� >";
            }   
        }
    }
    else if(!strncmp(buffer,"Success",8))
    {
        //�г��� ��� �� ���� �Ϸ�Ǹ� ģ�� ���� ��û
        // RequestFriendList(clnt);
        return true;
    }
    return false;
}
//ģ�� ���� ��û
void ClientHandle::RequestFriendList(SOCKET& sock)
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    //ģ�� ���� ��û
    msg=":FriendList:"+nickName;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//ģ�� ��� ���
void ClientHandle::PrintFriendList(std::vector<std::string> flist)
{
    system("cls");
    std::cout<<"==================================================\n";
    if(flist[2]=="none")
    {
        std::cout<<"���� ģ���� �����ϴ�. \n";
    }
    else
    {
        //ģ�� �г��� ���ӻ��� ����������� ����� ��
        friendNum=atoi(flist[2].c_str()); //ģ�� �� ����
        for(int i=0;i<friendNum*2;i+=2)
        {
            bool ck=false;
            if(Cflist.size()!=0)
            {
                for(int index: Cflist)
                {
                    if(index==i/2)
                    {
                        ck=true;
                        break;
                    }
                }
            }
            if(ck) std::cout<<"\x1b[34m"+flist[3+i]+"\x1b[m\t";
            else if(posforF==i/2) std::cout<<"\x1b[33m"+flist[3+i]+"\x1b[m\t";
            else std::cout<<flist[3+i]+"\t";
            if(flist[4+i]=="F")
            {
                std::cout<<"\x1b[31m��\x1b[m\n";
            }
            else
            {
                std::cout<<"\x1b[32m��\x1b[m\n";
            }
        }
    }
    std::cout<<"==================================================\n";
    if(posforF==0+friendNum) std::cout<<"\x1b[33mģ�� ã�� > \x1b[m\n";
    else std::cout<<"ģ�� ã�� > \n";
    if(posforF==1+friendNum) std::cout<<"\x1b[33mģ�� �߰� > \x1b[m\n";
    else std::cout<<"ģ�� �߰� > \n";
    if(posforF==2+friendNum) std::cout<<"\x1b[33mä�ù� ���� > \x1b[m\n";
    else std::cout<<"ä�ù� ���� > \n";
    if(posforF==3+friendNum) std::cout<<"\x1b[33m���� > \x1b[m\n";
    else std::cout<<"���� > \n";
    if(chooseFriend) std::cout<<"\x1b[34m�Բ��� ģ���� �����ϼ��� (��������� x)>\x1b[m";
}
//ģ�� â ���μ���
bool ClientHandle::ProcessFriendScreen(ClientBase& clnt)
{
    //ģ�� ��� â �޴� ����
    while(true)
    {
        int input;
        bool quit=false;
        RequestFriendList(clnt.sock);
        while(!quit)
        {
            input=getch();
            switch (input)
            {
            case 72:  
            case 119:
                if(posforF!=0) 
                {
                    posforF--;
                    RequestFriendList(clnt.sock);
                }
                break;
            case 80: 
            case 115:
                if(posforF!=3+friendNum)
                {
                    posforF++;
                    RequestFriendList(clnt.sock);
                }
                break;
            case 13:
                quit=true;
                break;
            default:
                continue;
            }
        }
        if(posforF<friendNum)
        {
            //ģ�� �����Ѱ� --1:1 ä�ù�
            if(ConnectMyFriend(clnt.sock)) return true; //ä�� ���� ������
            //���⼭ ��� �ǰ� �ϰų�
        }
        else if(posforF==friendNum)
        {
            //ģ�� ã�� --�г��� �Է�-���� �۽�-����-��� ���
            AddorFindMyFriend(clnt.sock,"Find");
        }
        else if(posforF==friendNum+1)
        {
            //ģ�� �߰� 
            AddorFindMyFriend(clnt.sock,"Add");
        }
        else if(posforF==friendNum+2)
        {
            // //ä�ù� ���� --1:N ä�ù�
            if(EnterChatRoom(clnt.sock)) return true;
            // //������ ä�ù� ��� ���� ��û
            // //�����޽���) �޽����ѱ���:Clist:�г��� --> ����) �޽����ѱ���:clist:ä�ù氳��:ä�ù��̸�:ä���ڵ�...
            // std::string msg=":Clist:"+nickName;
            // char buf[BUF_SIZE]={0};
            // itoa(msg.size(),buf,10);
            // msg=buf+msg;
            // send(clnt.sock,msg.c_str(),msg.size(),0);
            // //�굵 ���⼭ ��� ��ų�� �ʿ��ҵ�
            // //�׳� ������ ����
            // return true;
        }
        else if(posforF==friendNum+3)
        {
            //����
            std::cout<<"����˴ϴ�. \n";
            clnt.~ClientBase();
            break;
        }
    }
    return false;
}
//ģ�� �߰� �Ǵ� ã��
void ClientHandle::AddorFindMyFriend(SOCKET& sock,std::string AorF)
{ //--�г��� �Է�-���� �۽�-����-ģ�� ��� ������Ʈ
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;

    std::string friendNick;
    std::cout<<"==================================================\n";
    std::cout<<"ģ���� �г����� �Է��ϼ��� >";
    std::getline(std::cin,friendNick);
    while(friendNick==nickName) //���� �г��� �Է��������...�̷��ͱ��� �����ؾ���?
    {
        std::cout<<"���� �г����Դϴ�. �ٽ� �Է��ϼ��� >";
        std::getline(std::cin,friendNick);
    }
    msg=":"+AorF+":"+friendNick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(sock,msg.c_str(),msg.size(),0);
}
//ã�� ģ�� ��� ���
void ClientHandle::PrintFoundFriend(std::vector<std::string> flist)
{
    std::cout<<"==================================================\n";
    if(flist[2]=="none")
    {
        std::cout<<"�ش��ϴ� ģ���� �����ϴ�. \n";
        return;
    }
    for(int i=0;i<atoi(flist[2].c_str())*2;i+=2)
    {
        std::cout<<flist[3+i]<<"\t";
        if(flist[4+i]=="F")
        {
            std::cout<<"\x1b[31m��\x1b[m\n";
        }
        else
        {
            std::cout<<"\x1b[32m��\x1b[m\n";
        }
    }
    Sleep(1000);
}
//ģ�� ���� --posforF�� ģ�� �ε��� ���� �� ����
bool ClientHandle::ConnectMyFriend(SOCKET& sock)
{
    //������ ������ posforF�� ģ�� ����Ʈ �ε��� �ѹ�
    //�ӼӸ� Ȥ�� ä�ù� ���� �����ϰ� �ؾ���
    std::cout<<"==================================================\n";
    std::cout<<"�ӼӸ� ������(W) 1:1 ä�ù� ����(C) ��������(X)>\n";
    bool quit=false;
    int input;
    while(!quit)
    {
        input=getch();
        switch (input)
        {
        case 119: //�ӼӸ� ������
        {
            std::cout<<"���� �޽��� >";
            std::string msg;
            std::getline(std::cin,msg);
            //�Է¹��� �޽��� ������ �����ؾ��Ѵ�.
            //���� ����) �޽��� ��ü ����:Whisper:�� �г���:ģ�� �ε���:�޽���
            char buf[BUF_SIZE]; //���� ���ڿ��� �ٲ� ����
            itoa(posforF,buf,10);
            msg=":Whisper:"+nickName+":"+buf+":"+msg;
            itoa(msg.size(),buf,10);
            msg=buf+msg;
            send(sock,msg.c_str(),msg.size(),0);
            std::cout<<"==================================================\n"
                        "�ӼӸ� ������(W) 1:1 ä�ù� ����(C) ��������(X)>\n";
        }
            break;
        case 99: //1:1 ä�ù� ����
        {
            //���⼭ ä�ù� �ٷ�� �Լ��� �Ѱܾ� �� ��.
            //�켱 1:1ä�ù濡 �����Ѵٴ°� ������ �˷�����
            //�ִ� ���̸� ���� �濡 ���¹��̸� ���� ������ ������
            //�׷��� ���� �޽�����
            //�޽����ѱ���:Enter:OtO:���г���:ģ���ε���
            char buf[BUF_SIZE]; //���� ���ڿ��� �ٲ� ����
            itoa(posforF,buf,10);
            std::string msg=":Enter:OtO:"+nickName+":"+buf;
            itoa(msg.size(),buf,10);
            msg=buf+msg;
            send(sock,msg.c_str(),msg.size(),0);
            //���⼭ ��� �ǰ� �ϰų�
            return true;
        }
            quit=true; //������ ���ݴµ� ���� �־���ҵ�
            break;
        case 120: //��������
            quit=true;
            break;
        default:
            break;
        }
    }
    return false;
    //119 ,99 ,120
}
//ä�ù� ����
bool ClientHandle::EnterChatRoom(SOCKET& sock)
{
    chooseFriend=true;
    Cflist.clear();
    // std::vector<int> flist;
    // std::cout<<"()"
    // while(true)
    // {
    //     std::string friendNick;
    //     std::cout<<"�Բ��� ģ���� �г����� �Է��ϼ��� >";
    //     std::getline(std::cin,friendNick);
    // }
    if(friendNum==0) return false;
    posforF=0;
    while(true)
    {
        int input;
        bool quit=false;
        RequestFriendList(sock);
        while(!quit)
        {
            input=getch();
            switch (input)
            {
            case 72:  
            case 119:
                if(posforF!=0) 
                {
                    posforF--;
                    RequestFriendList(sock);
                }
                break;
            case 80: 
            case 115:
                if(posforF!=friendNum-1)
                {
                    posforF++;
                    RequestFriendList(sock);
                }
                break;
            case 13:
                quit=true;
                break;
            case 120:
            {
                chooseFriend=false;
                if(Cflist.size()==0) return false;
                char buf[BUF_SIZE]={0};
                std::string msg=":Enter:OtM:"+nickName;
                for(int index: Cflist)
                {
                    itoa(index,buf,10);
                    msg.append(":");
                    msg.append(buf);
                    memset(buf,0,BUF_SIZE);
                }
                itoa(msg.size(),buf,10);
                msg=buf+msg;
                send(sock,msg.c_str(),msg.size(),0);
                Cflist.clear();
                return true;
            }
                break;
            default:
                continue;
            }
        }
        if(Cflist.size()!=0) //�ߺ����� �˻�
        {
            for(int index : Cflist)
            {
                if(index==posforF) continue;
            }
        }
        Cflist.push_back(posforF);
    }
    Cflist.clear();
    chooseFriend=false;
    return false;
}