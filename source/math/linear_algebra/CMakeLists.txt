cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (matrix)
add_subdirectory (transformation)
add_subdirectory (vector)

# add sources to the project
set (sources
     ${sources}
     PARENT_SCOPE)

