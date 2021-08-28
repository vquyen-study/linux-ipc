#ifndef __US_STREAM_HEADER_H__
#define __US_STREAM_HEADER_H__

#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */

#include <sys/un.h>
#include <sys/socket.h>

#define US_SERVER_SOCK_PATH "/tmp/unix-stream-socket-server"
#define US_SERVER_BACKLOG   5

#define SOCK_BUFF_SIZE  1024
#define PORT_NUM        "8899"

#endif // __US-STREAM-HEADER_H__