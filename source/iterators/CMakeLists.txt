cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/bidirectional_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/forward_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/inherited_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/input_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/output_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/random_access_iterator.h
     PARENT_SCOPE)

