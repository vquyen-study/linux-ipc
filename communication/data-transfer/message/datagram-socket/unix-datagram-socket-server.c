#include "datagram-socket-header.h"


#define TAG "[server] "

int main(int argc, char **argv)
{
    int sfd;
    struct sockaddr_un s_addr, c_addr;
    socklen_t c_sockaddr_len = sizeof(struct sockaddr_un);

    int rx_bytes;
    char buff[BUF_SIZE] = {0};

    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        printf(TAG "fail to open socket.\n");
        exit(EXIT_FAILURE);
    }

    if (remove(UNIX_DATAGRAM_SOCK_SERVER_FILE) == -1 && errno != ENOENT)
    {
        printf(TAG "fail to remove old socket.\n");
        exit(EXIT_FAILURE);
    }

    memset(&s_addr, 0, sizeof(struct sockaddr_un));
    s_addr.sun_family = AF_UNIX;
    strncpy(s_addr.sun_path, UNIX_DATAGRAM_SOCK_SERVER_FILE, sizeof(s_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
    {
        printf(TAG "fail to bind socket to unix domain\n");
        exit(EXIT_FAILURE);
    }

    printf(TAG "ready to receive message from client...\n");
    char tx_buf[100] = "tx buffer 1";
    while (1)
    {
        c_sockaddr_len = sizeof(struct sockaddr_un);
        
        rx_bytes = recvfrom(sfd, buff, BUF_SIZE, 0,(struct sockaddr *) &c_addr, &c_sockaddr_len);
        if (rx_bytes == -1)
        {
            printf(TAG "Fail on recvfrom on socket %d\n", errno);
            exit(EXIT_FAILURE);
        }
        
        rx_bytes = (rx_bytes >= BUF_SIZE ? BUF_SIZE - 1 : rx_bytes);
        buff[rx_bytes] = '\0';

        printf("Server received %d bytes from %s\n", rx_bytes, buff);
        for (int i = 0; i < rx_bytes; i++)
            buff[i] = toupper(buff[i]);

        sendto(sfd, buff, rx_bytes, 0,(const struct sockaddr *) &c_addr, c_sockaddr_len);
    }

    return 0;
}