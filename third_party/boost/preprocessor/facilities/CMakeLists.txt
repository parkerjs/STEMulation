cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/empty.hpp
     ${CMAKE_CURRENT_LIST_DIR}/expand.hpp
     ${CMAKE_CURRENT_LIST_DIR}/identity.hpp
     ${CMAKE_CURRENT_LIST_DIR}/intercept.hpp
     ${CMAKE_CURRENT_LIST_DIR}/is_1.hpp
     ${CMAKE_CURRENT_LIST_DIR}/is_empty.hpp
     ${CMAKE_CURRENT_LIST_DIR}/is_empty_variadic.hpp
     ${CMAKE_CURRENT_LIST_DIR}/overload.hpp
     PARENT_SCOPE)

