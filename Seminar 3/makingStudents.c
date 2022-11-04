#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
// static void* creating_student_consumer(void *args)
// {

// }
static int timer = 0;
int main(int argc, char*argv[]) 
{

    // pthread_t student_consumer;
    int msec = 0, delay = 1000; 
    clock_t before = clock();
    while(timer< 10)
    {
        
        while(msec < delay)
        {
            clock_t difference = clock() - before;
            msec = difference * 1000 / CLOCKS_PER_SEC;
        }
        msec = 0;
        before = clock();
        printf("[Time %d]: Creating Student.\n", timer);
        timer++;
    }
    return 0;
}