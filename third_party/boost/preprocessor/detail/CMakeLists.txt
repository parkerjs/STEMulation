cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (dmc)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/auto_rec.hpp
     ${CMAKE_CURRENT_LIST_DIR}/check.hpp
     ${CMAKE_CURRENT_LIST_DIR}/is_binary.hpp
     ${CMAKE_CURRENT_LIST_DIR}/split.hpp
     PARENT_SCOPE)

