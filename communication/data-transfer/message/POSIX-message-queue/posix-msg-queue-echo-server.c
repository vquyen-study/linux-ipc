#include "posix-msgq-header.h"

int main(int argc, char **argv)
{
    mqd_t qd_server, qd_client;
    long token_number = 1;

    struct mq_attr attr;
    char in_buff[MSG_BUFFER_SIZE] = {0};
    char out_buff[MSG_BUFFER_SIZE] = {0};

    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    
    if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT,
                             QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror("mq_open qd_server fail");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (mq_receive(qd_server, in_buff, MSG_BUFFER_SIZE, NULL) == -1)
        {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }

        if ((qd_client = mq_open(in_buff, O_WRONLY)) == -1)
        {
            perror("mq_open client failure");
            exit(EXIT_FAILURE);
        }

        sprintf(out_buff, "%ld", token_number);
        printf("[server] service %s, %s\n", in_buff, out_buff);

        if (mq_send(qd_client, out_buff, strlen(out_buff), 0) == -1)
        {
            perror("mq_send qd_client failure");
            continue;
        }

        token_number++;
    }

    return 0;
}