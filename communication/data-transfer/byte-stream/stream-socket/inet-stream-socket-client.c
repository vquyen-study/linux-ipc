#include <netdb.h>
#include "is-stream-header.h"

#define TAG "[Client] "

int main(int argc, char **argv)
{
    int count = 10;

    char buff[SOCK_BUFF_SIZE] = {0};

    socklen_t addrlen;
    struct sockaddr_storage claddr;

    int lfd, cfd, optval, reqLen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc != 3)
    {
        printf("%s <count> <msg>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    count = atoi(argv[1]);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;                            /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;           /* Wildcard IP address; service name is numeric */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0)
    {
        printf(TAG "Fail to getaddrinfo...\n");
        exit(EXIT_FAILURE);
    } 

    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;                                       /* On error, try next address */

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                              /* Success */

        /* bind() failed: close this socket and try next address */
        close(cfd);
    }
    freeaddrinfo(result);

    printf(TAG "Connected to server\n");
    printf(TAG "Send msg to server\n");
    while (count--)
    {
        write(cfd, argv[2], strlen(argv[2]));
        if (read(cfd, buff, SOCK_BUFF_SIZE) > 0)
        {
            printf(TAG "Receving msg %s\n", buff);
            memset(buff, 0, SOCK_BUFF_SIZE);
        }

        sleep(1);
    }
    return 0;
}