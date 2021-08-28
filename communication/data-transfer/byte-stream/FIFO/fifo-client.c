#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "fifo-header.h"

static char client_fifo_name[CLIENT_FIFO_NAME_LEN] = {0};

void clean_client_fifo_file(void)
{
    unlink(client_fifo_name);
}

int main(int argc, char **argv)
{
    struct request req;
    struct response rsp;

    int client_fifo_fd = 0;
    int server_fifo_fd = 0;

    int count;
    int seq_num = 10;
    int flags;

    if (argc < 2 || (argc > 1 && strcmp(argv[1], "--help") == 0))
    {
        printf("%s <num-req> [req-id]\n", argv[0]);
        return 0;
    }

    
    
    count = strtol(argv[1], NULL, 0);
    seq_num = argc > 2 ? strtol(argv[2], NULL, 0) : 10;

    req.pid = getpid();
    req.seq_num = seq_num;

    umask(0);

    snprintf(client_fifo_name, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long int) req.pid);
    if (mkfifo(client_fifo_name, S_IRUSR | S_IWUSR | S_IWGRP) == -1)
    {
        printf("[client] Fail to create client fifo\n");
        exit(0);
    }

    printf("fifo is created %s\n", client_fifo_name);

    if (atexit(clean_client_fifo_file) != 0)
    {
        printf("[client] Fail to register clean fifo file on exit\n");
        return -1;
    }

    /** 
     *  We use O_NONBLOCK to be able to open fifo client without waiting the open on 
     *  write end(sever side).
     */
    client_fifo_fd = open(client_fifo_name, O_RDONLY | O_NONBLOCK);
    // client_fifo_fd = open(client_fifo_name, O_RDONLY);
    if (client_fifo_fd == -1)
    {
        printf("[client] Fail to open client fifo\n");
        return 0;
    }

    int dummyfd = open(client_fifo_name, O_WRONLY);

    flags = fcntl(client_fifo_fd, F_GETFL);
    flags &= ~O_NONBLOCK; /* Disable O_NONBLOCK bit */
    fcntl(client_fifo_fd, F_SETFL, flags);

    server_fifo_fd = open(SERVER_FIFO, O_WRONLY);
    if (client_fifo_fd == -1)
    {
        printf("[client] Fail to open server fifo\n");
        return 0;
    }

    for (; count != 0; count--)
    {
        if (write(server_fifo_fd, &req, sizeof(req)) != sizeof(req))
        {
            printf("[client] Fail on writing on the server fifo\n");
            return 0;
        }

        if (read(client_fifo_fd, &rsp, sizeof(rsp)) != sizeof (rsp))
        {
            printf("[client] Fail to read response form client fifo\n");
            return 0;
        }

        printf("[client] receving echo message id %d ...\n", rsp.seq_num); 

        req.seq_num++;       
        sleep(1);
    }

    close(server_fifo_fd);
    close(client_fifo_fd);

    return 0;
}