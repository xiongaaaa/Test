// #include <stdio.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/epoll.h>
// #include <thread>

// #define BUF_SIZE 64
// #define LISTEN_MAX     512
// #define EPOLL_FDSIZE   LISTEN_MAX
// #define EPOLL_EVENTS   1024
// #define CLIENT_NUM     10000


// void EpollAddEvent(int epollfd, int fd, int event)
// {
//     printf("epollfd:%d add fd:%d(event:%d)\n", epollfd, fd, event);
//     struct epoll_event ev;
//     ev.events = event;
//     ev.data.fd = fd;
//     epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
// }

// void TcpServerThread()
// {
// 	//------------socket
// 	int lfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (lfd < 0)
// 	{
// 		printf("create server-socket fail\n");
// 		return;
// 	}
// 	//printf("create server-socketfd:%d\n", lfd);

// 	struct sockaddr_in saddr;
// 	memset(&saddr, 0, sizeof(saddr));
// 	saddr.sin_family = AF_INET;
// 	saddr.sin_port = htons(8800);
//     saddr.sin_addr.s_addr = htonl(INADDR_ANY);

// 	int opt = 1;
// 	setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
// 	//------------bind
// 	if (bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr)))
// 	{
// 		printf("bind fail\n");
// 		return;
// 	}
// 	printf("bind ok\n");

// 	//------------listen
// 	if (listen(lfd, LISTEN_MAX))
// 	{
// 		printf("listen fail\n");
// 		return;
// 	}
// 	printf("listen ok\n");

// 	//------------epoll---------------
// 	int epollfd = epoll_create(EPOLL_FDSIZE);
// 	if (epollfd < 0)
// 	{
// 		printf("epoll create fail\n");
// 		return;
// 	}
// 	printf("epoll create fd:%d\n", epollfd);

// 	EpollAddEvent(epollfd, lfd, EPOLLIN);
// 	//printf("add server-lfd:%d success!!!\n",lfd);

// 	struct epoll_event events[EPOLL_EVENTS];
// 	while(1)
// 	{
// 		printf("epoll wait...\n");
// 		int num = epoll_wait(epollfd, events, EPOLL_EVENTS, -1);
// 		printf("epoll wait done, num:%d\n", num);
// 		for (int i = 0;i < num;i++)
// 		{
// 			int curfd = events[i].data.fd;
// 			if (EPOLLIN == events[i].events)
// 			{
// 				//接受客户端的连接请求
// 				if (curfd == lfd)
// 				{
// 					//------------accept
// 					int clientfd = accept(lfd, NULL, NULL);
// 					if (clientfd == -1)
// 					{
// 						printf("accpet error\n");
// 					}
// 					else
// 					{
// 						printf("=====> accept new clientfd:%d\n", clientfd);
						
// 						EpollAddEvent(epollfd, clientfd, EPOLLIN);
// 						//printf("add server-clientfd:%d success!!!\n",clientfd);

// 					}
// 				}
// 				//读取客户端发来的数据
// 				else
// 				{
// 					char buf[BUF_SIZE] = {0};
// 					//------------recv
// 					size_t size = recv(curfd, buf, BUF_SIZE, 0);
// 					//size = read(clientfd, buf, BUF_SIZE);
// 					if (size > 0)
// 					{
// 						printf("recv from clientfd:%d, msg:%s\n", curfd, buf);
// 					}
// 				}
// 			}
// 		}
// 	}

// 	printf("end\n");
// }
