#include "HttpServer.h"
#include "HttpClient.h"
#include <thread>
using namespace std;

XHttpServer::XHttpServer() : isExit(false)
{

}

XHttpServer::~XHttpServer()
{

}

bool XHttpServer::Start(unsigned short port)
{
    isExit = false;
    server.CreateSocket();
    if(!server.Bind(port))
        return false;
    
    thread sth(&XHttpServer::Main,this);
    sth.detach();

    return true;
}

void XHttpServer::Main()
{
    while(!isExit)
    {
        XTcp client = server.Accept();
        if(client.sock <= 0)
            continue;
        XHttpClient *th = new XHttpClient();
        th->Start(client);
    }
}

void XHttpServer::Stop()
{
    isExit = true;
}