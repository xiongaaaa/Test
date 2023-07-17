#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

pthread_rwlock_t m_rwlock;
int num = 0;

void* Thread_Read(void* arg)
{
   int index = (int)(long)arg;
    while(1)
    {
        pthread_rwlock_rdlock(&m_rwlock);
        printf("thread id%d,Read num value :%d\n",index,num);
        pthread_rwlock_unlock(&m_rwlock);
        sleep(rand()%3);
    }
    return NULL;
}

void* Thread_Write(void* arg)
{
   int index = (int)(long)arg;
    while(1)
    {
        pthread_rwlock_wrlock(&m_rwlock);
        printf("thread id%d,Write num value :%d\n",index,++num);
        pthread_rwlock_unlock(&m_rwlock);
        sleep(rand()%3);
    }
    return NULL;
}

int main()
{
    pthread_rwlock_init(&m_rwlock,NULL);

    pthread_t tid[8];
    for(int i =0; i<8; ++i)
    {
        if(i<5)
        {
            pthread_create(&tid[i],NULL,Thread_Read,(void*)(long)i);
        }
        else{
            pthread_create(&tid[i],NULL,Thread_Write,(void*)(long)i);
        }
    }

    for(int i =0; i<8; ++i)
    {
        pthread_join(tid[i],NULL);
    }

    pthread_rwlock_destroy(&m_rwlock);
    pthread_exit(NULL);

    return 0;
}