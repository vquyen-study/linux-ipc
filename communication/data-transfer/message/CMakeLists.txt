cmake_minimum_required (VERSION 3.20.0)
project (ipc-communication-data-transfer)

add_subdirectory (datagram-socket)
add_subdirectory (POSIX-message-queue)
add_subdirectory (System-V-message-queue)