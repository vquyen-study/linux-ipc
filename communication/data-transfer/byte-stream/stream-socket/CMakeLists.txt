cmake_minimum_required (VERSION 3.20.0)
project (socket-stream)

add_executable (us-server unix-stream-sock-server.c)
add_executable (us-client unix-stream-sock-client.c)
add_executable (is-server inet-stream-socket-server.c)
add_executable (is-client inet-stream-socket-client.c)