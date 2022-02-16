cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (estimation)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/digital_filter.h
     ${CMAKE_CURRENT_LIST_DIR}/exponential_moving_average.h
     ${CMAKE_CURRENT_LIST_DIR}/moving_average.h
     ${CMAKE_CURRENT_LIST_DIR}/moving_correlation.h
     ${CMAKE_CURRENT_LIST_DIR}/moving_covariance.h
     ${CMAKE_CURRENT_LIST_DIR}/moving_standard_deviation.h
     ${CMAKE_CURRENT_LIST_DIR}/moving_sum.h
     ${CMAKE_CURRENT_LIST_DIR}/moving_variance.h
     ${CMAKE_CURRENT_LIST_DIR}/sliding_window_extremum.h
     ${CMAKE_CURRENT_LIST_DIR}/sliding_window_extremum_index.h
     PARENT_SCOPE)

