cmake_minimum_required (VERSION 3.20.0)
project (posix-message-queue)

add_executable (server posix-msg-queue-echo-server.c)
add_executable (client posix-msg-queue-echo-client.c)