#ifndef __DATAGRAM_SOCKET_HEADER_H__
#define __DATAGRAM_SOCKET_HEADER_H__

#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions, plus EXIT_SUCCESS and EXIT_FAILURE constants */                
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */
#include <ctype.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Maximum size of messages exchanged between client and server */
#define BUF_DATA_SIZE   1024
#define BUF_SIZE (BUF_DATA_SIZE + 1)             
                                   
#define UNIX_DATAGRAM_SOCK_SERVER_FILE "/tmp/unix-datagram-sock-server"
#define UNIX_DATAGRAM_SOCK_CLIENT_FILE "/tmp/ud-sock-client.%d"

#endif // __DATAGRAM-SOCKET-HEADER_H__