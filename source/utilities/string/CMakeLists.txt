cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/cpp_comment_stripper.h
     ${CMAKE_CURRENT_LIST_DIR}/line_getter.h
     ${CMAKE_CURRENT_LIST_DIR}/string_replacer.h
     ${CMAKE_CURRENT_LIST_DIR}/string_utilities.h
     PARENT_SCOPE)

