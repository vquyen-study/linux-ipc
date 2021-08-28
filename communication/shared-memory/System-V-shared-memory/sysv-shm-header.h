#ifndef __SYSV_SHM_HEADER_H__
#define __SYSV_SHM_HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>

// Buffer data structures
#define MAX_BUFFERS 10

#define PROJECT_KEY                 "ftok.key"
#define PRJ_MUTEX_ID                'A'
#define PRJ_BUFF_CNT_ID             'B'
#define PRJ_SPOLL_SIG_ID            'C'
#define PRJ_SHM_ID                  'D'

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