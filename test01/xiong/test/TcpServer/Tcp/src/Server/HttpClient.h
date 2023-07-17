#ifndef XHTTPCLIENT_H
#define XHTTPCLIENT_H

#include "XTcp.h"
#include "HttpResponse.h"

class XHttpClient
{
public:
    XHttpClient();
    ~XHttpClient();

    bool Start(XTcp client);
    void Main();
public:
    XTcp client;
    XHttpResponse res;
};



#endif