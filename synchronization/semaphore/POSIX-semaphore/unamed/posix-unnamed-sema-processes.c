
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


struct shared_resource 
{
    int glob;
    int loops;
    sem_t sem;
};


static void error(char *format)
{
    perror(format);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    struct shared_resource *p_res;
    int fd;

    fd = open("/dev/zero", O_RDWR);
    if (fd == -1)
        error("open");
    /**
     * 
     *  There are two way to get ANONYMOUS MAPPING below.
     *      - Adding flag MAP_ANONYMOUS to 'flag' field (We must define either the _BSD_SOURCE or the _SVID_SOURCE feature test macros
     *         to get the definition of MAP_ANONYMOUS from header <sys/mman.h>)
     *      - Open the /dev/zero device file and pass the resulting file descriptor to mmap()
     *  
     **/
    p_res = (struct shared_resource *) mmap(NULL, sizeof(struct shared_resource), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (((void *)p_res) == MAP_FAILED)
        error("mmap");

    if (close(fd) == -1)                /* No longer need 'fd' */
        error("close");

    /***
     * 
     * int sem_init(sem_t *sem, int pshared, unsigned int value);
     * 
     *  pshared : 0 means to be shared between the threads
     *  pshared : none-zero means the semaphore is to be shared between processes
     * 
     */
    p_res->loops = (argc == 2 ? atoi(argv[1]) : 10);
    if (sem_init(&p_res->sem, 1, 1) == -1)
        error("sem_init");

    if (fork() == 0)
    {
        int loops, count;
        if (sem_wait(&p_res->sem))
            error("sem_wait");
            
            loops = p_res->loops;

        if (sem_post(&p_res->sem))
            error("sem_wait");

        for (int i = 0; i < loops; i++)
        {
            if (sem_wait(&p_res->sem))
                error("sem_wait");
            
            count = p_res->glob;
            count++;
            p_res->glob = count;
            
            if (sem_post(&p_res->sem))
                error("sem_wait"); 

            printf("Running on pid %ld - count %d \n", getpid(), count);
            sleep(1);
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        int loops, count;
        if (sem_wait(&p_res->sem))
            error("sem_wait");
            
            loops = p_res->loops;

        if (sem_post(&p_res->sem))
            error("sem_wait");

        for (int i = 0; i < loops; i++)
        {
            if (sem_wait(&p_res->sem))
                error("sem_wait");
            
            count = p_res->glob;
            count++;
            p_res->glob = count;
            
            if (sem_post(&p_res->sem))
                error("sem_wait"); 

            printf("Running on pid %ld - count %d \n", getpid(), count);
            sleep(2);
        }

        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
