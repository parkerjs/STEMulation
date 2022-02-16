cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/has_addition.h
     ${CMAKE_CURRENT_LIST_DIR}/has_addition_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_division.h
     ${CMAKE_CURRENT_LIST_DIR}/has_division_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_modulo.h
     ${CMAKE_CURRENT_LIST_DIR}/has_modulo_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_multiplication.h
     ${CMAKE_CURRENT_LIST_DIR}/has_multiplication_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_prefix_decrement.h
     ${CMAKE_CURRENT_LIST_DIR}/has_prefix_increment.h
     ${CMAKE_CURRENT_LIST_DIR}/has_subtraction.h
     ${CMAKE_CURRENT_LIST_DIR}/has_subtraction_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_suffix_decrement.h
     ${CMAKE_CURRENT_LIST_DIR}/has_suffix_increment.h
     PARENT_SCOPE)

