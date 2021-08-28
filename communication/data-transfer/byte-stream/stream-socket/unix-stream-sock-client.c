#include "us-stream-header.h"

#define TAG "[Client] "

int main(int argc, char **argv)
{
    int sfd;
    struct sockaddr_un s_addr;

    int count;
    char buff[SOCK_BUFF_SIZE] = {0};

    if (argc != 3)
    {
        printf("%s <count> <msg>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    count = atoi(argv[1]);

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        printf(TAG "Error to open socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sun_family = AF_UNIX;
    strncpy(s_addr.sun_path, US_SERVER_SOCK_PATH, sizeof(s_addr.sun_path) - 1);

    printf(TAG "Connect to server %s\n", US_SERVER_SOCK_PATH);
    if (connect(sfd, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
    {
        printf(TAG "Fail to connect to server %s\n", US_SERVER_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    printf(TAG "sending message(%s) to server %d times\n", argv[2], count);
    while (count--)
    {
        int xfer_bytes = strlen(argv[2]);
        if (write(sfd, argv[2], xfer_bytes) != xfer_bytes)
        {
            printf(TAG "Fail to write to socket tx_bytes(%d)\n", xfer_bytes);
            exit(EXIT_FAILURE);
        }

        if (read(sfd, buff, SOCK_BUFF_SIZE) != SOCK_BUFF_SIZE)
        {
            printf(TAG "Fail to read to socket tx_bytes(%d)\n", xfer_bytes);
            exit(EXIT_FAILURE);
        }

        printf(TAG "Receiving echo msg %s\n", buff);

        memset(buff, 0, SOCK_BUFF_SIZE);
        sleep(1);
    }

    return 0;
}