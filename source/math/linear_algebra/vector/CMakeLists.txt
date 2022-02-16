cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/std_vector_overloads.h
     ${CMAKE_CURRENT_LIST_DIR}/vector2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/vector2d.h
     ${CMAKE_CURRENT_LIST_DIR}/vector3d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/vector3d.h
     PARENT_SCOPE)

