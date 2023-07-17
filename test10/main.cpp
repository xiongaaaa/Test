//#include "epoll_server.h"
#include "epoll_client.h"

#include <thread>
#include <iostream>
using namespace std;

int main()
{
	//创建一个服务端
	//thread thServer(TcpServerThread);

	//创建多个客户端
	//thread thClinet[CLIENT_NUM];
	thread thClinet[10000];
	//for (int i=0; i<CLIENT_NUM; i++)
	for (int i=0; i<10000; i++)
	{
		thClinet[i] = thread(TcpClientThread);
		sleep(1);
	}

	while(1)
	{
		
	}
}
