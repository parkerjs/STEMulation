cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_and_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_binary_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_left_shift_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_or_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_right_shift_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_xor_functor.h
     PARENT_SCOPE)

