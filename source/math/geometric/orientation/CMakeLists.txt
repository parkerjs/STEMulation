cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/euler_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/eulers.cpp
     ${CMAKE_CURRENT_LIST_DIR}/eulers.h
     ${CMAKE_CURRENT_LIST_DIR}/rotation_type.h
     PARENT_SCOPE)

