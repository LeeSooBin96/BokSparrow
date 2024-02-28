//�����зο� ����(����)
#include <process.h>
#include <vector>

#include "serverbase.h"
#include "userhandle.h"
#include "chatroom.h"

const char* PORT_NUM="91016";
// const char* FTP_PORT_NUM="90320";
const unsigned int BUF_SIZE=1024;
//Ŭ���̾�Ʈ ��û ó��
unsigned WINAPI HandlingClient(void* arg); 
//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//���� ���� ���� ó�� Ŭ����
UserHandle hUser; 
//ä�� ���� ó�� Ŭ����
ChatRoom hChat;


int main()
{
    //���� �ۼ��� ���� �����ϱ� ���� �ڽ� ���μ��� ����
    // fork(); //��� �ڽ� ���μ����� �����ؾ��ұ�~ �� �̰� ������������...��??
    // CreateProcess( )//�� �̰� ����غ����� ���µ�;; ������ ���������� �ʿ�..��..
    //�׷� �׳� �ҽ����� �ϳ��� �����Ű�°Ŷ� ���� �ٸ���.......................
    //�׷� �ƿ� ���ʿ� ��� 2�� �ȵǰڳ� 
    /*��� 1. ó������ �ڽ����μ��� �����ؼ� FTP ���� �ѵα�
      ��� 2. ���� �ۼ��� ���� ��û�� ������ ���� �Ѽ� ó���ϰ� ���μ��� �Ҹ�
      ��� 1�� �ڽ� ���μ��� �ϳ��� �ǰ� �� �ڽ����μ����� �뷮�� ������?
      ��� 2�� �ڽ� ���μ����� ��û ���ö����� ��������? ���� ���ϵ� �ݾ�����ϰ�?
               ������ �ڽ� ���μ��� �� �Ҹ�ǰ� �ִ��� ���� ���μ��� ������ Ȯ���ؾ���
      �ƹ����� 1�� ������...�غ���*/
    //�켱�� ���� ����
    ServerBase serv(PORT_NUM);
    // ServerBase FTPserv(FTP_PORT_NUM);
    serv.openServer();
    // FTPserv.openServer();
    //Ŭ���̾�Ʈ ��û ����
    SOCKET clntSock;
    SOCKADDR_IN clntAdr;
    int clntAdrSz=sizeof(clntAdr);

    while(true)
    { //Ŭ���̾�Ʈ ��� �޾ƾ���
        clntSock=accept(serv.servSock,(SOCKADDR*)&clntAdr,&clntAdrSz);
        std::cout<<"������ ������: "<<inet_ntoa(clntAdr.sin_addr)<<std::endl;
      //���� Ŭ���̾�Ʈ�� �ޱ� ���� ��û�� ó���� ������� ���� ����
        HANDLE hThread; //������ �ڵ�
        hThread=(HANDLE)_beginthreadex(NULL,0,HandlingClient,(void*)&clntSock,0,NULL);
    }
    return 0;
}
//Ŭ���̾�Ʈ ��û ó��
unsigned WINAPI HandlingClient(void* arg)
{
    SOCKET clntSock=*((SOCKET*)arg);
    //�޽��� ���Ž� ����ϴ� �͵�
    char buffer[BUF_SIZE]={0};
    std::string bufString, nickname;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����

    while(recv(clntSock,buffer,BUF_SIZE,0)>0) //���Ż��� ����
    { //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clntSock,buffer,BUF_SIZE,0)<=0) return 0; //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[1];
        if(msg=="Nick")
        {
            hUser.UserLogin(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="FriendList")
        {
            nickname=split(bufString,':')[2];
            hUser.SendFriendList(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Add")
        {
            //ģ�� �߰�
            hUser.AddFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Find")
        {
            //ģ�� ã��
            hUser.FindFriend(clntSock,split(bufString,':')[2]);
        }
        else if(msg=="Whisper")
        {
            //�ӼӸ� ������
            hUser.sendWhisper(clntSock,split(bufString,':'));
        }
        // else if(msg=="Clist")
        // {
        //     //�г����� ���Ե� ��� ä�ù��ڵ� ����Ʈ�� ��������
        //     hChat.ShowChatRoomList(clntSock,split(bufString,':')[2]);
        // }
        else if(msg=="Enter")
        {
            //ä�ù� ����
            std::vector<std::string> mlist; //��� ����Ʈ ������ ����
            mlist.push_back(split(bufString,':')[3]);
            if(split(bufString,':')[2]=="OtO") //1:1 ä�ù�
            {
            //���� �޽���)�޽����ѱ���:Enter:OtO:�� �г���:ģ�� �ε���
                //ä�ù� ���� --ä�ù� �ڵ� �۽��ؾ���
                mlist.push_back(hUser.BringMyFriend(split(bufString,':')[3],atoi(split(bufString,':')[4].c_str())));
                // hChat.EnterOtOchatRoom(clntSock,{split(bufString,':')[3],hUser.BringMyFriend(split(bufString,':')[3],atoi(split(bufString,':')[4].c_str()))});
            }
            else if(split(bufString,':')[2]=="OtM") //��Ƽ ä�ù�
            {
            //���� �޽���)�޽����ѱ���:Enter:OtO:�� �г���:ģ���г���1:ģ���г���2...
                //1:N �϶��� �г��� ���޹޾Ƽ� �������ϰ����� --�ε����� ����
                for(int i=4;i<split(bufString,':').size();i++)
                {
                    // mlist.push_back(split(bufString,':')[i]);
                    mlist.push_back(hUser.BringMyFriend(split(bufString,':')[3],atoi(split(bufString,':')[i].c_str())));
                }
            }
            std::string code = hChat.EnterChatRoom(clntSock,mlist);
            std::string notic=split(bufString,':')[3]+"���� �����ϼ̽��ϴ�. ";
            //�׷��� �̽������� ä�ù��� �ٸ� ����鿡�� �� �����Ѵٴ� �޽��� ���������
            hUser.SendNoticMSG(code,mlist,notic);
        }
        else if(msg=="Chat")
        {
            //ä�� ���� ó���� ��� ���⼭ ����
            std::string sep=split(bufString,':')[2];
            if(sep=="Connect")
            {
                //ä�ù� ��� ���ӻ��� ��û
                hChat.SendMemConnect(clntSock,split(bufString,':')[3],split(bufString,':')[4]);
                //�̶� ä�ù� ��������� ���������� �˷�����...
                //����鿡�� �����ѻ�� �г����� �˷����� --����
                hUser.SendEnterMem(split(bufString,':')[3],hChat.BringMemList(split(bufString,':')[3]),split(bufString,':')[4]);
            }
            else if(sep=="Send")
            {
                //ä�� �޽��� ����
                //�Ϸ��� �ش� ä�ù� ��� ����Ʈ�� �����ͼ�
                //����� �ش��ϴ� ���Ͽ� �޽��� ��������
                hUser.SendChatMSG(split(bufString,':')[3],hChat.BringMemList(split(bufString,':')[3]),split(bufString,':')[4],split(bufString,':')[5]);
            }
            else if(sep=="Quit")
            {
                //ä�ù� ����
                std::string notic=split(bufString,':')[4]+"���� �����ϼ̽��ϴ�. :Q:"+split(bufString,':')[4];
                hUser.SendNoticMSG(split(bufString,':')[3],hChat.BringMemList(split(bufString,':')[3]),notic);
                //���� ���� ����
                hChat.QuitChatRoom(split(bufString,':')[3],split(bufString,':')[4]);
                
                //ä�� ���� ������ ���� ���Ѿ���
                char buf[BUF_SIZE]={0};
                std::string quitM=":"+split(bufString,':')[3]+":quit";
                itoa(quitM.size(),buf,10);
                quitM=buf+quitM;
                send(clntSock,quitM.c_str(),quitM.size(),0);
            }
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    std::cout<<"������ ����Ǿ����ϴ� : "<<nickname<<std::endl;
    //���� ���� �� ���� ���� ���� ��ȭ
    hUser.UserLogout(nickname);
    return 0;
}
std::vector<std::string> split(std::string string,const char D)
{ //�̰� �ϼ���Ű�� �ʹ� --�ϼ�!
    std::vector<std::string> result;
    while(true) //ã�� ��ġ�� ���ڿ� ũ�⺸�� ���� ����
    {
        result.push_back(string.substr(0,string.find(D)));
        if(string.find(D)>string.size()) break; //���⿡ �־�� �������͵� ����
        string.erase(0,string.find(D)+1); //���� ���ڿ� �����
    }
    return result;
}
