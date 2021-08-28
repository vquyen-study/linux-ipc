#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  Example description :  
 *      - parent will create numbers(argc - 1) of children process. 
 *      - children process will send message attached PID back to parent process.
 *      - each process send N(user-input) message child process will close pipe[1]
 *      - Parent will wait all child finish to shutdown program.
 *          
 *  Technique: 
 *      - child process sends N message will call close(pipe[1]).
 *      - Parent process read(pipe[0]) != 0 (0 : means all pipe[1] closed). then shutdown program.
 */
int main(int argc, char **argv)
{
    int pipe_fd[2];
    int j, dummy;
    char buff[100] = {0};

    pid_t pid;
    char msg[100] = {0};
    int sleep_time ;
    int count;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s number-msg ...\n", argv[0]);
        return 0;
    }

    if (pipe(pipe_fd) == -1)
    {
        printf("[Err] Could not create pipe\n");
        return 0;
    }

    printf("Parent process is running... pid(%d)\n", getpid());
    for (j = 1; j < argc; j++)
    {
        switch(fork())
        {
            case -1:
                printf("[Err] Fail to fork new process.\n");
                exit(0);
                break;
            case 0:                 /* Child process handling. */
            {
                pid_t pid = getpid();
                char msg[100] = {0};
                int count = atoi(argv[j]);

                printf("Child process is running... pid(%ld)\n", (long int) pid);
                snprintf(msg, 100, "Message come from pid %d", pid);
                
                close(pipe_fd[0]);  /* Close on read pip descriptor. */
                
                while (count-- > 0)
                {
                    sleep(1);
                    write(pipe_fd[1], msg, 100);
                }

                printf("Child process is going to shutdown... pid(%ld)\n", (long int) pid);
                while(1) {sleep(1); };
                close(pipe_fd[1]);  /* Close on write descriptor    */
                
                exit(EXIT_SUCCESS);
            }
                break;
            default:                 /* Parent process handling. */
                
                break;
        }
    }

    close(pipe_fd[1]);      /* close on write pipe. */
  
    while (read(pipe_fd[0], buff, 100) != 0)     /* 0: means end-of-file for closed pipe[1] */
    {
        printf("[Parent] rx: %s\n", buff);
    }

    printf("Ready to end parent process...\n");
    return 0;
}