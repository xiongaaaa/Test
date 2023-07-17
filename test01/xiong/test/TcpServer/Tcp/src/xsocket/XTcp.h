#ifndef XTCP_H
#define XTCP_H

#ifdef WIN32
#ifdef XSOCKET_EXPORTS
#define XSOCKET_API __declspec(dllexport)
#else
#define XSOCKET_API __declspec(dllimport)
#endif
#else
#define XSOCKET_API
#endif

#include <string>

class XSOCKET_API XTcp
{
public:
    XTcp();
    virtual ~XTcp();

    int  CreateSocket();
    bool Bind(unsigned short port);
    XTcp Accept();
    void Close(); 
    int  Recv(char* buf,int bufSize);
    int  Send(const char* buf,int bufSize);
    bool Connect(const char* ip, unsigned short port,int timeoutms = 1000);
    bool SetBlock(bool isblock);

public:
    int sock;
    unsigned short port;
    char ip[16];
};

#endif
