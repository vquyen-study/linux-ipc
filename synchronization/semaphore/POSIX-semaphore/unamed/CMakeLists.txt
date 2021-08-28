cmake_minimum_required (VERSION 3.20.0)
project (posix-unnamed-semaphore)

add_executable (unnamed-sema-thread posix-unnamed-sema-pthread.c)
add_executable (unnamed-sema-process posix-unnamed-sema-processes.c)