cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/complex.h
     ${CMAKE_CURRENT_LIST_DIR}/conjugate.h
     ${CMAKE_CURRENT_LIST_DIR}/quat.cpp
     ${CMAKE_CURRENT_LIST_DIR}/quat.h
     PARENT_SCOPE)

