#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>




static int loops = 0;
static void* produce_rasberry_pi(void *args)
{
    int i;
    printf("Hi\n");
    int RNG = rand() % (*(int *)args + 1);

    for(i = 0; i < loops; i ++)
    {
        //printf("%d\n", *(int *)args);
        printf("Created a Rasberry Pi in %d seconds\n", RNG);
        RNG = rand() % (*(int *)args + 1);
        //usleep(1000 * 1000);  
    }
    return NULL;
}
int main(int argc, char*argv[]) 
{
    loops = 10;
    printf("hi. We doing some coding\n");
    pthread_t producer_1, producer_2;

    srand(time(0));
    int p1_args = 3;
    pthread_create(&producer_1, NULL, produce_rasberry_pi, &p1_args);
    int p2_args = 5;
    pthread_create(&producer_2, NULL, produce_rasberry_pi, &p2_args);
    
    pthread_join(producer_1, NULL);
    pthread_join(producer_2, NULL);
    return 0;
}
