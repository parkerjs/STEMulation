cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/logical_and_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/logical_binary_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/logical_or_functor.h
     PARENT_SCOPE)

