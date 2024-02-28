//FTP ����
#include <fstream>
#include <process.h>
#include <vector>

#include "serverbase.h"

const char* FTP_PORT_NUM="90320";
const unsigned int BUF_SIZE=10000;

//Ŭ���̾�Ʈ ��û ó��
unsigned WINAPI HandlingClient(void* arg); 
//���ڿ� ���ø�(split)
std::vector<std::string> split(std::string,const char);
//���� ����
void SaveFile(SOCKET&,int);
//���� ����
void SendFile(SOCKET&,std::string);

struct FileInfo //���� ���� ���� ����ü
{
    std::string name;
    std::string path;
};
std::vector<FileInfo> fileList;

int main() 
{
    ServerBase serv(FTP_PORT_NUM);
    serv.openServer();

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
    //���� ���ſ� ���
    // int fSize;
    // std::cout<<"�ᱹ �״µ� ���� ���� �Ǵ�... \n"; //��
    //�׷��� ���� �ؾ��Ұ� Ŭ���̾�Ʈ ��û���� �غ�
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
        if(msg=="File")
        {
            //���� ���ε� ��û --���� ������ �޴´�
            // fSize=atoi(split(bufString,':')[2].c_str());
            SaveFile(clntSock,atoi(split(bufString,':')[2].c_str()));
        }
        else if(msg=="Download")
        {
            SendFile(clntSock,split(bufString,':')[2]);
        }
    }
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
//���� ����
void SaveFile(SOCKET& sock,int size)
{
    std::string data, name, ext;
    char buffer[BUF_SIZE]={0};  
    send(sock,"y",2,0); //Ȯ�� �޽��� ����

    char* fptr=new char[size];
    // while(recv(sock,buffer,BUF_SIZE,0)) //������ ��� ����������
    // {
    //     data.append(buffer);
    //     if(data.size()>=size) break;
    //     memset(buffer,0,BUF_SIZE);
    // } //�� ���̳� ������ �˸� �����ͷ� �ѹ��� ���� �� �ִ�! ���Ӱ� �ϳ��� ��
    recv(sock,fptr,size,0);
    send(sock,"f",2,0); //Ȯ�� �޽��� ����
    memset(buffer,0,BUF_SIZE);

    std::string bufString;
    int recvLen, strLen;
    while(recv(sock,buffer,BUF_SIZE,0)) //�����̸� ���۹ޱ� --�̰Ŷ� ��ζ� �����ؾ���
    {
        bufString=buffer;
        recvLen=bufString.size();
        strLen=atoi(buffer)+bufString.substr(0,bufString.find(':')).size();
        while(recvLen!=strLen) //���̰� ��ġ���� ������ �� �б�
        {
            memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
            if(recv(sock,buffer,BUF_SIZE,0)<=0) return; //���ſ������� ������ ����
            bufString.append(buffer); recvLen=bufString.size();
        }
        if(recvLen==strLen) break;
        memset(buffer,0,BUF_SIZE); //���� �ʱ�ȭ
    }
    send(sock,"y",2,0);
    name=split(bufString,':')[1];
    ext=split(bufString,':')[2];
    //���� �̸� �ߺ��˻�� �ؾ��ҵ�


    //���� �����ؾ���
    std::string path="./downloads/"+name+"."+ext;
    std::cout<<path<<std::endl;
    std::ofstream wfile(path,std::ios::out|std::ios::binary);
    // wfile.write(data.c_str(),size);
    wfile.write(fptr,size);
    wfile.close();

    //���� ����ü �迭�� ���
    FileInfo nfile;
    nfile.name=name;
    nfile.path=path;
    fileList.push_back(nfile);
    std::cout<<name<<"�� ���ε� �Ǿ����ϴ�. \n";
    delete[] fptr;
}
//���� ����
void SendFile(SOCKET& sock,std::string fName)
{
    std::string path;
    for(auto f:fileList)
    {
        if(f.name==fName)
        {
            path=f.path;
        }
    }
    std::ifstream rfile(path,std::ios::in|std::ios::binary); //�б�, ���̳ʸ�����
    if(!rfile.is_open()) //���� ���� ����
    {
        std::cout<<"���� ���� ����! \n";
        return;
    }
    rfile.seekg(0,std::ios::end);
    int fSize=rfile.tellg();
    rfile.seekg(0,std::ios::beg);
    //���� �б�
    char* fptr=new char[fSize]; //�迭 ����
    rfile.read(fptr,fSize);
    rfile.close();

    char buffer[BUF_SIZE]={0};
    itoa(fSize,buffer,10);
    std::string bufString=buffer;
    send(sock,buffer,bufString.size(),0);
    recv(sock,buffer,BUF_SIZE,0);

    send(sock,fptr,fSize,0);
    recv(sock,buffer,BUF_SIZE,0);

    bufString=split(path,'.')[split(path,'.').size()-1];
    send(sock,bufString.c_str(),bufString.size(),0);
    recv(sock,buffer,BUF_SIZE,0); //Ȯ�� �޽��� �ް� ������ ����

    std::cout<<fName<<" ���� �Ϸ� \n";
    delete[] fptr;
}