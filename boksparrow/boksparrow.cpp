//�����зο� Ŭ���̾�Ʈ
#include <conio.h>
#include <fstream>
#include <process.h>
#include <vector>
#include <string>

#include "clientbase.h"
#include "clienthandle.h"
#include "chathandle.h" 

const char* SERVER_IP="127.0.0.1";
const char* PORT_NUM="91016";
const char* FTP_PORT_NUM="90320";
const unsigned int BUF_SIZE=1024;

//������ ����
ClientBase clnt(SERVER_IP,PORT_NUM); //�̰� �������� ����?
//Ŭ���̾�Ʈ ���� ���� Ŭ����
ClientHandle hClient;
//Ŭ���̾�Ʈ ä�� ���� Ŭ����
ChatHandle hChat;

//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//�޽��� ���� ������
unsigned WINAPI ReceiveMSG(void* arg);
//ä�� �۽� ������
unsigned WINAPI InputChat(void* arg);
//ä�� ���� ������
unsigned WINAPI OuputChat(void* arg);
//���� ���ε� ������
unsigned WINAPI UploadFile(void* arg);
//���� �ٿ�ε� ������
unsigned WINAPI DownloadFile(void* arg);



int main()
{
    //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string msg;
    //�α��� --�г��� ���
    system("cls");
    std::ifstream rFile("./img/logo.txt");
    rFile.seekg(0,std::ios::end);
    int fSize=rFile.tellg();
    rFile.seekg(0,std::ios::beg);
    char* fptr=new char[fSize];
    rFile.read(fptr,fSize);
    std::cout<<fptr<<std::endl;
    while(!hClient.LoginProcess(clnt.sock)); 
    while(true) //���⸦ ����
    {
        //���� ������ ����
        HANDLE hRecvThread; 
        hRecvThread=(HANDLE)_beginthreadex(NULL,0,ReceiveMSG,nullptr,0,NULL);
        //ģ�� ��� â
        if(hClient.ProcessFriendScreen(clnt)) //�̰� �׳� �������� ä������ �����ؾ���
        {
            //������ ���� ���
            WaitForSingleObject(hRecvThread,INFINITE);

            //ä������ �ѱ�°� ���⼭ ����
            HANDLE hCthread[2];
            hCthread[1]=(HANDLE)_beginthreadex(NULL,0,OuputChat,nullptr,0,NULL);
            hCthread[0]=(HANDLE)_beginthreadex(NULL,0,InputChat,nullptr,0,NULL);
            WaitForMultipleObjects(2,hCthread,TRUE,INFINITE);
        }
        else break;
    }
    return 0;
}
std::vector<std::string> split(std::string string,const char D)
{ 
    std::vector<std::string> result;
    while(true) //ã�� ��ġ�� ���ڿ� ũ�⺸�� ���� ����
    {
        result.push_back(string.substr(0,string.find(D)));
        if(string.find(D)>string.size()) break; //���⿡ �־�� �������͵� ����
        string.erase(0,string.find(D)+1); //���� ���ڿ� �����
    }
    return result;
}
//�޽��� ���� ������ --�� ���۾�����?
unsigned WINAPI ReceiveMSG(void* arg)
{
    // //���۷� ����� ���ڿ�
    char buffer[BUF_SIZE]={0}; 
    std::string bufString;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����

    while(recvLen=recv(clnt.sock,buffer,BUF_SIZE,0)>0)
    {
        //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) hClient.ReceiveError(); //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        // std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[1];
        if(msg=="friend")
        {
            //ģ�� ��� ���
            hClient.PrintFriendList(split(bufString,':'));
        }
        else if(msg=="add")
        {
            //ģ�� �߰� ���
            if(!strncmp(split(bufString,':')[2].c_str(),"Failed",7))
            {
                std::cout<<"�������� �ʴ� �����̰ų� �̹� �߰��� �����Դϴ�. \n";
            }
            else if(!strncmp(split(bufString,':')[2].c_str(),"Success",8))
            {
                std::cout<<"���������� �߰��Ǿ����ϴ�. \n";
            }
            Sleep(1000);
        }
        else if(msg=="find")
        {
            //ģ�� ã�� ���
            hClient.PrintFoundFriend(split(bufString,':'));
        }
        else if(msg=="whisper")
        {
            //�ӼӸ� ����
            std::cout<<"==================================================\n";
            std::cout<<split(bufString,':')[2]<<"�����κ���: "<<split(bufString,':')[3]<<std::endl;
            std::cout<<"==================================================\n";
            Sleep(1000);
        }
        // else if(msg=="clist")
        // {
        //     //��������) �޽����ѱ���:clist:ä�ù氳��:ä�ù��̸�:ä�ù��ڵ�...
        //     //ä�ù� ��� ����ȭ������ ����~
        //     //�Լ��� ����Ǿ��� ���⼭ ä�ù� ���� Ȥ�� �������ΰ��⸦ ���ٵ�
        //     //�������� ���⸦ �ϸ� �Լ� �ٽ� ȣ��
        //     //ä�ù� ���� �����ϸ� �� �����嵵 �����Ű�� ��
        //     //�غ���
        //     hChat.ChooseChatRoom(split(bufString,':'));
        // }
        else if(msg=="enter")
        {
            //ä�ù� ����) �޽��� �ѱ���:enter:ä���ڵ�:���г���(1:1)
            //ä�ù� �����ϸ� �� ������� ����Ǿ���� --���ص���!!!
            //ä�� �ʱ� ���� --1:N�� ����� �ð�
            hChat.SettingChat(hClient.nickName,split(bufString,':'));

            break;
        }

        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    //���� ����
    if(recvLen<0) hClient.ReceiveError();
}
//ä�� �۽� ������ --�޽��� �Է� �κ� (�� ä�ù� ���� �����)
unsigned WINAPI InputChat(void* arg)
{ //������ �޽��� ��û�� �Ծ� : �޽��� �ѱ���:Chat:��û�޽���:ä�ù��ڵ�:��û�ڴг���
    
    //����� ����
    char buffer[BUF_SIZE];
    std::string msg;
    //ä�� ��� ���� ���� Ȯ���ؾ���
    msg=":Chat:Connect:"+hChat.code+":"+hChat.myNick;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(clnt.sock,msg.c_str(),msg.size(),0);
    
    //ä�� �Է� ���� �κ�
    std::string chat;
    while(true)
    {
        memset(buffer,0,BUF_SIZE);
        std::getline(std::cin,chat);
        //�Է��� �޽��� ä�� ������
        //������ ���� �޽���)�޽����ѱ���:Chat:Send:ä���ڵ�:�г���:�޽���
        if(chat=="/Q"||chat=="/q") 
        {
            //�������� ����޽��� )�޽����ѱ���:Chat:Quit:ä���ڵ�:�г���
            msg=":Chat:Quit:"+hChat.code+":"+hClient.nickName;
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
            break;
        }
        else if(chat=="/P"||chat=="/p")
        {
            std::string fPath, fName; //���� ���, ���� �̸� ����
            std::cout<<"==================================================\n";
            std::cout<<"���� ��θ� �Է��ϼ��� >";
            std::getline(std::cin,fPath);
            std::cout<<"���ϸ��� �Է��ϼ��� >";
            std::getline(std::cin,fName);
            //ä�� ������ ���ϸ� ���ε� ��� �˸��� --ä�ü����� �޽����� ���Ŵϱ� ä�� �޽��� ������ �ϸ� �ȴ�.
            msg=":Chat:Send:"+hChat.code+":"+hClient.nickName+":"+fName+"�� ���ε��Ͽ����ϴ�.";
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
            //�׸��� ������� ���� ftp������ ���� �����ؾ���
            std::string f[2]={fPath,fName};
            HANDLE hUthread;
            hUthread=(HANDLE)_beginthreadex(NULL,0,UploadFile,(void*)&f,0,NULL);
            // std::cout<<"���� ��������...\n";
            //������ ���� �������� �˸��� ���� ������ FTPŬ���̾�Ʈ �ѱ�
            //���� �ڽ� ���μ��� ������ ������� �ȵȴٰ� �ϴϱ�... ���⼭ �ٷ� ���� ����
            // ClientBase ftpClient(SERVER_IP,FTP_PORT_NUM); //ftp ���� ����
            //���⼭ �۵��� �Լ� �ʿ��ϴ� --���� ���ε�� ä���̶� ���� �����ؾ��ϴ� 
            //������ �ϳ� �����ؼ� �ű⼭ ������ ���� ���� ���ε� �ϰ� ����
            //���� �����ϰ� �����ϴ� �κи� �������
        }
        else if(chat.substr(0,2)=="/D"||chat.substr(0,2)=="/d")
        {
            //�̸� �޾ƿ��ϱ� �װ� ������ ������ �ش��ϴ� ���� �޵���
            std::string fName=chat.substr(3,chat.size()-3);
            //����� ���ϸ� �ٷ� ������ �Ǵϱ� �ٷ� �������
            HANDLE hDthread;
            hDthread=(HANDLE)_beginthreadex(NULL,0,DownloadFile,(void*)&fName,0,NULL);
            //������ ���� ������ �˸��� ���� ������ FTPŬ���̾�Ʈ �ѱ�
        }
        else if(!chat.empty())
        {
            msg=":Chat:Send:"+hChat.code+":"+hClient.nickName+":"+chat;
            itoa(msg.size(),buffer,10);
            msg=buffer+msg;
            send(clnt.sock,msg.c_str(),msg.size(),0);
        }
        else hChat.PrintChatScreen();
    }


    return 0;
}
//ä�� ���� ������ --�޽��� ��� �κ�
unsigned WINAPI OuputChat(void* arg)
{
    /*���� �������� ä�� �ڵ� �Բ� ������� ���� �� ä�ù��� �޽����� ���� �� �ִ�.*/
    //�޽��� ���Ž� ����ϴ� �͵�
    char buffer[BUF_SIZE]={0};
    std::string bufString, nickname;
    int recvLen, strLen; //���Ÿ޽��� ����, ���ڿ� ����
    while(recvLen=recv(clnt.sock,buffer,BUF_SIZE,0)>0) //���Ż��� ����
    { //�޽�������+�޽������� ���� ����
        bufString=buffer;
        recvLen=bufString.size(); //���ŵ� ���ڿ� ���� ����
        //���ŵ� ���ڿ� ���� ����
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(clnt.sock,buffer,BUF_SIZE,0)<=0) return 0; //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        // std::cout<<"���޵� �޽���: "<<bufString<<std::endl; //�ӽ�
        std::string msg=split(bufString,':')[2];
        if(split(bufString,':')[1]!=hChat.code)
        {
            //�ٸ��� �޽��� ������ ȭ�� �������
            hChat.PrintChatScreen();
            // continue; //���� �ִ� ä�ù��� �ƴϸ�
        }
        else if(msg=="connect")
        {
            //ä�ù� ��� ���ӻ��� ����
            hChat.UpdateMemState(split(bufString,':'));
        }
        else if(msg=="enterM")
        {
            //���� ��� �г��� ���� -->���� �����ؾ���
            hChat.UpdateEnterMem(split(bufString,':')[3]);
        }
        else if(msg=="notic")
        {
            //���� �޽��� ���� --���⼭ �����϶� ���� ���� �־�߰ڴ�~
            if(split(bufString,':')[4]=="Q") hChat.UpdateQuitMem(split(bufString,':')[5]);
            hChat.ReceiveNotic(split(bufString,':')[3]);
        }
        else if(msg=="chat")
        {
            //ä�� �޽��� ����
            hChat.ReceiveChat(split(bufString,':')[3],split(bufString,':')[4]);
        }
        else if(msg=="quit") break; //���� ������ ����


        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    if(recvLen<0) hClient.ReceiveError();
    return 0;
}
//���� ���ε� ������
unsigned WINAPI UploadFile(void* arg)
{
    //���ϰ�� ���� Ȯ���غ��� --�� ���޵�
    std::string fPath=*((std::string*)arg);
    std::string fName=*((std::string*)arg+1);
    // Sleep(1000); //������ �۵� Ȯ�ο� �ӽ�
    //���� �ѹ� �����
    //1. ���� ��, ���� ��� �Է� --���� ������ ���� --�Ϸ�
    //2. chat������ ���� ���ε� ��� ����(���ϸ�) --�̰͵� ���� ������ --�Ϸ�
    //3. ftp������ ���� ���� --�̰͸� ���⼭
    //�ϴ� ���� ����
    std::ifstream rfile(fPath,std::ios::in|std::ios::binary); //�б�, ���̳ʸ�����
    if(!rfile.is_open()) //���� ���� ����
    {
        std::cout<<"���� ���� ����! \n";
        return 0;
    }
    //���� ��ü ������ �˾ƾ��ϰ�? --������� ��������
    // int fSize; //���� ũ��
    rfile.seekg(0,std::ios::end);
    int fSize=rfile.tellg();
    rfile.seekg(0,std::ios::beg);
    //���� �б�
    char* fptr=new char[fSize]; //�迭 ����
    rfile.read(fptr,fSize);
    rfile.close();

    ClientBase ftpClient(SERVER_IP,FTP_PORT_NUM); //ftp ���� ����
    //���� �޽���) �޽����ѱ���:File:���ϻ�����
    std::string msg=":File:";
    char buf[BUF_SIZE]={0};
    itoa(fSize,buf,10); msg.append(buf);
    memset(buf,0,BUF_SIZE);
    itoa(msg.size(),buf,10);
    msg=buf+msg;
    send(ftpClient.sock,msg.c_str(),msg.size(),0); //���� ������ ����

    if(recv(ftpClient.sock,buf,BUF_SIZE,0)<0) return 0; //���� �������� ����
    // for(int i=0;i<fSize;i++) std::cout<<*(fptr+i);
    send(ftpClient.sock,fptr,fSize,0); //���� ������ ����
    recv(ftpClient.sock,buf,BUF_SIZE,0); 

    msg=":"+fName+":"+split(fPath,'.')[split(fPath,'.').size()-1]; //���� �̸��̶� Ȯ���� ����
    memset(buf,0,BUF_SIZE);
    itoa(msg.size(),buf,10);
    msg=buf+msg;
    send(ftpClient.sock,msg.c_str(),msg.size(),0);
    // std::cout<<std::endl;
    //���� �Ϸ�(�Ϸ�޽��� ����)�Ǹ� ������ ����
    recv(ftpClient.sock,buf,BUF_SIZE,0); 
    delete[] fptr;
    std::cout<<fName<<"�� ���ε尡 �Ϸ�Ǿ����ϴ�. \n";
}
//���� �ٿ�ε� ������
unsigned WINAPI DownloadFile(void* arg)
{
    //����
    //1. �ٿ��� ���ϸ� �Է�
    std::string fName=*((std::string*)arg);
    Sleep(1000); //������ �۵� Ȯ�ο� �ӽ�
    std::cout<<fName<<std::endl;
    //2. �ٿ�ε� ���� ���� --������ �ڵ����� �Ƿ���--�ȵǳ� --�� ����.. ���̺귯�� ���� �ȵǴϱ� �����..
    // const wchar_t* dir=new const wchar_t("./downloads");
    // if(_waccess(dir,0)<0)
    //     CreateDirectory(dir,NULL);
    //3. ftp���� ����
    ClientBase ftpClient(SERVER_IP,FTP_PORT_NUM); //ftp ���� ����
    //4. ���ϸ� �۽�
    char buffer[BUF_SIZE]={0};
    std::string msg=":Download:"+fName;
    itoa(msg.size(),buffer,10);
    msg=buffer+msg;
    send(ftpClient.sock,msg.c_str(),msg.size(),0); //���� ��û
    //5. ������ ����
    memset(buffer,0,BUF_SIZE);
    recv(ftpClient.sock,buffer,BUF_SIZE,0); //���� ũ�� ����
    int size=atoi(buffer);
    send(ftpClient.sock,"y",2,0); //Ȯ�θ޽���

    //6. ���� ���� �� ������ ���
    char* fptr=new char[size];
    recv(ftpClient.sock,fptr,size,0); //���� ������ ����
    send(ftpClient.sock,"y",2,0);
    
    memset(buffer,0,BUF_SIZE);
    recv(ftpClient.sock,buffer,BUF_SIZE,0); //���� Ȯ����
    send(ftpClient.sock,"y",2,0);
    std::string ext=buffer;

    std::ofstream wfile("./downloads/"+fName+"."+ext,std::ios::out|std::ios::binary);
    wfile.write(fptr,size);
    wfile.close();
    std::cout<<fName<<"�� �ٿ�ε尡 �Ϸ�Ǿ����ϴ�. \n";
    delete[] fptr;
}