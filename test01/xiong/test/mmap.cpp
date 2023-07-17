#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <wait.h>

int main()
{
    int ret = -1;
    pid_t pid;
    void *addr = NULL;

    int fd = open("xiong.txt",O_RDWR);
    if(-1 == fd)
    {
        perror("open");
        exit(1);
    }

    addr = mmap(NULL,1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(addr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }
    close(fd);

    pid = fork();
    if(-1 == pid)
    {
        perror("fork");
        exit(1);
    }

    //child process
    if(0 == pid)
    {
        //write
        memcpy(addr,"12345",5);
    }
    //parent process
    else
    {
        wait(NULL);
        printf("addr:%s\n",(char*)addr);
    }


    munmap(addr,1024);
    return 0;
}