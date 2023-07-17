#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <thread>

#include "ngx_c_crc32.h"

int SendData(int sSocket, char *p_sendbuf, int ibuflen);
void OnBnClickedButton2(int sockfd);

//结构定义------------------------------------
#pragma pack (1) //对齐方式,1字节对齐 
//一些和网络通讯相关的结构放在这里
typedef struct _COMM_PKG_HEADER
{
	unsigned short pkgLen;    //报文总长度【包头+包体】--2字节，2字节可以表示的最大数字为6万多，我们定义_PKG_MAX_LENGTH 30000，所以用pkgLen足够保存下
	unsigned short msgCode;   //消息类型代码--2字节，用于区别每个不同的命令【不同的消息】
	int            crc32;     //CRC32效验--4字节，为了防止收发数据中出现收到内容和发送内容不一致的情况，引入这个字段做一个基本的校验用	
}COMM_PKG_HEADER, *LPCOMM_PKG_HEADER;

typedef struct _STRUCT_REGISTER
{
	int           iType;          //类型
	char          username[56];   //用户名 
	char          password[40];   //密码

}STRUCT_REGISTER, *LPSTRUCT_REGISTER;


typedef struct _STRUCT_LOGIN
{
	char          username[56];   //用户名 
	char          password[40];   //密码

}STRUCT_LOGIN, *LPSTRUCT_LOGIN;


#pragma pack() //取消指定对齐，恢复缺省对齐

int  g_iLenPkgHeader = sizeof(COMM_PKG_HEADER);


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
    
    int iSendRecvTimeOut = 5000;
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&iSendRecvTimeOut, sizeof(int));
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&iSendRecvTimeOut, sizeof(int));

	while(1)
	{
        //send(sockfd, str.c_str(), str.length(), 0);
        OnBnClickedButton2(sockfd);
		usleep(800);
	}
}

//发送数据，值得讲解
int SendData(int sSocket, char *p_sendbuf, int ibuflen)
{
	int usend = ibuflen; //要发送的数目
	int uwrote = 0;      //已发送的数目
	int tmp_sret;

	while (uwrote < usend)
	{
		tmp_sret = send(sSocket, p_sendbuf + uwrote, usend - uwrote, 0);
		if ((tmp_sret == errno) || (tmp_sret == 0))
		{
			//有错误发生了
			perror("send:");
		}
		uwrote += tmp_sret;
	}//end while
	return uwrote;
}


void OnBnClickedButton2(int sockfd)
{
	CCRC32   *p_crc32 = CCRC32::GetInstance();

	char *p_sendbuf = (char *)new char[g_iLenPkgHeader + sizeof(STRUCT_REGISTER)];

	LPCOMM_PKG_HEADER         pinfohead;
	pinfohead = (LPCOMM_PKG_HEADER)p_sendbuf;
	pinfohead->msgCode = 5;
	pinfohead->msgCode = htons(pinfohead->msgCode);
	//pinfohead->crc32 = htonl(123); //测试，所以随便来 一个
	pinfohead->pkgLen = htons(g_iLenPkgHeader + sizeof(STRUCT_REGISTER));

	LPSTRUCT_REGISTER pstruc_sendstruc = (LPSTRUCT_REGISTER)(p_sendbuf + g_iLenPkgHeader);
	pstruc_sendstruc->iType = htonl(100);
	strcpy(pstruc_sendstruc->username, "xiong");
	strcpy(pstruc_sendstruc->password, "123456");

	//crc值需要最后算的
	pinfohead->crc32   = p_crc32->Get_CRC((unsigned char *)pstruc_sendstruc, sizeof(STRUCT_REGISTER));
	pinfohead->crc32   = htonl(pinfohead->crc32); //针对四字节数字，主机序转网络序

	if (SendData(sockfd, p_sendbuf, g_iLenPkgHeader + sizeof(STRUCT_REGISTER)) == errno)
	{
		delete[] p_sendbuf; //内存总还是要释放的
		return;
	}

	delete[] p_sendbuf; //释放上边的内存

	//再发个登录命令
	p_sendbuf = (char *)new char[g_iLenPkgHeader + sizeof(STRUCT_LOGIN)];

	//LPCOMM_PKG_HEADER         pinfohead;
	pinfohead = (LPCOMM_PKG_HEADER)p_sendbuf;
	pinfohead->msgCode = 6;  //变化一下
	pinfohead->msgCode = htons(pinfohead->msgCode);
	pinfohead->pkgLen = htons(g_iLenPkgHeader + sizeof(STRUCT_LOGIN));

	LPSTRUCT_LOGIN pstruc_sendstruc2 = (LPSTRUCT_LOGIN)(p_sendbuf + g_iLenPkgHeader);
	strcpy(pstruc_sendstruc2->username, "5678");

	//crc值需要最后算的
	pinfohead->crc32 = p_crc32->Get_CRC((unsigned char *)pstruc_sendstruc2, sizeof(STRUCT_LOGIN));
	pinfohead->crc32 = htonl(pinfohead->crc32); //针对四字节数字，主机序转网络序

	if (SendData(sockfd, p_sendbuf, g_iLenPkgHeader + sizeof(STRUCT_LOGIN)) == errno)
	{
		//测试程序也就不关闭socket，直接返回去算了
		delete[] p_sendbuf; //内存总还是要释放的
		return;
	}
	delete[] p_sendbuf;
}