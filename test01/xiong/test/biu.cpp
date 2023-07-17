#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
 
int glo = 50;
int main()
{
    int var = 88;

    int *p = (int*)malloc(sizeof(int));
    memset(p,0,sizeof(int));
    *p = 200;
    int fd = open("aaa.txt",O_RDWR|O_CREAT,0664);
    write(fd,"xiong",5);


    pid_t pid = fork();
    if(-1 == pid)
    {
        perror("fork");
        return 1;
    }

    if(0 == pid)
    {
        sleep(1);
        printf("child sleep end,pid = %d,*p = %d,glo = %d,var = %d\n",getpid(),*p,glo,var);
        free(p);
        char b[128];
        lseek(fd,0,SEEK_SET);
        read(fd,b,128);
        printf("read:%s\n",b);
    }
    else
    {
        printf("parent process before,pid = %d,*p = %d,glo = %d,var = %d\n",getpid(),*p,glo,var);
        ++var;glo += 5;
        ++(*p);
        printf("parent process end,*p = %d,,glo = %d,var = %d\n",*p,glo,var);
        free(p);
        write(fd,"1111111111",10);
    }


    return 0;
}