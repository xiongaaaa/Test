#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <thread>

void TcpClientThread()
{
	//------------socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("create client-socket fail\n");
		return;
	}
	printf("create client-socketfd:%d\n", sockfd);

	struct sockaddr_in addr;
	memset (&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	inet_pton(AF_INET,"127.0.0.1",&addr.sin_addr.s_addr); 

	sleep(2);//wait server ready

	//------------connect
	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)))
	{
		printf("connect fail\n");
		return;
	}
	printf("connect ok\n");

	while(1)
	{
		static int i = 0;
		std::string str("HelloTcp:" + std::to_string(++i));
		//------------send
		send(sockfd, str.c_str(), str.length(), 0);
        //write(sockfd, str.c_str(), str.length());
		usleep(800);
	}
}
