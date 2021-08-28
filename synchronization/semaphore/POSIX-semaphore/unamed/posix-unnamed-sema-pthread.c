
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>

struct shared_resource 
{
    int glob;
    int loops;
    sem_t sem;
};

struct shared_resource g_res = {0};

static void error(const char* fmt)
{
    perror(fmt);
    exit(EXIT_FAILURE);
}

static void *thread_loop(void *arg)
{
    struct shared_resource *p_res = (struct shared_resource *) arg;
    int loops = p_res->loops;
    int loc, j;

    for (j = 0; j < loops; j++) {
        printf("running on pthread_id %ld ...\n", pthread_self());
        
        if (sem_wait(&p_res->sem) == -1)
            error("sem_wait");

        loc = p_res->glob;
        loc++;
        p_res->glob = loc;

        if (sem_post(&p_res->sem) == -1)
            error("sem_post");

        sleep(1);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int s;

    g_res.loops = (argc == 2 ? atoi(argv[1]) : 10);
    /* Initialize a semaphore with the value 1 */
    
    /***
     * 
     * int sem_init(sem_t *sem, int pshared, unsigned int value);
     * 
     *  pshared : 0 means to be shared between the threads
     *  pshared : none-zero means the semaphore is to be shared between processes
     * 
     */
    if (sem_init(&g_res.sem, 0, 1) == -1)
        error("sem_init");

    /* Create two threads that increment 'glob' */

    s = pthread_create(&t1, NULL, thread_loop, &g_res);
    if (s != 0)
        error("pthread_create");
    s = pthread_create(&t2, NULL, thread_loop, &g_res);
    if (s != 0)
        error("pthread_create");

    /* Wait for threads to terminate */
    s = pthread_join(t1, NULL);
    if (s != 0)
        error("pthread_join");
    s = pthread_join(t2, NULL);
    if (s != 0)
        error("pthread_join");

    if (sem_destroy(&g_res.sem))
        perror("sem_destroy");

    printf("glob = %d\n", g_res.glob);
    exit(EXIT_SUCCESS);
}
