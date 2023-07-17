#include <iostream>
using namespace std;
#include <thread>

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

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (::bind(sock, (struct sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		cout << "bind port " << port << "failed!" << endl;
		return -2;
	}
	cout << "bind port " << port << "success!" << endl;

	listen(sock, 10);

	struct sockaddr_in client;
#ifdef WIN32
	int len = sizeof(client);
#else
	socklen_t len = sizeof(client);
#endif
	char buf[1020] = { 0 };
	int num = 0;
	
	while (1)
	{
		int recvsize = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &len);
		if (recvsize <= 0)
		{
			cout << "recvFrom failed!" << endl;
			return -3;
		}
		if (recvsize > 0)
			buf[recvsize] = '\0';

		cout << "recv " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << endl;

		cout << buf << endl;

		sprintf(buf, "������ְ֣�%d", num++);
		sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, sizeof(client));
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}


	return 0;
}
