cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/adt.hpp
     ${CMAKE_CURRENT_LIST_DIR}/append.hpp
     ${CMAKE_CURRENT_LIST_DIR}/fold_left.hpp
     ${CMAKE_CURRENT_LIST_DIR}/fold_right.hpp
     ${CMAKE_CURRENT_LIST_DIR}/for_each_i.hpp
     ${CMAKE_CURRENT_LIST_DIR}/reverse.hpp
     ${CMAKE_CURRENT_LIST_DIR}/transform.hpp
     PARENT_SCOPE)

