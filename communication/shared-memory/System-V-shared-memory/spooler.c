/*
 *
 *       spooler.c: Print strings in the shared memory segment
 *                  (Consumer process)
 * 
 * ref : https://www.softprayog.in/programming/interprocess-communication-using-system-v-shared-memory-in-linux
 */

#include "sysv-shm-header.h"

void error (char *msg);

int main (int argc, char **argv)
{
    key_t s_key;
    union semun  
    {
        int val;
        struct semid_ds *buf;
        ushort array [1];
    } sem_attr;
    int shm_id;
    struct shared_memory *shared_mem_ptr;
    int mutex_sem, buffer_count_sem, spool_signal_sem;
    
    printf ("spooler: hello world\n");
    //  mutual exclusion semaphore
    /* generate a key for creating semaphore  */
    if ((s_key = ftok (PROJECT_KEY, PRJ_MUTEX_ID)) == -1)
        error ("ftok");
    if ((mutex_sem = semget (s_key, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // Giving initial value. 
    sem_attr.val = 0;        // locked, till we finish initialization
    if (semctl (mutex_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");
    
    // Get shared memory 
    if ((s_key = ftok (PROJECT_KEY, PRJ_SHM_ID)) == -1)
        error ("ftok");    
    if ((shm_id = shmget (s_key, sizeof (struct shared_memory), 
         0660 | IPC_CREAT)) == -1)
        error ("shmget");
    if ((shared_mem_ptr = (struct shared_memory *) shmat (shm_id, NULL, 0)) 
         == (struct shared_memory *) -1) 
        error ("shmat");
    // Initialize the shared memory
    shared_mem_ptr -> buffer_index = shared_mem_ptr -> buffer_print_index = 0;

    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok (PROJECT_KEY, PRJ_BUFF_CNT_ID)) == -1)
        error ("ftok");
    if ((buffer_count_sem = semget (s_key, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // giving initial values
    sem_attr.val = MAX_BUFFERS;    // MAX_BUFFERS are available
    if (semctl (buffer_count_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");

    /* generate a key for creating semaphore  */
    if ((s_key = ftok (PROJECT_KEY, PRJ_SPOLL_SIG_ID)) == -1)
        error ("ftok");
    if ((spool_signal_sem = semget (s_key, 1, 0660 | IPC_CREAT)) == -1)
        error ("semget");
    // giving initial values
    sem_attr.val = 0;    // 0 strings are available initially.
    if (semctl (spool_signal_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL");
    
    // Initialization complete; now we can set mutex semaphore as 1 to 
    // indicate shared memory segment is available
    sem_attr.val = 1;
    if (semctl (mutex_sem, 0, SETVAL, sem_attr) == -1)
        error ("semctl SETVAL"); 

    struct sembuf asem [1];

    asem [0].sem_num = 0;
    asem [0].sem_op = 0;
    asem [0].sem_flg = 0;

    while (1) {  // forever
        // Is there a string to print? P (spool_signal_sem);
        asem [0].sem_op = -1;
        if (semop (spool_signal_sem, asem, 1) == -1)
	    perror ("semop: spool_signal_sem");
    
        printf ("%s", shared_mem_ptr -> buf [shared_mem_ptr -> buffer_print_index]);

        /* Since there is only one process (the spooler) using the 
           buffer_print_index, mutex semaphore is not necessary */
        (shared_mem_ptr -> buffer_print_index)++;
        if (shared_mem_ptr -> buffer_print_index == MAX_BUFFERS)
           shared_mem_ptr -> buffer_print_index = 0;
        

        /** QUYENCV ADDING FOR DEBUGGING */
        sleep(3);

        /* Contents of one buffer has been printed.
           One more buffer is available for use by producers.
           Release buffer: V (buffer_count_sem);  */
        asem [0].sem_op = 1;
        if (semop (buffer_count_sem, asem, 1) == -1)
	    perror ("semop: buffer_count_sem");
    }
}
