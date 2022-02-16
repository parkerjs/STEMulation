cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/jenkins_traub.h
     ${CMAKE_CURRENT_LIST_DIR}/lowOrderPolynomialSolver.cpp
     ${CMAKE_CURRENT_LIST_DIR}/lowOrderPolynomialSolver.h
     PARENT_SCOPE)

