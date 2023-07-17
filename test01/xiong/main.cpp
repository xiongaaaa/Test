#include <stdio.h>

#include "add.h"
#include "sub.h"
#include "mul.h"
#include "dev.h"

int main()
{
    int x =5,y = 7;

    printf("x + y =%d\n",add(x,y));
    printf("x - y =%d\n",sub(x,y));
    printf("x * y =%d\n",mul(x,y));
    printf("x / y =%d\n",dev(x,y));
    


    return 0;
}