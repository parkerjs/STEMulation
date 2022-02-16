cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/all_of.h
     ${CMAKE_CURRENT_LIST_DIR}/any_of.h
     ${CMAKE_CURRENT_LIST_DIR}/function_traits.h
     ${CMAKE_CURRENT_LIST_DIR}/index_of.h
     ${CMAKE_CURRENT_LIST_DIR}/iterator_traits.h
     ${CMAKE_CURRENT_LIST_DIR}/member_function_pointer_helper.h
     ${CMAKE_CURRENT_LIST_DIR}/pointer_unwrapper.h
     ${CMAKE_CURRENT_LIST_DIR}/reference_unwrapper.h
     ${CMAKE_CURRENT_LIST_DIR}/size.h
     PARENT_SCOPE)

