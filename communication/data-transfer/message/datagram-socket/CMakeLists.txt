cmake_minimum_required (VERSION 3.20.0)
project (datagram-socket)

add_executable (unix-datagram-server unix-datagram-socket-server.c)
add_executable (unix-datagram-client unix-datagram-sock-client.c)

add_executable (inet-datagram-server inet-datagram-socket-echo-server.c)
add_executable (inet-datagram-client inet-datagram-socket-echo-client.c)