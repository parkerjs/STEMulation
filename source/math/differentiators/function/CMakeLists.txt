cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/derivative.h
     ${CMAKE_CURRENT_LIST_DIR}/jacobian.h
     ${CMAKE_CURRENT_LIST_DIR}/partial.h
     PARENT_SCOPE)

