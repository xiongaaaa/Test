#include "threadpool.h"
#include <iostream>

const int TASK_MAX_THRESHHOLD = 1024;
const int THREAD_MAX_THRESHHOLD = 100;
const int THREAD_MAX_IDLE_TIME = 60;  //单位:秒

//线程池构造
ThreadPool::ThreadPool() :
	initThreadSize_(0),
	taskSize_(0),
	idleThreadSize_(0),
	curThreadSize_(0),
	taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),
	threadSizeThreshHold_(THREAD_MAX_THRESHHOLD),
	poolMode_(PoolMode::MODE_FIXED),
	isPoolRunning_(false)
{}

//线程池析构
ThreadPool::~ThreadPool() 
{
	isPoolRunning_ = false;

	//等待线程池里面的线程返回，有两种状态：阻塞  &  正在执行任务中
	std::unique_lock<std::mutex> lock(taskQueMtx_);
	notEmpty_.notify_all();
	exitCond_.wait(lock, [&]()->bool {return threads_.size() == 0; });
}

//设置线程池的工作模式
void ThreadPool::setMode(PoolMode mode)
{
	if (checkRunningState())
		return;
	poolMode_ = mode;
}

//设置task任务队列上限阈值
void ThreadPool::setTaskQueMaxThreshHold(int threshhold)
{
	if (checkRunningState())
		return;
	taskQueMaxThreshHold_ = threshhold;
}

//设置线程池cached模式下线程上限阈值
void ThreadPool::setThreadSizeThreshHold(int threshhold)
{
	if (checkRunningState())
		return;
	if(poolMode_ == PoolMode::MODE_CACHED)
		threadSizeThreshHold_ = threshhold;	
}

//给线程池提交任务
Result ThreadPool::submitTask(std::shared_ptr<Task> sp)
{
	//获取锁
	std::unique_lock<std::mutex> lock(taskQueMtx_);

	//线程的通信，等待任务队列有空余
	if (!notFull_.wait_for(lock, std::chrono::seconds(1),
		[&]()->bool {return taskQue_.size() < (size_t)taskQueMaxThreshHold_; }))//条件不满足，则阻塞
	{
		//表示notFull_等待1秒，条件依然没有满足
		std::cerr << "task queue is full,submit task fail." << std::endl;
		return Result(sp, false);
	}

	//如果有空余，把任务放入任务队列中
	taskQue_.emplace(sp);
	++taskSize_;

	//因为新放了任务，任务队列肯定不空了，在notEmpty_上进行通知,赶快分配线程执行任务
	notEmpty_.notify_all();

	//cached模式，任务处理比较紧急 场景：小而快的任务；需要根据【任务数量】和【空闲线程的数量】，判断是否需要创建新的线程出来
	if (poolMode_ == PoolMode::MODE_CACHED
		&& taskSize_ > idleThreadSize_
		&& curThreadSize_ < threadSizeThreshHold_)
	{
		std::cout << ">>> create new thread..." << std::endl;

		//创建新的线程对象
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));
		int threadId = ptr->getId();
		threads_.emplace(threadId, std::move(ptr));
		//启动线程
		threads_[threadId]->start();
		//修改线程个数相关的变量
		++curThreadSize_;
		++idleThreadSize_;
	}

	return Result(sp);
}

//开启线程池
void ThreadPool::start(int initThreadSize)
{
	//设置线程池的运行状态
	isPoolRunning_ = true;

	//记录初始的线程个数
	initThreadSize_ = initThreadSize;
	curThreadSize_  = initThreadSize;

	//创建线程对象
	for (int i = 0; i < initThreadSize_; ++i)
	{
		//创建thread线程对象的时候，把线程函数给thread线程对象
		auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this,std::placeholders::_1));
		int threadId = ptr->getId();
		threads_.emplace(threadId,std::move(ptr));	
	}

	//启动所有线程
	for (int i = 0; i < initThreadSize_; ++i)
	{
		threads_[i]->start();	//需要去执行一个线程函数
		++idleThreadSize_;
	}
}

//定义线程入口函数
void ThreadPool::threadFunc(int threadid)
{
	auto lastTime = std::chrono::high_resolution_clock().now();

	//所有任务必须执行完成，线程池才可以回收所有线程资源
	for (;;)
	{
		std::shared_ptr<Task> task;
		{
			//先获取锁
			std::unique_lock<std::mutex> lock(taskQueMtx_);

			std::cout << "tid:" << std::this_thread::get_id() << "尝试获取任务..." << std::endl;

			//在cached模式下，有可能已经创建了很多线程，但是空闲时间超过60s，应该把多余的线程
			  //结束回收掉(超过initThreadSize_数量的线程要进行回收)
			//(当前时间 - 上一次线程的执行的时间) > 60s

			//每一秒返回一次  怎么区分：超时返回？还是有任务执行返回
			//锁 + 双重判断
			while (taskQue_.size() == 0)
			{
				//线程池要结束，回收线程资源
				if (!isPoolRunning_)
				{
					threads_.erase(threadid);
					std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
					exitCond_.notify_all();
					return;  //线程函数结束，当前线程结束
				}

				if (poolMode_ == PoolMode::MODE_CACHED)
				{
					//条件变了，超时返回了
					if (std::cv_status::timeout == notEmpty_.wait_for(lock, std::chrono::seconds(1)))
					{
						auto now = std::chrono::high_resolution_clock().now();
						auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
						if (dur.count() >= THREAD_MAX_IDLE_TIME
							&& curThreadSize_ > initThreadSize_)
						{
							//开始回收当前线程
							//记录线程数量的相关变量的值的修改
							//把线程对象从线程列表容器中删除
							//threadId  ===>   thread对象   ===》删除
							threads_.erase(threadid);
							--curThreadSize_;
							--idleThreadSize_;

							//std::cout<<"*********************"<<std::endl;		
							std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
							return;
						}
					}
				}
				else {
					//等待notEmpty条件
					notEmpty_.wait(lock);
				}
			}

			//空闲线程数 -1
			--idleThreadSize_;

			std::cout << "tid:" << std::this_thread::get_id() << "【获取任务成功】..." << std::endl;

			//从任务队列中取一个任务出来
			task = taskQue_.front();
			taskQue_.pop();
			--taskSize_;

			//如果依然有剩余任务，继续通知其他线程执行任务
			if (taskQue_.size() > 0)
			{
				notEmpty_.notify_all();
			}

			//取出一个任务，进行通知，通知可以继续提交生产任务
			notFull_.notify_all();
		}//释放锁

		//当前线程负责执行这个任务
		if (task != nullptr)
		{
			//task->run(); //执行任务;把任务的返回值setVal()给到Result
			task->exec();
		}

		//空闲线程数 +1
		++idleThreadSize_;

		lastTime = std::chrono::high_resolution_clock().now();	//更新线程执行完成任务的时间
	}
}

bool ThreadPool::checkRunningState() const
{
	return isPoolRunning_;
}

////////////【Thread方法实现】//////////////
int Thread::generateId_ = 0;

//线程构造
Thread::Thread(ThreadFunc func) :
	func_(func),
	threadId_(generateId_++) 
{
	std::cout << "threadID :" << threadId_ << "<====>construct..." << std::endl;
}

//线程析构
Thread::~Thread() {}

//启动线程
void Thread::start()
{
	//创建一个线程来执行一个线程函数
	std::thread t(func_ , threadId_);   // C++11来说 线程对象t  和线程函数func_
	t.detach();
}

//获取线程id
int Thread::getId() const
{
	return threadId_;
}

////////////【Task方法实现】//////////////
Task::Task() :result_(nullptr) {}

void Task::exec()
{
	if (result_ != nullptr)
		result_->setVal(run());		//这里发生多态调用
}

void Task::setResult(Result* res)
{
	result_ = res;
}

////////////【Result方法实现】//////////////
Result::Result(std::shared_ptr<Task> task, bool isValid):
	task_(task),
	isValid_(isValid)
{
	task_->setResult(this);
}

Any Result::get()	  //用户调用
{
	if (!isValid_)
		return "";

	sem_.wait();	 //task任务如果没有执行完，这里会阻塞用户的线程
	return std::move(any_);
}

void Result::setVal(Any any)
{
	//存储task的返回值
	this->any_ = std::move(any);
	sem_.post();	//已经获取的任务的返回值，增加信号量资源
}
