cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (functors)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/any.h
     ${CMAKE_CURRENT_LIST_DIR}/holder.h
     ${CMAKE_CURRENT_LIST_DIR}/invoke.h
     ${CMAKE_CURRENT_LIST_DIR}/lambda_helper.h
     ${CMAKE_CURRENT_LIST_DIR}/tuple_for_each.h
     ${CMAKE_CURRENT_LIST_DIR}/variable_wrapper.h
     PARENT_SCOPE)

