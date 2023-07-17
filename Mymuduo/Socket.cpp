#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>


Socket::~Socket()
{
    ::close(sockfd_);
}

void Socket::bindAddress(const InetAddress &Localaddr)
{
    if(0 != ::bind(sockfd_,(struct sockaddr*)Localaddr,sizeof(struct sockaddr*)))
    {
        LOG_FATAL("bind sockfd:%d fail\n",sockfd_);
    }
}

void Socket::listen()
{
    if(0 != listen(sockfd_,1024))
    {
        LOG_FATAL("listen sockfd:%d fail \n",sockfd_);
    }
}

void Socket::accept(InetAddress *peeraddr)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    bzero(&addr,sizeof(addr));
    int connfd = ::accept(sockfd_,(struct sockaddr*)&addr,&len);
    if(connfd >= 0)
    {
        peeraddr->setSocAddr(addr);
    }
    return connfd;
}

void Socket::shutdownWrite()
{
    if(::shutdown(sockfd_,SHUT_WR) <0)
    {
        LOG_ERROR("shutdownWrite error");
    }
}
    
void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
}
void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
}