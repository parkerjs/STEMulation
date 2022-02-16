cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_addition_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_binary_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_division_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_modulo_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_multiplication_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_subtraction_functor.h
     PARENT_SCOPE)

