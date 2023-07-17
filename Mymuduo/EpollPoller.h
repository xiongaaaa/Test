#pragma once

#include "Poller.h"
#include "Timestamp.h"
#include "Logger.h"
#include "Channel.h"
#include <vector>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <strings.h>

/**
*  epoll的使用
*  epoll_create
*  epoll_ctl    add/mod/del
*  epoll_wait 
**/

class Channel;

class EpollPoller : public Poller
{
public:
    EpollPoller(EventLoop *loop);    //epoll_create
    ~EpollPoller() override;

    //重写基类Poller的抽象方法
    Timestamp poll(int timeoutMs,ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;  //--->epoll_ctl
    void removeChannel(Channel *channel) override;  //--->epoll_ctl
private:
    static const int kInitEventListSize = 16;

    //填写活跃的连接
    void fillActiveChannels(int numEvents,ChannelList *activeChannels) const;
    //更新Channel通道    epoll_ctl   add/mod/del
    void update(int operation,Channel *channel);

    using EventList = std::vector<struct epoll_event>;

    int epollfd_;   //epoll_create的返回值
    EventList events_;
};
