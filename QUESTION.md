# Inter Process Communication on Unix

### 1. Running ipcs & ipcrm on linux?
> Have you had problem when unable to create key in message queue after stop and start program?
>> Answer : It is result of missing of removing the message-identifier after uses.

### 2. Using SysV IPCs?
> Suppose we have 3 program, (Creator, Writer, Reader) is it possible (Writer, Reader) can use the ipcs(message queue, sharemem,...) that are created by (Creator)? if yes, why? if not, why not?
>> Answer : Yes, these ipcs remain in kernel with specific identifier, if the xxx_open feeds on correct key, that will result correct identifier, then you will use those ipcs.

### 3. what is advancetages using read/write on **Shared File Mappings**(mmap file descriptor).
> The reasons that memory-mapped I/O can provide performance benefits are as
follows:
>> - A normal read() or write() involves two transfers: one between the file and the kernel buffer cache, and the other between the buffer cache and a user-space buffer. Using mmap() eliminates the second of these transfers. For input, the data is available to the user process as soon as the kernel has mapped the corresponding file blocks into memory. For output, the user process merely needs to modify the contents of the memory, and can then rely on the kernel memory manager to automatically update the underlying file.
>> - In addition to saving a transfer between kernel space and user space, mmap() can also improve performance by lowering memory requirements. When using read() or write(), the data is maintained in two buffers: one in user space and the other in kernel space. When using mmap(), a single buffer is shared between the kernel space and user space. Furthermore, if multiple processes are performing I/O on the same file, then, using mmap(), they can all share the same kernel buffer, resulting in an additional memory saving.

### 4. What is problem if the value is increased by semop is greater than availabe in semaphore(ex : by default resource has 5 writer(feeder), and 1 reader(eater). Normally after a resource is read one by reader(eater) then it is increase by 1, but what is the problem if it is tried to increase 10.)
> The resource will be overrided by writers, then data integrity is not realiable:
>> - Because semaphore is integer, so when it is written with 10, then it means the available resource will be 10, instead 5, so with it is overrided if 10 writers write at a time.

### 5. Is the below code legal?
> - Below code is legal because the lock and unlock is push to ***pthread_cond_wait()***
```C++
int main(int argc, char**argv)
{

    //....

    /* The main thread waits on a condition variable that is signaled
       on each invocation of the thread notification function. We
       print a message so that the user can see that this occurred. */

    s = pthread_mutex_lock(&mtx);
    if (s != 0)
        errExitEN(s, "pthread_mutex_lock");

    for (;;) {
        s = pthread_cond_wait(&cond, &mtx);
        if (s != 0)
            error("pthread_cond_wait");
        printf("main(): expireCnt = %d\n", expireCnt);
    }

    //...
}
```

