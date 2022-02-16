cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/deduce_d.hpp
     ${CMAKE_CURRENT_LIST_DIR}/expr_if.hpp
     ${CMAKE_CURRENT_LIST_DIR}/expr_iif.hpp
     ${CMAKE_CURRENT_LIST_DIR}/if.hpp
     ${CMAKE_CURRENT_LIST_DIR}/iif.hpp
     ${CMAKE_CURRENT_LIST_DIR}/while.hpp
     PARENT_SCOPE)

