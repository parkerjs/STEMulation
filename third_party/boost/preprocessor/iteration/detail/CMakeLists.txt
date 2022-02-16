cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (bounds)
add_subdirectory (iter)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/finish.hpp
     ${CMAKE_CURRENT_LIST_DIR}/local.hpp
     ${CMAKE_CURRENT_LIST_DIR}/rlocal.hpp
     ${CMAKE_CURRENT_LIST_DIR}/self.hpp
     ${CMAKE_CURRENT_LIST_DIR}/start.hpp
     PARENT_SCOPE)

