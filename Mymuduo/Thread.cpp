#include "Thread.h"
#include "CurrentThread.h"

#include <semaphore.h>

std::atomic_int Thread::numCreated_ = 0;

Thread::Thread(ThreadFunc func,const std::string &name) 
    :started_(false),
    joined_(false),
    tid_(0),
    func_(std::move(func)),
    name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        thread_->detach();  //thread类提供的设置线程分离的方法
    }
}

void Thread::start()    //一个thread对象,记录的就是一个新线程的详细信息
{
    started_ = true;
    sem_t sem;
    sem_init(&sem,false,0); // false-->用在线程间  true-->用在进程间
    //开启线程
    thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
        //获取线程的tid值
        tid_ = CurrentThread::tid();
        sem_post(&sem);     //对信号量解锁,调用一次对信号量的值+1
        //开启一个新线程,专门执行该线程函数
        func_();    //回调函数,实际执行--->EventLoopThread::threadFunc()
    };));

    //这里必须等待获取上面新创建的线程的tid值
    sem_wait(&sem);     //信号量加锁,调用一次对信号量的值-1,如果值为0,就阻塞
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if(name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf,sizeof(buf),"Thread%d",num);
        name_ = buf;
    }
}