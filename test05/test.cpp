#include <iostream>
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"


int main()
{
    queue_t *Q = queue_create(50);
    cout << queue_is_full(Q)<<endl;
    bool a = queue_push_tail(Q,(void*)"xiong");
    cout <<a <<endl;
    queue_push_tail(Q,(void*)"bo");
    queue_push_tail(Q,(void*)"da");
    queue_push_tail(Q,(void*)"9999");
    

    cout << Q->capacity<<endl;
    cout << Q->size<<endl;

    char *str = (char*)queue_pop_head(Q);
    cout<<str<<endl;
    str = (char*)queue_pop_head(Q);
    cout<<str<<endl;
    str = (char*)queue_pop_head(Q);
    cout<<str<<endl;

    char *q = (char*)queue_pop_head(Q);
    int sz = atoi(q);
    cout<<sz<<endl;



    return 0;
}