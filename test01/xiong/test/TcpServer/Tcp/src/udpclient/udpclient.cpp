#include <iostream>
using namespace std;
#include<thread>

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

int main(int argc, char** argv)
{
	unsigned short port = 8080;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}

	//windows�ϵ���socket֮ǰ�������ȳ�ʼ��socket��
#ifdef WIN32
	WSADATA ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
#endif


	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock <= 0)
	{
		cout << "create socket failed!" << endl;
		return -1;
	}

	int optval = 1;
	setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(const char*)&optval,sizeof(optval));

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	//saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_addr.s_addr = INADDR_BROADCAST;
#ifdef WIN32
	int len = sizeof(saddr);
#else
	socklen_t len = sizeof(saddr);
#endif
	int num = 0;
	char buf[128] = {0};
	while (1)
	{
		sprintf(buf, "I'm client:%d", num++);
		int size = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&saddr, sizeof(saddr));
		cout << "sendto size is " << size << endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));

		recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&saddr,&len);
		cout << "recv :" << buf << endl;
	}

	return 0;
}
