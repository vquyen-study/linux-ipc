#include <netdb.h>
#include "is-stream-header.h"

#define TAG "[Server] "

int main(int argc, char **argv)
{
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
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;                                       /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))
                == -1)
        {
            printf(TAG "Fail to adding setsockopt\n");
            exit(EXIT_FAILURE);
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                                          /* Success */

        /* bind() failed: close this socket and try next address */
        close(lfd);
    }
    freeaddrinfo(result);

    if (listen(lfd, US_SERVER_BACKLOG) == -1)
    {
        printf(TAG "Unable to listen socket on port %d\n", PORT_NUM);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) {
            continue;
        }

        if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                    host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);

        while (read(cfd, buff, SOCK_BUFF_SIZE) > 0)
        {
            write(cfd, buff, SOCK_BUFF_SIZE);
            memset(buff, 0, SOCK_BUFF_SIZE);
        }

        close(cfd);                                         /* Close connection.*/
    }
    return 0;
}