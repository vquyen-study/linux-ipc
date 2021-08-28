#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
#include<signal.h>


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

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        error("./binary file\n");

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        error("open");

    // addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
        error("mmap");

    if (close(fd) == -1)                /* No longer need 'fd' */
        error("close");

    printf("Current string=%.*s\n", MEM_SIZE, addr);
                        /* Secure practice: output at most MEM_SIZE bytes */

    if (fork() == 0)
    {
        printf("Child string=%.*s\n", MEM_SIZE, addr);
        while (1)
        {
            addr[MEM_SIZE/2] = (addr[MEM_SIZE/2] + 1) % 'z';
            printf("Child string=%.*s\n", MEM_SIZE, addr);
            if (msync(addr, MEM_SIZE, MS_SYNC) == -1)
                error("msync");
            sleep(1);
        }
    }
    else
    {
        printf("Parent string=%.*s\n", MEM_SIZE, addr);
        while (1)
        {
            printf("Parent string=%.*s\n", MEM_SIZE, addr);
            sleep(1);
        }

        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
