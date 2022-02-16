cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/has_and.h
     ${CMAKE_CURRENT_LIST_DIR}/has_and_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_left_shift.h
     ${CMAKE_CURRENT_LIST_DIR}/has_left_shift_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_or.h
     ${CMAKE_CURRENT_LIST_DIR}/has_or_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_right_shift.h
     ${CMAKE_CURRENT_LIST_DIR}/has_right_shift_assign.h
     ${CMAKE_CURRENT_LIST_DIR}/has_xor.h
     ${CMAKE_CURRENT_LIST_DIR}/has_xor_assign.h
     PARENT_SCOPE)

