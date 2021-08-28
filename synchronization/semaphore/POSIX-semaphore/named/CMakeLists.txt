cmake_minimum_required (VERSION 3.20.0)
project (posix-named-semaphore)

add_executable (spooler spooler.c)
add_executable (client client.c)