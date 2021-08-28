cmake_minimum_required (VERSION 3.20.0)
project (systemv-message-queue)

configure_file(ftok.key.samp ftok.key COPYONLY)

add_executable (server sysv-msg-queue-echo-server.c)
add_executable (client sysv-msg-queue-echo-client.c)