cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (abstract)
add_subdirectory (concrete)
add_subdirectory (interfaces)
add_subdirectory (operators)

# add sources to the project
set (sources
     ${sources}
     PARENT_SCOPE)

