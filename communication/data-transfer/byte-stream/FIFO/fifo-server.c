#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "fifo-header.h"

static char client_fifo_name[CLIENT_FIFO_NAME_LEN] = {0};

void clean_server_fifo_file(void)
{
    unlink(SERVER_FIFO);
}

int main(int argc, char **argv)
{
    int server_fifo_fd, dummyfd;
    int client_fifo_fd;
    int flags;

    struct request req;
    struct response rsp;

    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1)
    {
        printf("[server] Fail to create server fifo\n");
        exit(0);
    }

    printf("[server] fifo is created %s\n", SERVER_FIFO);

    if (atexit(clean_server_fifo_file) == -1)
    {
        printf("[server] Fail to register exit clean server fifo file\n");
        return 0;
    }

    // 1. open for read.
    /** 
     *  We use O_NONBLOCK to be able to open fifo client without waiting the open on 
     *  write end(sever side).
     */
    server_fifo_fd = open(SERVER_FIFO, O_RDONLY | O_NONBLOCK);
    // server_fifo_fd = open(SERVER_FIFO, O_RDONLY);
    if (client_fifo_fd == -1)
    {
        printf("[server] Fail to open client fifo\n");
        return 0;
    }

    printf("[server] received connection from client.\n");

    // We make sure the server never get EOF, that make fail on 
    // reading server fifo.
    dummyfd = open(SERVER_FIFO, O_WRONLY);
    if (client_fifo_fd == -1)
    {
        printf("[server] Fail to open client fifo\n");
        return 0;
    }

    // It should be skipped SIGPIPE signal(failure), this is generated
    // when server gets fail on writing to client_fifo, by default the SIGPIPE
    // terminates process.
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        printf("[server] Fail to register SIGPIPE with SIG_IGN\n");
        return 0;
    }

    flags = fcntl(server_fifo_fd, F_GETFL);
    flags &= ~O_NONBLOCK;       /* Disable O_NONBLOCK bit */
    fcntl(server_fifo_fd, F_SETFL, flags);

    while (1)
    {
        if (read(server_fifo_fd, &req, sizeof(req)) != sizeof(req))
            continue;

        printf("[server] receiving %ld, seqid(%d)\n", req.pid, req.seq_num);
        // open client fifo to write.
        memset(client_fifo_name, 0, CLIENT_FIFO_NAME_LEN);
        snprintf(client_fifo_name, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, req.pid);
        client_fifo_fd = open(client_fifo_name, O_WRONLY);
        if (client_fifo_fd == -1)
        {
            printf("[server] Fail to open client fifo %s\n", client_fifo_name);
            return 0;
        }

        rsp.seq_num = req.seq_num;
        if (write(client_fifo_fd, &rsp, sizeof(rsp)) != sizeof(rsp))
        {
            printf("[server] Fail to write response message to client fifo\n");
            return 0;
        }

        // close client fifo to save server resource.
        close(client_fifo_fd);
    }

    close(server_fifo_fd);
    return 0;
}