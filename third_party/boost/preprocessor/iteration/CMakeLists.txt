cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/iterate.hpp
     ${CMAKE_CURRENT_LIST_DIR}/local.hpp
     ${CMAKE_CURRENT_LIST_DIR}/self.hpp
     PARENT_SCOPE)

