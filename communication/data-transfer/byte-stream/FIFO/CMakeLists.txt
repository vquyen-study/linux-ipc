cmake_minimum_required (VERSION 3.20.0)
project (fifo)

add_executable (fifo-client fifo-client.c)
add_executable (fifo-server fifo-server.c)