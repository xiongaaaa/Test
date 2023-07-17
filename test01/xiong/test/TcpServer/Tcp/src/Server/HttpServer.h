#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "XTcp.h"

class XHttpServer
{
public:
    XHttpServer();
    ~XHttpServer();

    bool Start(unsigned short port);
    void Main();
    void Stop();

public:
    XTcp server;
    bool isExit;
};

#endif