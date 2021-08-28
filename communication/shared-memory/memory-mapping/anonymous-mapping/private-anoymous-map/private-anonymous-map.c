
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
#define _BSD_SOURCE     1
#include <sys/mman.h>


#define MEM_SIZE 10

static void error(char *format)
{
    perror(format);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    char *addr;
    int fd;

    /**
     * 
     *  There are two way to get ANONYMOUS MAPPING below.
     *      - Adding flag MAP_ANONYMOUS to 'flag' field (We must define either the _BSD_SOURCE or the _SVID_SOURCE feature test macros
     *         to get the definition of MAP_ANONYMOUS from <sys/mman.h>)
     *      - Open the /dev/zero device file and pass the resulting file descriptor to mmap()
     *  
     **/
    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        error("mmap");

    if (close(fd) == -1)                /* No longer need 'fd' */
        error("close");

    snprintf(addr, MEM_SIZE, argc > 1 ? argv[1] : "AAAAAAAAAAAAAAAAAAA");
    printf("Main string=%.*s\n", MEM_SIZE, addr);
                        /* Secure practice: output at most MEM_SIZE bytes */

    if (fork() == 0)
    {
        printf("Child base string %.*s\n", MEM_SIZE, addr);
        while (1)
        {
            addr[MEM_SIZE/2] = (addr[MEM_SIZE/2] + 1) % 'z';
            printf("Child \t\t%.*s\n", MEM_SIZE, addr);
            if (msync(addr, MEM_SIZE, MS_SYNC) == -1)
                error("msync");
            sleep(1);
        }
    }
    else
    {
        printf("Parent base string %.*s\n", MEM_SIZE, addr);
        while (1)
        {
            printf("Parent \t\t\t\t%.*s\n", MEM_SIZE, addr);
            sleep(1);
        }

        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
