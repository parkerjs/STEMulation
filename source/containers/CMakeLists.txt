cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (tree)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/entry_container.h
     ${CMAKE_CURRENT_LIST_DIR}/identifier_and_time_sorted_container.h
     ${CMAKE_CURRENT_LIST_DIR}/identifier_sorted_container.h
     ${CMAKE_CURRENT_LIST_DIR}/time_sorted_container.h
     PARENT_SCOPE)

