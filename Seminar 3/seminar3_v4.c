/* Group [123]-DE45: Peter Gabradilla, Sherwin Sathish, Yongqiang Wang
Create a producer/consumer problem for the a "Raspberry PI" structure.

you should use 2 producers
    Producer 1 creates a new "R_PI" at a random time in between 0s and 3 s
    Producer 2 creates a new "R_PI" at a random time in between 0s and 5 s
a new consumer (student) is trying to get/buy a "R_PI" every 1s
    each consumer will be in a sep thread
use a global list to hold each available "Raspberry PI"
    each "R_PI" will have: ID, next(for list implementation) ....
    you will have to use malloc for each new "R_PI"
    max size for the list = 20
*/
#define _GNU_SOURCE  
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int maxStock = 20;
static int timer = 0;
static int id = 0;
static int currentStock = 0;
static int done = 0;
static pthread_cond_t inStock = PTHREAD_COND_INITIALIZER;
static pthread_cond_t inventory = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static void* the_students(void *);  

typedef struct rasberryPi  
{
    int id;
    struct rasberryPi *next;
} rasberryPi_t;

rasberryPi_t *create_new_rasberryPi(int id)
{
    rasberryPi_t *result = (rasberryPi_t *)malloc(sizeof(rasberryPi_t));     
    result->id = id;
    result->next = NULL;
    return result;
};

static rasberryPi_t *stockList = NULL;  

static void* produce_rasberry_pi(void *args) 
{
    char name[16]; 
    int RNG = rand() % (*(int *)args + 1);


    int ret;
    for(; done < 100;)
    {
        ret = pthread_mutex_lock(&mtx); 
        if(ret != 0) return NULL;
        while (currentStock == maxStock)
        { 
            printf("Stockroom is full\n");
            ret = pthread_cond_wait(&inventory, &mtx); 
            if(ret != 0) return NULL;
        }

        if (currentStock < maxStock)
        {
            rasberryPi_t *newRasberryPi =  create_new_rasberryPi(id);
            currentStock++;
            newRasberryPi->next = stockList;  
            pthread_getname_np(pthread_self(), name, 16);
            printf("[Time %d] %s Created Rasberry Pi # %d. New creation in %d seconds, Current Stock: %d\n", timer, name, id, RNG, currentStock);
            stockList = newRasberryPi;  
            id++;
        }
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0) return NULL;

        pthread_cond_signal(&inStock);
        if(ret != 0) return NULL;

        sleep(RNG);  
        RNG = rand() % (*(int *)args + 1);  
    }

    return NULL;
}


static void* time_manager(void *args)
{
    for (;done < 100 ;)
    {
        timer++;
        printf("\n--------------------[Time %d]--------------------\n", timer);
        sleep(1);
    }
    return NULL;
}


static void* the_students(void *args)
{
    int ret;
    char name[16];
    pthread_getname_np(pthread_self(), name, 16);       
    for (;done < 100 ;){
        ret = pthread_mutex_lock(&mtx);
        if(ret != 0) return NULL;
        while (currentStock == 0){ 
            ret = pthread_cond_wait(&inStock, &mtx); 
            if(ret != 0) return NULL;
        }
        if (currentStock > 0){
            --currentStock;
            printf("[Time %d] Student %s bought a Rasberry Pi, Current Stock: %d\n",timer, name, currentStock);
        }
        ret = pthread_mutex_unlock(&mtx); 
        if(ret != 0) return NULL;
        pthread_cond_signal(&inventory); 
        if(ret != 0) return NULL;
        pthread_exit(NULL); 
    }
    return NULL;
}


int main(int argc, char*argv[]) 
{
    printf("We're Making Rasberry Pi!\n\n");
    srand(time(0)); 

    pthread_t producer_1, producer_2, time_manage;  
    int max_number_of_students = 100;
    pthread_t student_number[max_number_of_students];  
    char number[50];
    int p1_args = 3; 
    int p2_args = 5;
    pthread_create(&producer_1, NULL, produce_rasberry_pi, &p1_args);
    pthread_setname_np(producer_1, "Producer 1");
    pthread_create(&producer_2, NULL, produce_rasberry_pi, &p2_args);
    pthread_setname_np(producer_2, "Producer 2");
    pthread_create(&time_manage, NULL, time_manager, 0);
    
    for(int i = 0; i < max_number_of_students; i++)
    {
        pthread_create(&student_number[i], NULL, the_students, NULL); 
        printf("[Time %d] Student %d arrived\n",timer, i);
        sprintf(number, "%d", i);  
        pthread_setname_np(student_number[i], number);
        sleep(1);
    }

    pthread_join(producer_1, NULL);
    pthread_join(producer_2, NULL);
    pthread_join(time_manage, NULL);
    for(int i = 0; i < max_number_of_students; i++)
    {
        pthread_join(student_number[i], NULL);
    }
    
    return 0;
}
