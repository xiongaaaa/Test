#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <list>

class SellTickets
{
public:
    void Create(int size);
    void Join(int size);
    void Stop();
    static void *ThreadFunc(void *arg);

private:
    pthread_t *tid;
    static pthread_mutex_t m_mutex;
    static int m_tickets;
    std::list<char*> lst;
};
int SellTickets::m_tickets = 1000000;     //1亿
pthread_mutex_t SellTickets::m_mutex = PTHREAD_MUTEX_INITIALIZER;

void SellTickets::Create(int size)
{
    tid = new pthread_t[size];
    for (int i = 0; i < size; ++i)
    {
        int ret = pthread_create(&tid[i], NULL, ThreadFunc, NULL);
        if (0 != ret)
        {
            printf("pthread_create%d failed...\n", i);
        }
    }
}

void SellTickets::Stop()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_exit(NULL);
}

void SellTickets::Join(int size)
{
    for (int i = 0; i < size; ++i)
    {
        pthread_join(tid[i], NULL);
    }
}

void *SellTickets::ThreadFunc(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&m_mutex);
        if (m_tickets > 0)
        {
            printf("%lu 正在卖第 %d张门票...\n", pthread_self(), m_tickets--);
            //usleep(500);
        }
        else
        {
            pthread_mutex_unlock(&m_mutex);
            break;
        }
        pthread_mutex_unlock(&m_mutex);
    }
    return NULL;
}

int main()
{
    SellTickets sell;
    sell.Create(2000);
    sell.Join(2000);
    sell.Stop();

    return 0;
}
