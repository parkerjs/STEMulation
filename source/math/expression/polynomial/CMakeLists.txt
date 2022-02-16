cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (solvers)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/bernstein.h
     ${CMAKE_CURRENT_LIST_DIR}/polynomial.h
     PARENT_SCOPE)

