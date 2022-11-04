#define _GNU_SOURCE  // pthread_setname_np, pthread_getname_np
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int maxStock = 20;
static int timer = 0;
static int currentStock = 0;
static int done = 0;
static pthread_cond_t inStock = PTHREAD_COND_INITIALIZER;
static pthread_cond_t inventory = PTHREAD_COND_INITIALIZER;
static void* the_students(void *);  //prototype

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

static int id = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
//Producing Threads
static void* produce_rasberry_pi(void *args) 
{
    char name[16]; // char with 16 characters
    int RNG = rand() % (*(int *)args + 1);
    rasberryPi_t *stockList = NULL;  // firstnode of the list

    int ret;
    for(; done < 100;)
    {
        ret = pthread_mutex_lock(&mtx); // return NULL when error occurs, ret will become 0 when success
        if(ret != 0) return NULL;
        while (currentStock == maxStock)
        { 
            ret = pthread_cond_wait(&inventory, &mtx); 
            if(ret != 0) return NULL;
        }

        if (currentStock < maxStock)
        {
            rasberryPi_t *newRasberryPi =  create_new_rasberryPi(id);
            currentStock++;
            newRasberryPi->next = stockList;  // add to the begining of the list
            pthread_getname_np(pthread_self(), name, 16);
            printf("[Time %d] %s Created Rasberry Pi # %d. New creation in %d seconds, Current Stock: %d\n", timer, name, id, RNG, currentStock);
            stockList = newRasberryPi;  // update the firstnode of the list to the new node
            id++;
        }
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0) return NULL;

        pthread_cond_signal(&inStock);
        if(ret != 0) return NULL;

        usleep(1000*1000*RNG);  // sleep in RNG seconds
        RNG = rand() % (*(int *)args + 1);  // get new RNG every round
    }

    return NULL;
}



static void* make_students(void *args)
{

    pthread_t student_number[100];  // upto 100 students
    char number[50];
    for(int i = 0; i < *(int *)args; i++)
    {
        printf("[Time %d] Creating student %d\n",timer, i);
        pthread_create(&student_number[i], NULL, the_students, NULL); // NULL could be anything since not being used
        sprintf(number, "%d", i);  // sprintf convert integer to string
        pthread_setname_np(student_number[i], number);
        usleep(1000000);  // wait for 1 second
        timer++;
    }
    
    return NULL;
}

// cosumer function
static void* the_students(void *args)
{
    int ret;
    char name[16];
    for (;done < 100 ;){
        ret = pthread_mutex_lock(&mtx);
        if(ret != 0) return NULL;
        while (currentStock == 0){ 
            ret = pthread_cond_wait(&inStock, &mtx); 
            if(ret != 0) return NULL;
        }
        if (currentStock > 0){
            --currentStock;
            pthread_getname_np(pthread_self(), name, 16);       // pthread_self() function returns the ID of the calling thread
            printf("[Time %d] Student %s bought the thing, Current Stock: %d\n",timer, name, currentStock);
        }
        ret = pthread_mutex_unlock(&mtx); 
        if(ret != 0) return NULL;
        pthread_cond_signal(&inventory);
        if(ret != 0) return NULL;
                
        pthread_exit(0); 
    }
    return NULL;
}


int main(int argc, char*argv[]) 
{
    printf("Were Making PIES!\n\n");
    srand(time(0)); // use time as random number seed, time(0) gives the time in second

    pthread_t producer_1, producer_2, student_maker;  // variable for thread in stack
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

    return 0;
}