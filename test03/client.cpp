#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "define.h"
 
 
#define DEFAULT_PORT 6666
int main(int argc,char **argv)
{
	int connfd = 0;
	int cLen = 0;
	struct sockaddr_in client;
	if(argc < 2)
	{
		printf("Usage: client [server IP address]\n");
		return -1;
	}
	client.sin_family = AF_INET;
	client.sin_port = htons(DEFAULT_PORT);
	client.sin_addr.s_addr = inet_addr(argv[1]);
	connfd = socket(AF_INET,SOCK_STREAM,0);
	if(connfd <0 )
	{
		printf("socket() failure!\n");
		return -1;
	}
	if(connect(connfd,(struct sockaddr*)&client,sizeof(client)) <0){
		printf("connect() failure!\n");
		return -1;
	}

	Pkg mypkg;
	mypkg.head.num =1;
	mypkg.head.index = 10001;
	mypkg.content.sex = 'm';
	mypkg.content.score = 90;
	const char *temp = "GuangZhou and ShangHai";
	strncpy(mypkg.content.address,temp,sizeof(mypkg.content.address));
	mypkg.content.age = 28;
	ssize_t writeLen;
	int tLen = sizeof(mypkg);	//整个Pkg结构体的大小
	printf("tLen:%d\n",tLen);
	int iLen = 0;
	char *pBuff  = new char[1000];

	//整形数字类型用htonl转换、字符串类型用memcpy转换
	*(int*)pBuff = htonl(tLen);
	iLen += sizeof(int);
	
	*(int*)(pBuff + iLen) = htonl(mypkg.head.num);
	iLen += sizeof(int);

	*(int*)(pBuff + iLen) = htonl(mypkg.head.index);
	iLen += sizeof(int);

	memcpy(pBuff + iLen,&mypkg.content.sex,sizeof(char));
	iLen += sizeof(char);

	*(int*)(pBuff + iLen) = htonl(mypkg.content.score);
	iLen += sizeof(int);

	memcpy(pBuff + iLen,mypkg.content.address,sizeof(mypkg.content.address));
	iLen +=(sizeof(mypkg.content.address));

	*(int*)(pBuff + iLen) = htonl(mypkg.content.age);
	iLen += sizeof(int);
	writeLen = MySend(connfd,pBuff,iLen);
	if(writeLen < 0){
		printf("write failed!\n");
		close(connfd);
		return 0;
	}else{
		printf("write success,writenlen :%ld,iLen:%d,pBuff:%s\n",writeLen,iLen,pBuff);	
	}

	close(connfd);
	delete []pBuff;
	return 0;
}

