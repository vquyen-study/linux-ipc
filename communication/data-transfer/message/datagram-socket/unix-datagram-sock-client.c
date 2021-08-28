#include "datagram-socket-header.h"

#define TAG "[client] "

int main(int argc, char **argv)
{
    int sfd;
    struct sockaddr_un cl_sock_addr, sv_sock_addr;

    int rx_bytes;
    char rx_buff[BUF_SIZE] = {0};

    if (argc < 2)
    {
        printf("%s <msg> <msg> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        printf(TAG "fail to open socket.\n");
        exit(EXIT_FAILURE);
    }

    memset(&cl_sock_addr, 0, sizeof(struct sockaddr_un));
    cl_sock_addr.sun_family = AF_UNIX;
    snprintf(cl_sock_addr.sun_path, sizeof(cl_sock_addr.sun_path),
            "/tmp/ud_ucase_cl.%ld", (long) getpid());

    if (bind(sfd, (struct sockaddr *) &cl_sock_addr, sizeof(struct sockaddr_un)) == -1)
    {
        printf(TAG "fail to bind socket to unix domain\n");
        exit(EXIT_FAILURE);
    }

    // crreate server socket to send.
    memset(&sv_sock_addr, 0, sizeof(struct sockaddr_un));
    sv_sock_addr.sun_family = AF_UNIX;
    strncpy(sv_sock_addr.sun_path, UNIX_DATAGRAM_SOCK_SERVER_FILE, sizeof(sv_sock_addr.sun_path) - 1);
    for (int i = 0; i < argc; i++)
    {
        int tx_bytes = sendto(sfd, argv[i], strlen(argv[i]), 0,
                                (struct sockaddr *) &sv_sock_addr, 
                                sizeof(struct sockaddr_un));

        printf("tx bytes : %d\n", tx_bytes);


        int rx_bytes = recvfrom(sfd, rx_buff, BUF_SIZE, 0, NULL, NULL);
        rx_bytes = rx_bytes >= BUF_SIZE ? BUF_SIZE - 1 : rx_bytes;
        rx_buff[rx_bytes] = '\0';
        printf(TAG "receiving %d from %s\n", rx_bytes, rx_buff);
        
        sleep(1);
    }

    return 0;
}