#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
using namespace std;
#include <pthread.h>
#include <wait.h>
#include <sys/eventfd.h>
#include <string.h>
#include <errno.h>

void* threadFunc(void* args)
{
    uint64_t buf;
    int rc;

    while(1)
    {
        rc = read(*(int*)args,&buf,sizeof(buf));
        if(rc == 8)
        {
            printf("notify success\n");
        }
        printf("rc = %llu,buffer = %lu\n",(unsigned long long)rc,buf);
    }
}

int main(int argc, char **argv)
{
    pthread_t tid;
    int rc;
    uint64_t buf = 1;
    int efd;

    efd = eventfd(0, 0);
    if (efd == -1)
    {
        perror("eventfd");
    }

    if (pthread_create(&tid, NULL, threadFunc, &efd) != 0)
    {
        perror("pthread_create");
        exit(-1);
    }
    while(1){
        rc = write(efd,&buf,sizeof(buf));

        if(rc != 8)
        {
            perror("write");
        }
        sleep(2);
    }
    close(efd);

    return 0;
}