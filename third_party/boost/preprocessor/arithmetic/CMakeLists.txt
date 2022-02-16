cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/add.hpp
     ${CMAKE_CURRENT_LIST_DIR}/dec.hpp
     ${CMAKE_CURRENT_LIST_DIR}/inc.hpp
     ${CMAKE_CURRENT_LIST_DIR}/mod.hpp
     ${CMAKE_CURRENT_LIST_DIR}/sub.hpp
     PARENT_SCOPE)

