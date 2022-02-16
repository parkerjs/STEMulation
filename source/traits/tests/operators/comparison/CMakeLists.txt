cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/has_equal.h
     ${CMAKE_CURRENT_LIST_DIR}/has_greater.h
     ${CMAKE_CURRENT_LIST_DIR}/has_greater_equal.h
     ${CMAKE_CURRENT_LIST_DIR}/has_less.h
     ${CMAKE_CURRENT_LIST_DIR}/has_less_equal.h
     ${CMAKE_CURRENT_LIST_DIR}/has_not_equal.h
     PARENT_SCOPE)

