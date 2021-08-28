/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 55-1 */

#include <sys/file.h>
#include <fcntl.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */

void error(const char* fmt)
{
    perror(fmt);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int fd, lock;
    const char *lname;

    if (argc < 3 || strcmp(argv[1], "--help") == 0 ||
            strchr("sx", argv[2][0]) == NULL)
    {
        printf("%s <file> <lock> \n"
                 "    'lock' is 's' (shared) or 'x' (exclusive)\n"
                 "        optionally followed by 'n' (nonblocking)\n"
                 "    'sleep-time' specifies time to hold lock\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
    if (argv[2][1] == 'n')
        lock |= LOCK_NB;

    fd = open(argv[1], O_RDONLY);               /* Open file to be locked */
    if (fd == -1)
        error("open");

    lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";

    printf("PID %ld: requesting %s \n", (long) getpid(), lname);

    if (flock(fd, lock) == -1) {
        printf("PID %ld ", (long) getpid());
        if (errno == EWOULDBLOCK)
            error("process is already locked - bye!");
        else
            error("flock");
    }

    printf("PID %ld: granted    %s \n", (long) getpid(), lname);

    sleep(10);

    printf("PID %ld: releasing  %s\n", (long) getpid(), lname);
    if (flock(fd, LOCK_UN) == -1)
        error("flock");

    exit(EXIT_SUCCESS);
}
