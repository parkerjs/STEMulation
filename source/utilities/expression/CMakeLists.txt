cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/balanced_expression_checker.h
     ${CMAKE_CURRENT_LIST_DIR}/expression.h
     ${CMAKE_CURRENT_LIST_DIR}/expression_binary_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/expression_evaluator.h
     ${CMAKE_CURRENT_LIST_DIR}/expression_functor.h
     ${CMAKE_CURRENT_LIST_DIR}/expression_tree.h
     ${CMAKE_CURRENT_LIST_DIR}/expression_unary_functor.h
     PARENT_SCOPE)

