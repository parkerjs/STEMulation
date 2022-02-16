cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (estimation)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/average.h
     ${CMAKE_CURRENT_LIST_DIR}/correlation.h
     ${CMAKE_CURRENT_LIST_DIR}/covariance.h
     ${CMAKE_CURRENT_LIST_DIR}/standard_deviation.h
     ${CMAKE_CURRENT_LIST_DIR}/statistical_calculator.h
     ${CMAKE_CURRENT_LIST_DIR}/statistical_variance.h
     ${CMAKE_CURRENT_LIST_DIR}/sum.h
     PARENT_SCOPE)

