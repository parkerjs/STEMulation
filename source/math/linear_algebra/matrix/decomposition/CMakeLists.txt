cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/cholesky.h
     ${CMAKE_CURRENT_LIST_DIR}/column_pivot_strategy.h
     ${CMAKE_CURRENT_LIST_DIR}/crout_lu.h
     ${CMAKE_CURRENT_LIST_DIR}/doolittle_lu.h
     ${CMAKE_CURRENT_LIST_DIR}/linear_solver.h
     ${CMAKE_CURRENT_LIST_DIR}/lu.h
     ${CMAKE_CURRENT_LIST_DIR}/pivot_type.h
     ${CMAKE_CURRENT_LIST_DIR}/qr.h
     ${CMAKE_CURRENT_LIST_DIR}/triangular_matrix_type.h
     ${CMAKE_CURRENT_LIST_DIR}/tridiag_lu.h
     PARENT_SCOPE)

