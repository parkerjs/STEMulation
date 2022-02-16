cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/export_library.h
     ${CMAKE_CURRENT_LIST_DIR}/library_interface.h
     PARENT_SCOPE)

