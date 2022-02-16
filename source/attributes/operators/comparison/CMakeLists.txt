cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/comparable.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_equal_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_greater_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_greater_equal_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_less_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_less_equal_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/compare_not_equal_attribute.h
     PARENT_SCOPE)

