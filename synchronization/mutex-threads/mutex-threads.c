#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef enum
{
    INITIALIZE,
    INPUT_STATE,
    DATA_AVAILABE_STATE,
    DATA_HANDLED_STATE,
} enum_states;

struct shared_resource
{
    pthread_mutex_t mutex;
    int  counter;
    enum_states state;
    int a;
    int b;
    int sum;
};

struct shared_resource g_resource = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .counter = 0,
    .state = INITIALIZE,
    .a = 0,
    .b = 0,
    .sum = 0
};

void *input_data_routine(void *args);
void *handle_data_routine(void *args);
void *show_data_routine(void *args);


int main(int argc, char **argv)
{
   int rc1, rc2;
   pthread_t thread1, thread2, thread3;

   /* Create independent threads each of which will execute functionC */
   if ((rc1=pthread_create( &thread1, NULL, &input_data_routine, &g_resource)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }

   if( (rc2=pthread_create( &thread2, NULL, &handle_data_routine, &g_resource)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   if( (rc2=pthread_create( &thread3, NULL, &show_data_routine, &g_resource)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   printf("Threads are running...\n");

   /* Wait till threads are complete before main continues. Unless we  */
   /* wait we run the risk of executing an exit which will terminate   */
   /* the process and all threads before the threads have completed.   */

   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL); 
   pthread_join( thread3, NULL); 

   exit(0);
}

void *input_data_routine(void *args)
{
    struct shared_resource *res = (struct shared_resource *) args;
    int count = 1;
    while (1)
    {    
        pthread_mutex_lock(&res->mutex);
        if (res->state == INITIALIZE || res->state == INPUT_STATE)
        {
            res->a = 10*count;
            res->b = 20*count;

            printf("input: \t\t\t%d   %d\n", res->a, res->b);
            count++;
            res->state = DATA_AVAILABE_STATE;
        }
        pthread_mutex_unlock(&res->mutex);
        sleep(1);
    }
}

void *handle_data_routine(void *args)
{
    struct shared_resource *res = (struct shared_resource *) args;
    while (1)
    {  
        pthread_mutex_lock(&res->mutex);
        if (res->state == DATA_AVAILABE_STATE)
        {
            res->sum = res->a + res->b;
            res->state = DATA_HANDLED_STATE;
        }
        pthread_mutex_unlock(&res->mutex);
    }
}

void *show_data_routine(void *args)
{
    struct shared_resource *res = (struct shared_resource *) args;
    while (1)
    {
        pthread_mutex_lock(&res->mutex);
        if (res->state == DATA_HANDLED_STATE)
        {
            printf("Result: \t\t%d  \n\n", res->sum);
            res->state = INPUT_STATE;
        }
        pthread_mutex_unlock(&res->mutex);
    }
}