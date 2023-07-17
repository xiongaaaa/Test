#include "XTcp.h"
#include <unistd.h>
#include <string.h>

int main()
{
	XTcp client;
	client.CreateSocket();
	client.SetBlock(false);
	//client.Connect("127.0.0.1", 8080,3000);
	client.Connect("192.168.10.55", 8081,3000);

	// for(;;)
	// {
		//client.Send("client",6);
		const char *msg = "GET / HTTP/1.1\r\n\r\n";
		client.Send(msg,strlen(msg));
		char buf[1024] = {0};
		client.Recv(buf, sizeof(buf));
		printf("%s\n", buf);
	//}

	 getchar();


	return 0;
}