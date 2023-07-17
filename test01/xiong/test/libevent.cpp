#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <errno.h>
#include <iostream>
using namespace std;
#include <event.h>

void readcb(evutil_socket_t fd,short events,void* arg)
{
    char buf[256]="";
    int ret = recv(fd,buf,sizeof(buf),0);
    if(ret <= 0)
    {
        close(fd);
        //event_del(getEventByFd(fd));
    }else{
        for(int i =0;i<ret; ++i)
        {
            buf[i] = toupper(buf[i]);
        }
        send(fd,buf,ret,0);
    }
}

void conncb(evutil_socket_t fd,short events,void *arg)
{
    struct event_base* base = (struct event_base*)arg;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int cfd = accept(fd,(struct sockaddr*)&client,&len);
    if(cfd >0)
    {
        struct event* readev = event_new(base,cfd,EV_READ | EV_PERSIST,readcb,base);
        event_add(readev,NULL);   
    }
}

int main(int argc,char** argv)
{
    struct event_base *base = event_base_new();

    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd == -1){
        perror("socket");
        exit(1);
    }

    struct sockaddr_in saddr;
    bzero(&saddr,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(bind(lfd,(struct sockaddr*)&saddr,sizeof(saddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    listen(lfd,128);

    struct event* connev = event_new(base,lfd,EV_READ| EV_PERSIST,conncb,base);

    event_add(connev,NULL);

    event_base_dispatch(base);

    close(lfd);
    event_base_free(base);

    return 0;
}
 