- **How does the pthread_condition variable works**
>- Before considering the code of the consumer, we need to explain **pthread_cond_wait()** in greater detail. We noted earlier that a condition variable always has an associated mutex. Both of these objects are passed as arguments to ***pthread_cond_wait()***, which performs the following steps:
>> 1. **unlock the mutex** specified by *mutex*.
>> 2. **block the calling thread** until another thread signals the condition variable *cond*.
>> 3. relock *mutex*.

- Below code is legal, the lock and unlock is push to ***pthread_cond_wait()***

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