
#include "datagram-socket-header.h"

#define TAG         "[client] "

int
main(int argc, char *argv[])
{
    int sfd, j, s;
    size_t len;
    ssize_t numRead;
    char buf[BUF_SIZE];

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        printf(TAG "%s <msg1> <msg2> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;        /* Allows IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM;

    /* Construct server address from first command-line argument */

    s = getaddrinfo("localhost", "echo", &hints, &result);
    if (s != 0) {
        errno = ENOSYS;
        return -1;
    }

    /* Walk through returned list until we find an address structure
       that can be used to successfully connect a socket */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;                   /* On error, try next address */

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                      /* Success */

        /* Connect failed: close this socket and try next address */

        close(sfd);
    }

    freeaddrinfo(result);

    /* Send remaining command-line arguments to server as separate datagrams */
    printf(TAG "Sending msg to server...\n");
    for (j = 1; j < argc; j++) {
        len = strlen(argv[j]);
        if (write(sfd, argv[j], len) != len)
        {
            printf(TAG "Fail to write socket...\n");
            exit(EXIT_FAILURE);
        }

        numRead = read(sfd, buf, BUF_SIZE);
        if (numRead == -1)
        {
            printf(TAG "Fail to write socket...\n");
            exit(EXIT_FAILURE);
        }

        printf("[%06ld bytes] %.*s\n", (long) numRead, (int) numRead, buf);
    }

    exit(EXIT_SUCCESS);
}