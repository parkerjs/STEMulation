cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/certificateDownloader.cpp
     ${CMAKE_CURRENT_LIST_DIR}/certificateDownloader.h
     ${CMAKE_CURRENT_LIST_DIR}/SSL_Socket.cpp
     ${CMAKE_CURRENT_LIST_DIR}/SSL_Socket.h
     PARENT_SCOPE)

