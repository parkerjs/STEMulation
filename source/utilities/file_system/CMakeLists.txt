cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/directory_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/directory_traverser.h
     ${CMAKE_CURRENT_LIST_DIR}/file_system.h
     ${CMAKE_CURRENT_LIST_DIR}/posix_directory_traverser.h
     ${CMAKE_CURRENT_LIST_DIR}/windows_directory_traverser.h
     PARENT_SCOPE)

