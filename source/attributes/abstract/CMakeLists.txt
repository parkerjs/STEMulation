cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/enumerable.h
     ${CMAKE_CURRENT_LIST_DIR}/factory_constructible.cpp
     ${CMAKE_CURRENT_LIST_DIR}/factory_constructible.h
     ${CMAKE_CURRENT_LIST_DIR}/iterable.h
     ${CMAKE_CURRENT_LIST_DIR}/reflective.h
     ${CMAKE_CURRENT_LIST_DIR}/reverse_iterable.h
     ${CMAKE_CURRENT_LIST_DIR}/singleton.h
     PARENT_SCOPE)

