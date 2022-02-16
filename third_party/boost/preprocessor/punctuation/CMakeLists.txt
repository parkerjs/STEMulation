cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/comma.hpp
     ${CMAKE_CURRENT_LIST_DIR}/comma_if.hpp
     ${CMAKE_CURRENT_LIST_DIR}/is_begin_parens.hpp
     PARENT_SCOPE)

