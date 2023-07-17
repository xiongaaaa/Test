#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 128

//先读后写
//以只读方式打开管道1
//以只写到方式打开管道2
int main()
{   
    int ret = -1;
    char buf[SIZE];

    //以只读方式打开管道1
    int fdr = open("fifo1",O_RDONLY);
    if(-1 == fdr)
    {
        perror("open");
        return 1;
    }
    printf("以只读的方式打开管道1...\n");

    //以只写到方式打开管道2
    int fdw = open("fifo2",O_WRONLY); 
    if(-1 == fdw)
    {
        perror("open");
        return 1;
    }
    printf("以只写的方式打开管道2...\n");

    while(1)
    {
        //读管道1
        memset(buf,0,SIZE);
        ret = read(fdr,buf,SIZE);
        if(ret <= 0)
        {
            perror("read");
            break;
        }
        printf("read:%s\n",buf);

        //写管道2
        memset(buf,0,SIZE);
        fgets(buf,SIZE,stdin);
        //去掉最后一个换行符
        if('\n' == buf[strlen(buf) -1])
            buf[strlen(buf) -1] = '\0';

        ret = write(fdw,buf,strlen(buf));

    }

    close(fdr);
    close(fdw);

    return 0;
}
