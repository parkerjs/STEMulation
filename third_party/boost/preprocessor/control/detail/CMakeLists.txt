cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (dmc)
add_subdirectory (edg)
add_subdirectory (msvc)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/while.hpp
     PARENT_SCOPE)

