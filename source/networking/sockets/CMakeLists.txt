cmake_minimum_required (VERSION 3.7.0)

if (OpenSSL_FOUND)
    # add additional sub-directories with more CMakeLists
    add_subdirectory (ssl)
endif ()

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/chunkedReceiver.cpp
     ${CMAKE_CURRENT_LIST_DIR}/chunkedReceiver.h
     ${CMAKE_CURRENT_LIST_DIR}/nonChunkedReceiver.cpp
     ${CMAKE_CURRENT_LIST_DIR}/nonChunkedReceiver.h
     ${CMAKE_CURRENT_LIST_DIR}/receiver.cpp
     ${CMAKE_CURRENT_LIST_DIR}/receiver.h
     ${CMAKE_CURRENT_LIST_DIR}/socket.cpp
     ${CMAKE_CURRENT_LIST_DIR}/socket.h
     ${CMAKE_CURRENT_LIST_DIR}/TCP_Socket.cpp
     ${CMAKE_CURRENT_LIST_DIR}/TCP_Socket.h
     PARENT_SCOPE)

