#include <stdlib.h>
#include <cstring>
#include <thread>

#ifdef WIN32

#else
#include <sys/epoll.h>
#endif

#include "XTcp.h"

class TcpThread
{
public:
    void Main()
    {
        char buf[1024] = { 0 };
        for (;;)
        {
            int recvlen = client.Recv(buf, sizeof(buf)-1);
            if (recvlen <= 0)
                break;
            buf[recvlen] = '\0';
            if (strstr(buf, "quit") != NULL)
            {
                char re[] = "quit success!";
                client.Send(re, strlen(re));
                break;
            }
            int sendlen = client.Send("ok\n", 4);

            printf("recv:%s\n", buf);
        }
        client.Close();
        delete this;
    }
public:
    XTcp client;
};

int main(int argc, char **argv)
{
    unsigned short port = 8080;
    if(argc > 1)
    {
        port = atoi(argv[1]);
    }
    XTcp server;
    server.Bind(port);
    int epfd = epoll_create(256);

	//注册epoll事件
    struct epoll_event epev,epevs[1024];
	epev.data.fd = server.sock;
    epev.events = EPOLLIN | EPOLLET;  //数据接入事件|边缘检测
    epoll_ctl(epfd,EPOLL_CTL_ADD,server.sock,&epev);
		
	char buf[1024] = {0};
    const char* msg = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nXiong";
	int size = strlen(msg);
	
	server.SetBlock(false);
	for (;;)
	{
		//检测事件
        int count = epoll_wait(epfd,epevs,1024,500);
        if(count <= 0)
            continue;
		for(int i = 0; i<count; ++i)
		{
			//连接事件发生
            if(epevs[i].data.fd == server.sock)
			{
				for(;;)
				{
                    XTcp client = server.Accept();
                    if(client.sock <= 0)
                        break;
					
					//新注册客户端事件
                    epev.data.fd = client.sock;
                    epev.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,client.sock,&epev);
				}
			}
			else
			{
                XTcp client;
                client.sock = epevs[i].data.fd;
                client.Recv(buf,1024);
                client.Send(msg,size);
				//客户端处理完毕，清理事件
                epoll_ctl(epfd,EPOLL_CTL_DEL,client.sock,&epev);
                client.Close();
			}
		}
	}
	server.Close();
	getchar();


	return 0;
}