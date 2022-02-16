cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (arithmetic)
add_subdirectory (bitwise)
add_subdirectory (comparison)
add_subdirectory (logical)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/operator.h
     PARENT_SCOPE)

