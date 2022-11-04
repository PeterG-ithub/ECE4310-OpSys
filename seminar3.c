#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int ready = 0;
static int done = 0;

static void* thread_produce(void *args) // args blank pointer we will type cast it 
{
    int ret;
    for (;done < 100 ;){
        ret = pthread_mutex_lock(&mtx); // lock after access done (globle var) is ok since it is not modifying it 
        if(ret != 0) return NULL;
        ++ready;  
        printf("+");
        ret = pthread_mutex_unlock(&mtx);
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
        ret = pthread_mutex_lock(&mtx);
        if(ret != 0) return NULL;
        while (ready == 0){ // while list size is 0 goes into wait status

            // cond is q to move out of wait status
            // &mtx to unlock mutex after thread goes into wait status and lock it when it is out of wait
            ret = pthread_cond_wait(&cond, &mtx); 
            if(ret != 0) return NULL;
        }
        while (ready > 0){
            --ready;
            ++done;
            printf("-");
        }
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0) return NULL;
    }
    return NULL;
}
int main(int argc, char*argv[]) // argc: argument count and argv: argument verctor
{
    pthread_t t1, t2; // create new variable in stack
    int loops, ret;
    loops = (argc > 1) ? atoi(argv[1]) : 1000000; // argv[0] is the name of the file

    printf("%d\n", loops);

    ret = pthread_create(&t1, NULL, thread_consume, &loops);
    ret = pthread_create(&t2, NULL, thread_produce, &loops);

    pthread_join(t1, NULL);

    done =200;
    pthread_join(t2, NULL);

    return 0;
}