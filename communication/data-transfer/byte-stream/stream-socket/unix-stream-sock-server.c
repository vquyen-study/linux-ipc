#include "us-stream-header.h"

#define TAG "[Server] "

int main(int argc, char **argv)
{
    int sfd, cfd;
    struct sockaddr_un s_addr;
    char buff[SOCK_BUFF_SIZE] = {0};

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf(TAG "Error to open socket\n");
        exit(EXIT_FAILURE);
    }

    /* Construct server socket address, bind socket to it,
       and make this a listening socket */
    if (remove(US_SERVER_SOCK_PATH) == -1 && errno != ENOENT)
    {
        printf(TAG "Error to remove older socket file\n");
        exit(EXIT_FAILURE);
    }

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sun_family = AF_UNIX;
    strncpy(s_addr.sun_path, US_SERVER_SOCK_PATH, sizeof(s_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &s_addr, sizeof(struct sockaddr_un)) == -1)
    {
        printf(TAG "Error could not bind to socket %s\n", US_SERVER_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, US_SERVER_BACKLOG) == -1)
    {
        printf(TAG "Error on listen socket.\n");
        exit(EXIT_FAILURE);
    }

    printf(TAG "Waiting a connection to server....\n");
    while (1)
    {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1)
        {
            printf(TAG "Error on accept connection\n");
            exit(EXIT_FAILURE);
        }

        printf(TAG "Connection has been established...\n");
        while (read(cfd, buff, SOCK_BUFF_SIZE) > 0)
        {
            printf(TAG "Receving msg '%s'\n", buff);
            write(cfd, buff, SOCK_BUFF_SIZE);               /* echo back to client. */
            memset(buff, 0, SOCK_BUFF_SIZE);
        }
        printf(TAG "Connection has been closed...\n");
        close(cfd);

    }

    return 0;
}