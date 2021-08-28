cmake_minimum_required (VERSION 3.20.0)
project (posix-message-queue)

configure_file(data.sample data.sample COPYONLY)

add_executable (file-map-share file-map-shared.c)
