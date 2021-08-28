/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 30 */

#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int avail = 0;

static void error(const char* fmt)
{
    perror(fmt);
    exit(EXIT_FAILURE);
}

static void *producer(void *arg)
{
    int cnt = atoi((char *) arg);

    for (int j = 0; j < cnt; j++) {
        sleep(1);

        /* Code to produce a unit omitted */

        int s = pthread_mutex_lock(&mtx);
        if (s != 0)
            error("pthread_mutex_lock");

        avail++;        /* Let consumer know another unit is available */

        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
            error("pthread_mutex_unlock");

        s = pthread_cond_signal(&cond);         /* Wake sleeping consumer */
        if (s != 0)
            error("pthread_cond_signal");
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    time_t t = time(NULL);

    int totRequired = 0;        /* Total number of units that all threads
                                   will produce */
    /* Create all threads */
    if (argc < 2)
        error("./cond-variable <number1> <number2> ....");

    for (int j = 1; j < argc; j++) {
        totRequired += atoi(argv[j]);

        pthread_t tid;
        int s = pthread_create(&tid, NULL, producer, argv[j]);
        if (s != 0)
            error("pthread_create");
    }

    /* Loop to consume available units */

    int numConsumed = 0;        /* Total units so far consumed */
    bool done = false;

    for (;;) {
        int s = pthread_mutex_lock(&mtx);
        if (s != 0)
            error("pthread_mutex_lock");

        /**
         * 
         * For example, in the code segment in the preceding section, the predicate associated with cond 
         * is (avail == 0). This code segment demonstrates a general design principle: a pthread_cond_wait() 
         * call must be governed by a while loop rather than an if statement. This is so because, on return 
         * from pthread_cond_wait(), there are no guarantees about the state of the predicate; therefore, we
         * should immediately recheck the predicate and resume sleeping if it is not in the desired state.
         * 
         */
        while (avail == 0) {                        /* Wait for something to consume */
            s = pthread_cond_wait(&cond, &mtx);     /* the function will unlock mtx and go to sleep, until cond is signaled, then lock mtx do the rest work.*/
            if (s != 0)
                error("pthread_cond_wait");
        }

        /* At this point, 'mtx' is locked... */

        while (avail > 0) {             /* Consume all available units */

            /* Do something with produced unit */

            numConsumed ++;
            avail--;
            printf("T=%ld: numConsumed=%d\n", (long) (time(NULL) - t),
                    numConsumed);

            done = numConsumed >= totRequired;
        }

        s = pthread_mutex_unlock(&mtx);
        if (s != 0)
            error("pthread_mutex_unlock");

        if (done)
            break;

        /* Perhaps do other work here that does not require mutex lock */

    }

    exit(EXIT_SUCCESS);
}
