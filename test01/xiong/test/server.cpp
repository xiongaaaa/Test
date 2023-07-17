#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <errno.h>

int main()
{
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(ret == -1)
    {
        perror("bind");
        exit(1);
    }

    ret = listen(lfd,8);

    fd_set set,oldset;
    FD_ZERO(&set);
    FD_ZERO(&oldset);
    FD_SET(lfd,&oldset);
    int maxfd = lfd;

    while(1)
    {
        set = oldset;
        int ret = select(maxfd+1,&set,NULL,NULL,NULL);
        if(-1 == ret)
        {
            perror("select");
            exit(-1);
        }else if(ret == 0){
            continue;
        }else if(ret >0){
            if(FD_ISSET(lfd,&set)){
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                int cfd = accept(lfd,(struct sockaddr*)&cliaddr,&len);

                FD_SET(cfd,&oldset);

                maxfd = maxfd > cfd ? maxfd : cfd;
            }
            for(int i=lfd+1;i<=maxfd;++i)
            {
                if(FD_ISSET(i,&oldset)){
                    char buf[1024] = {0};
                    int len = read(i,buf,sizeof(buf));
                    if(len == -1){
                        perror("read");
                        exit(-1);
                    }else if(len == 0){
                        printf("client closed...\n");
                        close(i);
                        FD_CLR(i,&oldset);
                    }else if(len >0){
                        printf("read buf = %s\n",buf);
                        write(i,buf,strlen(buf)+1);
                    }
                }
            }
        }           
    }
    close(lfd);

    return 0;
}