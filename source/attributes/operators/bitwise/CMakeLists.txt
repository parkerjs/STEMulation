cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_and_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_and_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_attributes.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_left_shiftable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_left_shift_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_or_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_or_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_right_shiftable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_right_shift_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_xor_assignable.h
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_xor_attribute.h
     PARENT_SCOPE)

