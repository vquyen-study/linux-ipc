> * SUSv3 specifies two types of POSIX semaphores:
>> - **Named semaphores**: This type of semaphore has a name. By calling ***sem_open()*** with the same name, unrelated processes can access the same semaphore.
>> - **Unnamed semaphores**: ***This type of semaphore doesn’t have a name***; instead, it resides at an agreed-upon location in memory. ***Unnamed semaphores can be shared between processes or between a group of threads***. When shared between processes, the semaphore must reside in a region of (System V, POSIX, or mmap()) shared memory. When shared between threads, the semaphore may reside in an area of memory shared by the threads (e.g., on the heap or in a global variable).

**To work with a named semaphore, we employ the following functions**:
> - The sem_open() function opens or creates a semaphore, initializes the semaphore if it is created by the call, and returns a handle for use in later calls.
> - The sem_post(sem) and sem_wait(sem) functions respectively increment and decrement a semaphore’s value.
> - The sem_getvalue() function retrieves a semaphore’s current value.
> - The sem_close() function removes the calling process’s association with a semaphore that it previously opened.
> - The sem_unlink() function removes a semaphore name and marks the semaphore for deletion when all processes have closed it.

- **Operations on unnamed semaphores** use the same functions (sem_wait(), sem_post(), sem_getvalue(), and so on) that are used to operate on named semaphores. In addition, two further functions are required:
> - The sem_init() function initializes a semaphore and informs the system of whether the semaphore will be shared between processes or between the threads of a single process.
> The sem_destroy(sem) function destroys a semaphore.

- Unnamed versus named semaphores
> - A semaphore that is shared between threads doesn’t need a name. Making an unnamed semaphore a shared (global or heap) variable automatically makes it accessible to all threads.
> - A semaphore that is being shared between related processes doesn’t need a name. If a parent process allocates an unnamed semaphore in a region of shared memory (e.g., a shared anonymous mapping), then a child automatically inherits the mapping and thus the semaphore as part of the operation of fork().
> - If we are building a dynamic data structure (e.g., a binary tree), each of whose items requires an associated semaphore, then the simplest approach is to allocate an unnamed semaphore within each item. Opening a named semaphore for each item would require us to design a convention for generating a (unique) semaphore name for each item and to manage those names (e.g., unlinking them when they are no longer required).