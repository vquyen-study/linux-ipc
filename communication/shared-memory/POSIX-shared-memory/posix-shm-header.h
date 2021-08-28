#ifndef __SYSV_SHM_HEADER_H__
#define __SYSV_SHM_HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>

// Buffer data structures
#define MAX_BUFFERS 10

#define LOGFILE                 "example.log"

#define SEM_MUTEX_NAME          "sem-mutex"
#define SEM_BUFFER_COUNT_NAME   "sem-buffer-count"
#define SEM_SPOOL_SIGNAL_NAME   "sem-spool-signal"
#define SHARED_MEM_NAME         "posix-shared-mem-example"

struct shared_memory {
    char buf [MAX_BUFFERS] [256];
    int buffer_index;
    int buffer_print_index;
};

static void error (char *msg)
{
    perror (msg);
    exit (1);
}
#endif // __SYSV-SHM-HEADER_H__