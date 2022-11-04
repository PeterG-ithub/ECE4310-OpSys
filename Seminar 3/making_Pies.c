#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int maxStock = 20;
int currentStock = 0;
typedef struct rasberryPi
{
    int id;
    int stock;
    struct rasberryPi *next;
} rasberryPi_t;

rasberryPi_t *create_new_rasberryPi(int id, int currentStock)
{
    rasberryPi_t *result = (rasberryPi_t *)malloc(sizeof(rasberryPi_t));     
    result->id = id;
    result->stock = currentStock;
    result->next = NULL;
    return result;
};

static int id = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
//Producing Threads
static void* produce_rasberry_pi(void *args)
{
    //static int id = 0;
    char name[16];
    int RNG = rand() % (*(int *)args + 1);
    rasberryPi_t *stockList = NULL;

    int ret;
    for(; currentStock < maxStock;)
    {
        ret = pthread_mutex_lock(&mtx);

        if(ret != 0) return NULL;
        rasberryPi_t *newRasberryPi =  create_new_rasberryPi(id, currentStock);
        newRasberryPi->stock++;
        currentStock = newRasberryPi->stock;
        newRasberryPi->next = stockList;
        pthread_getname_np(pthread_self(), name, 16);
        printf("%s Created Rasberry Pi # %d in %d seconds, Current Stock: %d\n", name, id, RNG, newRasberryPi->stock);
        stockList = newRasberryPi;
        id++;
        
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0) return NULL;

        usleep(1000*1000*RNG);
        RNG = rand() % (*(int *)args + 1);
    }

    return NULL;
}


int main(int argc, char*argv[]) 
{
    printf("Were Making PIES!\n");
    srand(time(0));
    // rasberryPi_t *stockList = NULL;
    // for(int i = 0; i < 20; i++)
    // {
    //     rasberryPi_t *newRasberryPi =  create_new_rasberryPi(i);
    //     newRasberryPi->next = stockList;
    //     printf("Created Rasberry Pi # %d\n", i);
    //     stockList = newRasberryPi;
    // }
    

    pthread_t producer_1, producer_2;
    int p1_args = 3;
    pthread_create(&producer_1, NULL, produce_rasberry_pi, &p1_args);
    pthread_setname_np(producer_1, "Producer 1");

    int p2_args = 5;
    pthread_create(&producer_2, NULL, produce_rasberry_pi, &p2_args);
    pthread_setname_np(producer_2, "Producer 2");

    pthread_join(producer_1, NULL);
    pthread_join(producer_2, NULL);


    return 0;
}