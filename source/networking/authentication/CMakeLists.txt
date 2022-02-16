cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/cookie.cpp
     ${CMAKE_CURRENT_LIST_DIR}/cookie.h
     ${CMAKE_CURRENT_LIST_DIR}/credentialed.h
     ${CMAKE_CURRENT_LIST_DIR}/instanceCredentialed.cpp
     ${CMAKE_CURRENT_LIST_DIR}/instanceCredentialed.h
     ${CMAKE_CURRENT_LIST_DIR}/staticallyCredentialed.cpp
     ${CMAKE_CURRENT_LIST_DIR}/staticallyCredentialed.h
     PARENT_SCOPE)

