#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>

int main()
{ 
    int ret = -1;
    pid_t pid = -1;

    time_t t = -1;
    char file_name[64];
    struct tm* pT = NULL;
    
    pid = fork();
    if(-1 == pid)
    {
        perror("fork");
        exit(1);
    }
    if(pid > 0)
    {
        exit(0);
    }

    pid = setsid();
    if(-1 == pid)
    {
        perror("setsid");
        exit(1);
    }

    ret = chdir("/");
    if(ret == -1)
    {
        perror("chdir");
        exit(1);
    }

    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while(1)
    {
        t = time(NULL);

        pT = localtime(&t);
        if(NULL == pT)
        {
            printf("localtime failed...\n");
            return 1;
        }

        memset(file_name,0,64);
        sprintf(file_name,"%s%d%d%d%d%d%d.log","touch /home/parallels/Linux/log/",pT->tm_year+1900,pT->tm_mon +1,pT->tm_mday,pT->tm_hour,pT->tm_min,pT->tm_sec);

        system(file_name);
        sleep(1);
    }

    return 0;
}
