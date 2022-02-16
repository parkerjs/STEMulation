cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (decomposition)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/arithmetic_matrix_operations.h
     ${CMAKE_CURRENT_LIST_DIR}/complex_matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/complex_matrix_2d.h
     ${CMAKE_CURRENT_LIST_DIR}/complex_matrix_nd.h
     ${CMAKE_CURRENT_LIST_DIR}/forward_matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/general_matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/general_matrix_2d.h
     ${CMAKE_CURRENT_LIST_DIR}/general_matrix_nd.h
     ${CMAKE_CURRENT_LIST_DIR}/matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/matrix_dimension_type.h
     ${CMAKE_CURRENT_LIST_DIR}/matrix2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/matrix2d.h
     ${CMAKE_CURRENT_LIST_DIR}/matrix3x3.cpp
     ${CMAKE_CURRENT_LIST_DIR}/matrix3x3.h
     ${CMAKE_CURRENT_LIST_DIR}/numeric_matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/numeric_matrix_2d.h
     ${CMAKE_CURRENT_LIST_DIR}/numeric_matrix_nd.h
     ${CMAKE_CURRENT_LIST_DIR}/real_matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/real_matrix_2d.h
     ${CMAKE_CURRENT_LIST_DIR}/real_matrix_nd.h
     ${CMAKE_CURRENT_LIST_DIR}/reference_matrix.h
     ${CMAKE_CURRENT_LIST_DIR}/reference_matrix_2d.h
     ${CMAKE_CURRENT_LIST_DIR}/reference_matrix_nd.h
     PARENT_SCOPE)

