cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_addable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_addition_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_decrementable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_divisible.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_division_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_incrementable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_modulable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_modulo_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_multipliable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_multiplication_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_subtractable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithemtic_subtraction_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_attributes.h
     PARENT_SCOPE)

