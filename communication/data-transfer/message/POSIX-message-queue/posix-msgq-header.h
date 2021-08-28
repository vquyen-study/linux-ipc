#ifndef __POSIX_MSGQ_HEADER_H__
#define __POSIX_MSGQ_HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#define SERVER_QUEUE_NAME   "server-id"
#define CLIENT_QUEUE_NAME   "client-id-%d"
#define QUEUE_PERMISSIONS   0660
#define MAX_MESSAGES        10
#define MAX_MSG_SIZE        256
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)



#endif // __POSIX-MSGQ-HEADER_H__