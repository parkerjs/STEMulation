cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/compare_equal_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_greater_equal_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_greater_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_less_equal_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_less_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_not_equal_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/comparison_binary_functor.h
     PARENT_SCOPE)

