cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/decimal_place_value_type.h
     ${CMAKE_CURRENT_LIST_DIR}/place_value_type.h
     ${CMAKE_CURRENT_LIST_DIR}/whole_place_value_type.h
     PARENT_SCOPE)

