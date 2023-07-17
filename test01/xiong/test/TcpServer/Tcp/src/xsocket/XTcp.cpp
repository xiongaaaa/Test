#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <Windows.h>
#define socklen_t int
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#define closesocket close
#endif

#include <thread>
#include <iostream>
using namespace std;

#include "XTcp.h"

XTcp::XTcp():sock(0),port(0),ip("")
{
#ifdef WIN32
    static bool first = true;
    if(first)
    {
        first = false;
        WSADATA ws;
        WSAStartup(MAKEWORD(2,2),&ws);
    }

#endif
}

XTcp::~XTcp()
{

}

int XTcp::CreateSocket()
{
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock == -1)
    {
        printf("create sock failed!!!\n");
    }
    return sock;
}

bool XTcp::Bind(unsigned short port)
{
    if(sock <= 0)
        CreateSocket();
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(::bind(sock,(struct sockaddr*)&saddr,sizeof(saddr)) !=0)
    {
        printf("bind failed...\n");
        return false;
    }
    printf("bind %d success!\n", port);

    listen(sock,10);

    return true;
}

XTcp XTcp::Accept()
{
    XTcp tcp;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd = accept(sock,(struct sockaddr*)&cliaddr,&len);
    if(cfd <= 0)
        return tcp;
    printf("accept client %d\n",cfd);
    char* ip = inet_ntoa(cliaddr.sin_addr);
    strcpy(tcp.ip, ip);
    tcp.port = ntohs(cliaddr.sin_port);
    tcp.sock = cfd;
    printf("recv client ip is %s,port is %d\n",tcp.ip,tcp.port);
    
    return tcp;
}

void XTcp::Close()
{
    if(sock <= 0)
        return;
    closesocket(sock);
}
 
int XTcp::Recv(char* buf,int bufSize)
{
    return recv(sock,buf,bufSize,0);
}

int XTcp::Send(const char* buf,int bufSize)
{
    int s = 0;
    while(s != bufSize)
    {
        int len = send(sock,buf+s,bufSize-s ,0);
        if(len <= 0)
            break;
        s += len;
    }
    return s;
}


bool XTcp::Connect(const char* ip, unsigned short port,int timeoutms)
{
    if (sock <= 0)
       CreateSocket();
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
#ifdef WIN32
    saddr.sin_addr.s_addr = inet_addr(ip);
#else
    inet_pton(AF_INET,ip,&saddr.sin_addr.s_addr);
#endif

    SetBlock(false);
    
    fd_set set;

    if(connect(sock, (struct sockaddr*)&saddr, sizeof(saddr)) !=0)
    {
        FD_ZERO(&set);
        FD_SET(sock,&set);

        struct timeval tm;
        tm.tv_sec = 0;
        tm.tv_usec = timeoutms * 1000;
        if(select(sock+1,0,&set,0,&tm) <=0)
        {
            printf("connect timeout or error!\n");
            printf("connect %s:%d failed!:%s\n",ip,port,strerror(errno));
            return false;
        }         
    }

    SetBlock(true);
    printf("connect %s:%d success!:s\n",ip,port);
    return true;
}

bool XTcp::SetBlock(bool isblock)
{
    if(sock <= 0)
        return false;
    unsigned long ul = 0;
#ifdef WIN32
    if(!isblock)
        ul = 1;
    ioctlsocket(sock,FIONBIO,&ul);
#else
    if(!isblock)
        ul = 1;
    ioctl(sock,FIONBIO,&ul);
#endif

    return true;
}
