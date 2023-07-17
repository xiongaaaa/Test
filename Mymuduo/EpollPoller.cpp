#include "EpollPoller.h"

//Channel未添加到Poller中
const int kNew = -1;    //Channel的成员index_ = -1
//Channel添加到Poller中
const int kAdded = 1;
//Channel从Poller删除
const int kDeleted =2;


EpollPoller::EpollPoller(EventLoop *loop)
    :Poller(loop),epollfd_(::epoll_create1(EPOLL_CLOEXEC)),events_(kInitEventListSize)     
{
    if(epollfd_ < 0){
        LOG_FATAL("epoll_create error:%d\n",errno);
    }
}   

EpollPoller::~EpollPoller()
{
    ::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutMs,ChannelList *activeChannels)
{
    //实际上应该用LOG_DEBUG输入日志更合理
    LOG_INFO("func=%s => fd total count:%lu\n",__FUNCTION__,channels_.size());

    int numEvents = ::epoll_wait(epollfd_,&*events_.begin(),static_cast<int>(events_.size()),timeoutMs);
    int saveError = errno;
    Timestamp now(Timestamp::now());

    if(numEvents >0)
    {
        LOG_INFO("%d events happened \n",numEvents);
        fillActiveChannels(numEvents,activeChannels);
        //扩容
        if(numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0){
        LOG_DEBUG("%s timeout! \n",__FUNCTION__);
    }else{
        errno = saveError;
        LOG_ERROR("EpollPoller::poll() err!\n");
    }
    return now;
}

//channel update remove  --> EventLoop updateChannel removeChannel  -->Poller updateChannel removeChannel
/*
*                 EventLoop
*       ChannelList        Poller
*           ChannelMap<fd,channel*>  epollfd
*/

void EpollPoller::updateChannel(Channel *channel)      //epoll_ctl 
{
    const int index = channel->index();
    LOG_INFO("func=%s => fd = %d,events = %d,index = %d\n",__FUNCTION__,channel->fd(),channel->events(),index);
    
    if(index == kNew || index == kDeleted)
    {
        if(index == kNew){
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD,channel);
    }
    else  //channel已经在poller上注册过了
    {
        int fd = channel->fd();
        if(channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL,channel);
            channel->set_index(kDeleted);
        }else{
            update(EPOLL_CTL_MOD,channel);
        }
    }
}

//从Poller中删除Channel
void EpollPoller::removeChannel(Channel *channel)      //epoll_ctl 
{
    int fd = channel->fd();
    channels_.erase(fd);
    //channels_.erase(channel->fd())

    LOG_INFO("func=%s => fd = %d\n",__FUNCTION__,fd);
    int index = channel->index();
    if(index == kAdded)
    {
        update(EPOLL_CTL_DEL,channel);
    }
    channel->set_index(kNew);
}

    //填写活跃的连接
void EpollPoller::fillActiveChannels(int numEvents,ChannelList *activeChannels) const
{
    for(int i=0;i<numEvents;++i)
    {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);//EventLoop就拿到了它的Poller给它返回的所有发生事件的channel列表了
    }
}

//更新Channel通道    epoll_ctl   add/mod/del
void EpollPoller::update(int operation,Channel *channel)
{
    struct epoll_event event;
    bzero(&event,sizeof(event));

    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if(::epoll_ctl(epollfd_,operation,fd,&event) <0)
    {
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctl del error:%d\n",errno);
        }else{
            LOG_FATAL("epoll_ctl add/del error:%d\n",errno);
        }
    }
}
