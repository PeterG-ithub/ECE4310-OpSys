#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

static pthread_mutex_t store = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int ready = 0;
static int done = 0;

static void* produce_rasberry_pi(void *args) // args blank pointer we will type cast it 
{
    int ret;
    for (;done < 100 ;){
        ret = pthread_mutex_lock(&store); // lock after access done (globle var) is ok since it is not modifying it 
        if(ret != 0) return NULL;
        ++ready;  
        printf("+");
        ret = pthread_mutex_unlock(&store);
        if(ret != 0) return NULL;
        pthread_cond_signal(&cond);
        if(ret != 0) return NULL;
        usleep(1000 * 100); 
    }
    return NULL;
}
static void* thread_consume(void *args){
    int ret;
    printf("%d\n", atoi(args));
    for (;done < 100 ;){
        ret = pthread_mutex_lock(&store);
        if(ret != 0) return NULL;
        while (ready == 0){ // while list size is 0 goes into wait status

            // cond is q to move out of wait status
            // &store to unlock mutex after thread goes into wait status and lock it when it is out of wait
            ret = pthread_cond_wait(&cond, &store); 
            if(ret != 0) return NULL;
        }
        while (ready > 0){
            --ready;
            ++done;
            printf("-");
        }
        ret = pthread_mutex_unlock(&store);
        if(ret != 0) return NULL;
    }
    return NULL;
}
int main(int argc, char*argv[]) // argc: argument count and argv: argument verctor
{
    pthread_t producer_1, producer_2, consumer; // create new variable in stack
    int loops, ret;
    loops = (argc > 1) ? atoi(argv[1]) : 1000000; // argv[0] is the name of the file

    printf("%d\n", loops);

    ret = pthread_create(&consumer, NULL, thread_consume, &loops);
    ret = pthread_create(&producer_1, NULL, produce_rasberry_pi, &loops);

    pthread_join(consumer, NULL);

    done =200;
    pthread_join(producer_1, NULL);

    return 0;
}
