cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/eat.hpp
     ${CMAKE_CURRENT_LIST_DIR}/elem.hpp
     ${CMAKE_CURRENT_LIST_DIR}/rem.hpp
     ${CMAKE_CURRENT_LIST_DIR}/size.hpp
     ${CMAKE_CURRENT_LIST_DIR}/to_list.hpp
     PARENT_SCOPE)

