/*
 *
 *       client.c: Write strings for printing in the shared memory segment
 *                 (Producer process)
 * 
 * * ref : https://www.softprayog.in/programming/interprocess-communication-using-system-v-shared-memory-in-linux
 */

#include "sysv-shm-header.h"

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
    
    //  mutual exclusion semaphore
    /* generate a key for creating semaphore  */
    if ((s_key = ftok (PROJECT_KEY, PRJ_MUTEX_ID)) == -1)
        error ("ftok");
    if ((mutex_sem = semget (s_key, 1, 0)) == -1)
        error ("semget");
    
    // Get shared memory 
    if ((s_key = ftok (PROJECT_KEY, PRJ_SHM_ID)) == -1)
        error ("ftok");    
    if ((shm_id = shmget (s_key, sizeof (struct shared_memory), 0)) == -1)
        error ("shmget");
    if ((shared_mem_ptr = (struct shared_memory *) shmat (shm_id, NULL, 0)) 
         == (struct shared_memory *) -1) 
        error ("shmat");

    // counting semaphore, indicating the number of available buffers.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok (PROJECT_KEY, PRJ_BUFF_CNT_ID)) == -1)
        error ("ftok");
    if ((buffer_count_sem = semget (s_key, 1, 0)) == -1)
        error ("semget");

    // counting semaphore, indicating the number of strings to be printed.
    /* generate a key for creating semaphore  */
    if ((s_key = ftok (PROJECT_KEY, PRJ_SPOLL_SIG_ID)) == -1)
        error ("ftok");
    if ((spool_signal_sem = semget (s_key, 1, 0)) == -1)
        error ("semget");
    
    struct sembuf asem [1];

    asem [0].sem_num = 0;
    asem [0].sem_op = 0;
    asem [0].sem_flg = 0;

    char buf [200];

    printf ("Please type a message: ");

    while (fgets (buf, 198, stdin)) {
        // remove newline from string
        int length = strlen (buf);
        if (buf [length - 1] == '\n')
           buf [length - 1] = '\0';

        // get a buffer: P (buffer_count_sem);   
        asem [0].sem_op = -1;
        if (semop (buffer_count_sem, asem, 1) == -1)
	        error ("semop: buffer_count_sem");
    
        /* There might be multiple producers. We must ensure that 
            only one producer uses buffer_index at a time.  */
        // P (mutex_sem);
        asem [0].sem_op = -1;
        if (semop (mutex_sem, asem, 1) == -1)
	    error ("semop: mutex_sem");

	    // Critical section
            sprintf (shared_mem_ptr -> buf [shared_mem_ptr -> buffer_index], "(%d): %s\n", getpid (), buf);
            (shared_mem_ptr -> buffer_index)++;
            if (shared_mem_ptr -> buffer_index == MAX_BUFFERS)
                shared_mem_ptr -> buffer_index = 0;

        /** QUYENCV ADDING FOR DEBUGGING */
        // sleep(2);

        // Release mutex sem: V (mutex_sem)
        asem [0].sem_op = 1;
        if (semop (mutex_sem, asem, 1) == -1)
	    error ("semop: mutex_sem");
    
	// Tell spooler that there is a string to print: V (spool_signal_sem);
        asem [0].sem_op = 1;
        if (semop (spool_signal_sem, asem, 1) == -1)
	    error ("semop: spool_signal_sem");

        printf ("Please type a message: ");
    }
 
    if (shmdt ((void *) shared_mem_ptr) == -1)
        error ("shmdt");
    exit (0);
}