
#include "posix-msgq-header.h"

#define CLIENT_QUEUE_PATH_BYTES 64

int main(int argc, char **argv)
{
    char client_path[CLIENT_QUEUE_PATH_BYTES] = {0};
    struct mq_attr attr;
    mqd_t qd_client, qd_server;

    char in_buff[MSG_BUFFER_SIZE] = {0};

    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;

    /* rx queue - client queue */
    snprintf(client_path, CLIENT_QUEUE_PATH_BYTES, "client-id-%d", getpid());
    if ((qd_client = mq_open(client_path, O_RDONLY | O_CREAT, 
                             QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    
    /* tx queue - server queue */
    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }

    while (1)
    {
        if (mq_send(qd_server, client_path, strlen(client_path) + 1, 0) == -1)
        {
            perror("Client: unable to send message to server.");
            continue;
        }

        if (mq_receive (qd_client, in_buff, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Client: mq_receive");
            exit (1);
        }

        // display token received from server
        printf ("[client] receive seq %s\n", in_buff);
        sleep(1);
    }

    return 0;
}