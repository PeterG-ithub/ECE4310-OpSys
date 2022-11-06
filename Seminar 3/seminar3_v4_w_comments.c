#define _GNU_SOURCE     // pthread_setname_np, pthread_getname_np
#include <stdio.h>      // include the input/output related functions in our program ex: PRINTF or SCANF or SPRINTF
#include <stdlib.h>     // where MALLOC comes from, includes functions involving memory allocation, process control, conversions 
#include <pthread.h>    // function declarations and mappings for threading interfaces and defines a number of constants used by those functions
#include <unistd.h>     // needed for SLEEP. defines miscellaneous symbolic constants and types, and declares miscellaneous functions, provides access to API

int maxStock = 20;      //
static int timer = 0;   // Global timer to track the time
static int id = 0;      // Global ID for the rasberry PI ID
static int currentStock = 0;        // Current stock of Rasberry Pi
static int done = 0;        // On and Off switch for Threads?
static pthread_cond_t inStock = PTHREAD_COND_INITIALIZER;   // Condition to check if theres Pi's In stock
static pthread_cond_t inventory = PTHREAD_COND_INITIALIZER; // Condition to check if stock full
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;     // Flag
static void* the_students(void *);  //prototype

typedef struct rasberryPi       // Stucture of the RasberryPi
{
    int id;                     // Id of rasberry pi
    struct rasberryPi *next;    // 
} rasberryPi_t;                 // it is a data type like int x or resberrypi_t x

rasberryPi_t *create_new_rasberryPi(int id)     //
{
    rasberryPi_t *result = (rasberryPi_t *)malloc(sizeof(rasberryPi_t));    //
    result->id = id;                    //
    result->next = NULL;                //
    return result;                      //
};

static rasberryPi_t *stockList = NULL;  // firstnode of the list, a static int variable remains in memory while the program is running
//Producing Threads 
static void* produce_rasberry_pi(void *args)    //
{
    char name[16];                          // char with 16 characters
    int RNG = rand() % (*(int *)args + 1);  // generate random number name it as RNG


    int ret;                        //
    for(; done < 100;)              //
    {
        ret = pthread_mutex_lock(&mtx); // return NULL when error occurs, ret will become 0 when success
        if(ret != 0) return NULL;           //
        while (currentStock == maxStock)    //  if stockroom is full
        { 
            printf("Stockroom is full\n");  // 
            ret = pthread_cond_wait(&inventory, &mtx);  // wait until not full using condition inventory
            if(ret != 0) return NULL;       // 
        }

        if (currentStock < maxStock)        // if current stock is jnot max
        {
            rasberryPi_t *newRasberryPi =  create_new_rasberryPi(id);   //making new Pi
            currentStock++;                     //
            newRasberryPi->next = stockList;    // add to the begining of the list
            pthread_getname_np(pthread_self(), name, 16);   //
            printf("[Time %d] %s Created Rasberry Pi # %d. New creation in %d seconds, Current Stock: %d\n", timer, name, id, RNG, currentStock);   //
            stockList = newRasberryPi;  // update the firstnode of the list to the new node
            id++;                       //
        }
        ret = pthread_mutex_unlock(&mtx);   // unlock the flag
        if(ret != 0) return NULL;           //

        pthread_cond_signal(&inStock);      //
        if(ret != 0) return NULL;           //

        sleep(RNG);                         // sleep in RNG seconds
        RNG = rand() % (*(int *)args + 1);  // get new RNG every round
    }

    return NULL;                            //
}


static void* time_manager(void *args)       //
{
    for (;done < 100 ;)                     //
    {
        timer++;                            // increment timer
        printf("\n--------------------[Time %d]--------------------\n", timer); //
        sleep(1);                           // sleep for 1 seconds
    }
    return NULL;                            //
}

// cosumer function
static void* the_students(void *args)       //
{
    int ret;                                //
    char name[16];                          //
    pthread_getname_np(pthread_self(), name, 16);       // pthread_self() function returns the ID of the calling thread
    for (;done < 100 ;){                    //
        ret = pthread_mutex_lock(&mtx);     // locks the store
        if(ret != 0) return NULL;           // 
        while (currentStock == 0){          // 
            ret = pthread_cond_wait(&inStock, &mtx);    //
            if(ret != 0) return NULL;       //
        }
        if (currentStock > 0){              // 
            --currentStock;                 // decrease the current stock
            printf("[Time %d] Student %s bought a Rasberry Pi, Current Stock: %d\n",timer, name, currentStock);     //
        }
        ret = pthread_mutex_unlock(&mtx);   // unlocks the store
        if(ret != 0) return NULL;           //
        pthread_cond_signal(&inventory);    // inventory no longer full
        if(ret != 0) return NULL;           //
        pthread_exit(NULL);                 // terminates the calling thread, returns NULL to pthread_join
    }                                       // exit the thread that calls it, it will keep going back to top               
    return NULL;
}
// 
// this is main
int main(int argc, char*argv[]) 
{
    printf("We're Making Rasberry Pi!\n\n");
    srand(time(0)); // use time as random number seed, time(0) gives the time in second

    pthread_t producer_1, producer_2, time_manage;  // variable for thread in stack
    int max_number_of_students = 100;   // maximum number of student
    pthread_t student_number[max_number_of_students];      // upto 100 students
    char number[50];                    //
    int p1_args = 3;                    //  argument for produce_rasberry_pi
    int p2_args = 5;                    //  argument for 
    pthread_create(&producer_1, NULL, produce_rasberry_pi, &p1_args);       //
    pthread_setname_np(producer_1, "Producer 1");                           //
    pthread_create(&producer_2, NULL, produce_rasberry_pi, &p2_args);       //
    pthread_setname_np(producer_2, "Producer 2");                           //
    pthread_create(&time_manage, NULL, time_manager, 0);                    //
    
    for(int i = 0; i < max_number_of_students; i++)                         //
    {
        pthread_create(&student_number[i], NULL, the_students, NULL);       // NULL could be anything since not being used
        printf("[Time %d] Student %d arrived\n",timer, i);                  //
        sprintf(number, "%d", i);                                           // sprintf convert integer to string in 
        pthread_setname_np(student_number[i], number);                      //
        sleep(1);       //
    }
 
    pthread_join(producer_1, NULL);                 //
    pthread_join(producer_2, NULL);                 //
    pthread_join(time_manage, NULL);                //
    for(int i = 0; i < max_number_of_students; i++) //
    {
        pthread_join(student_number[i], NULL);      //
    }
    //
    return 0;           
}
