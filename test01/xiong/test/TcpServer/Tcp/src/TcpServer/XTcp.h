#pragma once
#include <string>

class XTcp
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

public:
    int sock;
    unsigned short port;
    char ip[16];
};

