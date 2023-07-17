#include "threadpool.h"

ThreadPool* thrPool = NULL;

int beginnum = 1000;

void* thrRun(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    int taskpos = 0;     //任务位置
    PoolTask* task = new PoolTask;

    while(1)
    {
        //获取任务，先要尝试加锁
        pthread_mutex_lock(&pool->pool_lock);

        //无任务并且线程池不是要摧毁
        while(pool->job_num <=0 && !pool->shutdown)
        {
            //如果没有任务，线程会阻塞
            pthread_cond_wait(&pool->not_empty_task,&pool->pool_lock);
        }

        if(pool->job_num)
        {
            //有任务需要处理
            taskpos = (pool->job_pop++)%pool->max_job_num;
            //为什么要拷贝？避免任务被修改，生产者会添加任务
            memcpy(task,&pool->tasks[taskpos],sizeof(PoolTask));
            task->arg = task;
            pool->job_num--;
            pthread_cond_signal(&pool->empty_task);  //通知生产者
        }

        if(pool->shutdown)
        {
            //代表要摧毁线程池，此时线程退出即可
            pthread_mutex_destroy(&pool->pool_lock);
            delete task;
            pthread_exit(NULL);
        }

        //释放锁
        pthread_mutex_unlock(&pool->pool_lock);
        task->task_func(task->arg);     //执行线程回调函数
    }
}

//创建线程池
void create_threadpool(int thrnum,int maxtasknum)
{
    printf("begin call %s------\n",__FUNCTION__);
    thrPool = new ThreadPool;

    thrPool->thr_num = thrnum;
    thrPool->max_job_num = maxtasknum;
    thrPool->shutdown = 0;      //是否摧毁线程池，1代表摧毁
    thrPool->job_push = 0;      //任务队列添加的位置
    thrPool->job_pop = 0;       //任务队列出队的位置
    thrPool->job_num = 0;       //初始化的任务个数为0

    thrPool->tasks = new PoolTask[maxtasknum];    //申请最大的任务队列

    //初始化锁和条件变量
    pthread_mutex_init(&thrPool->pool_lock,NULL);
    pthread_cond_init(&thrPool->empty_task,NULL);
    pthread_cond_init(&thrPool->not_empty_task,NULL);

    thrPool->threads = new pthread_t[thrnum];    //申请n个线程id的空间
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    for(int i =0; i<thrnum; ++i)
    {
        int ret = pthread_create(&thrPool->threads[i],&attr,thrRun,(void*)thrPool);
        if(0 != ret)
        {
           printf("pthread_create%d failed...\n", i);
        }
    }
}

//添加任务到线程池
void addtask(ThreadPool* pool)
{
    pthread_mutex_lock(&pool->pool_lock);

    //实际任务总数大于最大任务个数则阻塞等待（等待任务被处理）
    while(pool->max_job_num <= pool->job_num)
    {   
        pthread_cond_wait(&pool->empty_task,&pool->pool_lock);
    }

    int taskpos = (pool->job_push++)%pool->max_job_num;
    pool->tasks[taskpos].tasknum = beginnum++;
    pool->tasks[taskpos].arg = (void*)&pool->tasks[taskpos];
    pool->tasks[taskpos].task_func = taskRun;
    pool->job_num++;

    pthread_mutex_unlock(&pool->pool_lock);

    pthread_cond_signal(&pool->not_empty_task);
}

//销毁线程池
void destroy_threadpool(ThreadPool* pool)
{
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->not_empty_task);

    for(int i =0; i<pool->thr_num; ++i)
    {
        pthread_join(pool->threads[i],NULL);
    }

    pthread_cond_destroy(&pool->not_empty_task);
    pthread_cond_destroy(&pool->empty_task);
    pthread_mutex_destroy(&pool->pool_lock);

    delete pool->tasks;
    delete pool->threads;
    delete pool;
}

//任务回调函数
void taskRun(void* arg)
{
    PoolTask* task = (PoolTask*)arg;
    int num = task->tasknum;
    printf("task %d is runing %lu\n",num,pthread_self());

    sleep(1);
    //printf("task %d is done %lu\n",num,pthread_self());
}

int main()
{
    create_threadpool(3,20);
    
    //for(int i =0; i< 50; ++i)
    for(;;)
    {
        addtask(thrPool);
    }
    //sleep(5);

    destroy_threadpool(thrPool);

    return 0;
}