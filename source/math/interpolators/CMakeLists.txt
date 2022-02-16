cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/interpolator.h
     ${CMAKE_CURRENT_LIST_DIR}/linear_interpolator.h
     ${CMAKE_CURRENT_LIST_DIR}/prefix_tree_interpolator.h
     PARENT_SCOPE)

