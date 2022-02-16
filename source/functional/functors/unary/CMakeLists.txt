cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/bitwise_not_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/logical_not_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/unary_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/unary_functor_type.h
     ${CMAKE_CURRENT_LIST_DIR}/unary_functors.h
     ${CMAKE_CURRENT_LIST_DIR}/unary_minus_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/unary_plus_functor.h
     PARENT_SCOPE)

