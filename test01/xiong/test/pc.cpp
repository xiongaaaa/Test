#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <list>

class SellTickets
{
public:
    ~SellTickets();
public:
    void Create(int size);
    void Join(int size);
    static void *Producer(void *arg);
    static void *Customer(void *arg);

private:
    pthread_t *ptid;
    pthread_t *ctid;
    static int m_total;
    static pthread_mutex_t m_mutex;
    static pthread_cond_t m_cond;
    static std::list<char*> lst;
};
int SellTickets::m_total =0;
std::list<char*> SellTickets::lst;
pthread_mutex_t SellTickets::m_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t SellTickets::m_cond = PTHREAD_COND_INITIALIZER;

void SellTickets::Create(int size)
{
    ptid = new pthread_t[size];
    ctid = new pthread_t[size];
    for (int i = 0; i < size; ++i)
    {
        if(i <5)
        {
            int ret = pthread_create(&ptid[i], NULL, Producer, NULL);
            if (0 != ret)
            {
                printf("pthread_create--Producer%d failed...\n", i);
            }
        }
        int ret = pthread_create(&ctid[i], NULL, Customer, NULL);
        if (0 != ret)
        {
            printf("pthread_create--Customer%d failed...\n", i);
        }
    }
}

SellTickets::~SellTickets()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
    pthread_exit(NULL);
}

void SellTickets::Join(int size)
{
    for (int i = 0; i < size; ++i)
    {
        if(i < 5)
        {
            pthread_join(ptid[i], NULL);
        }
        pthread_join(ctid[i], NULL);
    }
}

void *SellTickets::Producer(void *arg)
{  
   char buf[128];
   while(m_total !=10000)
   {
        memset(buf,0,sizeof(buf));
        pthread_mutex_lock(&m_mutex);
        sprintf(buf,"This is a Data%d",++m_total);
        lst.emplace_back(buf);
        printf("%lu====生产了%s\n",pthread_self(),buf);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
        usleep(500);
   }
   
    return NULL;
}

void *SellTickets::Customer(void *arg)
{
    std::string p;
    while(m_total !=10000)
    {
        pthread_mutex_lock(&m_mutex);
        if(!lst.empty())
        {
            p = lst.front();
            lst.pop_front();
            printf("%lu---消费了%s\n",pthread_self(),p.c_str());
            pthread_mutex_unlock(&m_mutex);
            usleep(500);
        }else{
            pthread_cond_wait(&m_cond,&m_mutex);
            pthread_mutex_unlock(&m_mutex);
        }
    }
   return NULL;
}

int main()
{
    SellTickets sell;
    sell.Create(100);
    sell.Join(100);




    return 0;
}
