#include "datagram-socket-header.h"

#include <syslog.h>
#include <pthread.h>

#define IS_ADDR_STR_LEN 4096
#define TAG     "[server] "

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrlen,
                        char *addrStr, int addrStrLen)
{
    char host[NI_MAXHOST], service[NI_MAXSERV];

    if (getnameinfo(addr, addrlen, host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICSERV) == 0)
        snprintf(addrStr, addrStrLen, "(%s, %s)", host, service);
    else
        snprintf(addrStr, addrStrLen, "(?UNKNOWN?)");

    return addrStr;
}

/* Maximum length of string in per-thread buffer returned by strerror() */
#define MAX_ERROR_LEN 256           
static pthread_once_t once = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey;

/* Free thread-specific data buffer */
static void destructor(void *buf)
{
    free(buf);
}

/* One-time key creation function */
static void createKey(void)
{
    int s;

    /* Allocate a unique thread-specific data key and save the address
       of the destructor for thread-specific data buffers */

    s = pthread_key_create(&strerrorKey, destructor);
    if (s != 0)
    {
        printf(TAG "pthread_key_create");
        exit(EXIT_FAILURE);
    }
}


char *strerror(int err)
{
    int s;
    char *buf;

    /* Make first caller allocate key for thread-specific data */

    s = pthread_once(&once, createKey);
    if (s != 0)
    {
        printf(TAG "pthread_once");
        exit(EXIT_FAILURE);
    }

    buf = pthread_getspecific(strerrorKey);
    if (buf == NULL) {          /* If first call from this thread, allocate
                                   buffer for thread, and save its location */
        buf = malloc(MAX_ERROR_LEN);
        if (buf == NULL)
        {
            printf(TAG "malloc");
            exit(EXIT_FAILURE);
        }

        s = pthread_setspecific(strerrorKey, buf);
        if (s != 0)
        {
            printf(TAG "pthread_setspecific");
            exit(EXIT_FAILURE);
        }
    }

    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';          /* Ensure null termination */
    }

    return buf;
}


int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    int sfd, s, optval;

    ssize_t numRead;
    socklen_t len;
    struct sockaddr_storage claddr;
    char buf[BUF_SIZE];
    char addrStr[IS_ADDR_STR_LEN] = {0};

    memset(&hints, 0, sizeof(hints));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;            /* Allows IPv4 and IPv6. */
    hints.ai_socktype = SOCK_DGRAM;

    /**
     * the service argument must be one of the service that has enabled in /etc/services. 
    */
    s = getaddrinfo("localhost", "echo", &hints, &result);
    if (s != 0) {
        printf(TAG "Fail at getaddrinfo() ret(%d)\n", s);
        errno = ENOSYS;
        return -1;
    }

    /* Walk through returned list until we find an address structure
       that can be used to successfully create and bind a socket */

    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;                   /* On error, try next address */

        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                sizeof(optval)) == -1) {
            close(sfd);
            freeaddrinfo(result);
            return -1;
        }


        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                      /* Success */

        /* bind() failed: close this socket and try next address */

        close(sfd);
    }
    freeaddrinfo(result);

    printf(TAG "Server inet-datagram is waiting message from client...\n");
    for (;;) {
        len = sizeof(struct sockaddr_storage);
        numRead = recvfrom(sfd, buf, BUF_SIZE, 0,
                           (struct sockaddr *) &claddr, &len);
        if (numRead == -1)
        {
            printf(TAG "Fail to recvfrom on socket...\n");
            exit(EXIT_FAILURE);
        }

        if (sendto(sfd, buf, numRead, 0, (struct sockaddr *) &claddr, len)
                        != numRead)
        {
            syslog(LOG_WARNING, "Error echoing response to %s (%s)",
                    inetAddressStr((struct sockaddr *) &claddr, len,
                                   addrStr, IS_ADDR_STR_LEN),
                    strerror(errno));
            printf(TAG "Error echoing response to %s (%s)", inetAddressStr((struct sockaddr *) &claddr, len,
                            addrStr, IS_ADDR_STR_LEN), strerror(errno));
        }
    }

    return 0;
}