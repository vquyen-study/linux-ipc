cmake_minimum_required (VERSION 3.20.0)
project (posix-semaphore)

configure_file(ftok.key.samp ftok.key COPYONLY)

add_executable (spooler spooler.c)
add_executable (client client.c)