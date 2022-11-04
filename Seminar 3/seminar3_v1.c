#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
int maxStock = 20;
int currentStock = 0;
static pthread_cond_t inStock = PTHREAD_COND_INITIALIZER;
static void* stock_notification(void *);

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
        printf("%s Created Rasberry Pi # %d. New creation in %d seconds, Current Stock: %d\n", name, id, RNG, newRasberryPi->stock);
        stockList = newRasberryPi;
        id++;
        
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0) return NULL;

        pthread_cond_signal(&inStock);
        if(ret != 0) return NULL;

        usleep(1000*1000*RNG);
        RNG = rand() % (*(int *)args + 1);
    }

    return NULL;
}



static void* make_students(void *args)
{
    //int ret;
    pthread_t student_number[100];
    char name[16] = "Student ";
    char number[50];
    for(int i = 0; i < *(int *)args; i ++)
    {
        printf("Creating student %d\n", i);
        pthread_create(&student_number[i], NULL, stock_notification, 0);
        sprintf(number, "%d", i);
        strcat(name, number);
        pthread_setname_np(student_number[i], name);
        usleep(1000000);
    }
    
    return NULL;
}


static void* stock_notification(void *args)
{
    int ret;
    char name[16];
    int done = 0;
    for (;done < 100 ;){
        ret = pthread_mutex_lock(&mtx);
        if(ret != 0) return NULL;
        while (currentStock == 0){ 
            ret = pthread_cond_wait(&inStock, &mtx); 
            if(ret != 0) return NULL;
        }
        if (currentStock > 0){
            --currentStock;
            pthread_getname_np(pthread_self(), name, 16);
            printf("%s bought the thing, Current Stock: %d\n", name, currentStock);
        }
        ret = pthread_mutex_unlock(&mtx);
        pthread_exit(0);
        if(ret != 0) return NULL;
    }
    return NULL;
}


int main(int argc, char*argv[]) 
{
    printf("Were Making PIES!\n");
    srand(time(0));

    pthread_t producer_1, producer_2, student_maker;
    int max_number_of_students = 100;
    pthread_create(&student_maker, NULL, make_students, &max_number_of_students);
    
    int p1_args = 3;
    pthread_create(&producer_1, NULL, produce_rasberry_pi, &p1_args);
    pthread_setname_np(producer_1, "Producer 1");

    int p2_args = 5;
    pthread_create(&producer_2, NULL, produce_rasberry_pi, &p2_args);
    pthread_setname_np(producer_2, "Producer 2");



    pthread_join(student_maker, NULL);
    pthread_join(producer_1, NULL);
    pthread_join(producer_2, NULL);


    // int msec = 0, delay = 1000; 
    // clock_t before = clock();
    // while(timer< 10)
    // {
        
    //     while(msec < delay)
    //     {
    //         clock_t difference = clock() - before;
    //         msec = difference * 1000 / CLOCKS_PER_SEC;
    //     }
    //     msec = 0;
    //     before = clock();
    //     printf("[Time %d]: Creating Student.\n", timer);
    //     timer++;
    // }

    return 0;
}