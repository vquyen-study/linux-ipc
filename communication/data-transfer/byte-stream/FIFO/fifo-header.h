#ifndef __IPC_FIFO_HEADER_H__
#define __IPC_FIFO_HEADER_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_FIFO "/tmp/echo-server"
                                /* Well-known name for server's FIFO */
#define CLIENT_FIFO_TEMPLATE "/tmp/client-fifo.%ld"
                                /* Template for building client FIFO name */
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
                                /* Space required for client FIFO pathname
                                  (+20 as a generous allowance for the PID) */

struct request {                /* Request (client --> server) */
    pid_t pid;                  /* PID of client */
    int seq_num;                 /* Length of desired sequence */
};

struct response {               /* Response (server --> client) */
    int seq_num;                 /* Start of sequence */
};

#endif // __IPC_FIFO_HEADER_H__