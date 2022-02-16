cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/finite_difference.h
     ${CMAKE_CURRENT_LIST_DIR}/finite_difference_scheme.h
     ${CMAKE_CURRENT_LIST_DIR}/sequence_differentiator.h
     PARENT_SCOPE)

