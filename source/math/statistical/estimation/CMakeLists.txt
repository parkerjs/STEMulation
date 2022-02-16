cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (applied)
add_subdirectory (kalman)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/appliedEstimationFilter.cpp
     ${CMAKE_CURRENT_LIST_DIR}/appliedEstimationFilter.h
     ${CMAKE_CURRENT_LIST_DIR}/estimationFilter.cpp
     ${CMAKE_CURRENT_LIST_DIR}/estimationFilter.h
     ${CMAKE_CURRENT_LIST_DIR}/estimationFilterUser.cpp
     ${CMAKE_CURRENT_LIST_DIR}/estimationFilterUser.h
     ${CMAKE_CURRENT_LIST_DIR}/latestMeasurementStrategy.cpp
     ${CMAKE_CURRENT_LIST_DIR}/latestMeasurementStrategy.h
     ${CMAKE_CURRENT_LIST_DIR}/leastSquaresMeasurementStrategy.cpp
     ${CMAKE_CURRENT_LIST_DIR}/leastSquaresMeasurementStrategy.h
     ${CMAKE_CURRENT_LIST_DIR}/measurementAggregationStrategy.cpp
     ${CMAKE_CURRENT_LIST_DIR}/measurementAggregationStrategy.h
     ${CMAKE_CURRENT_LIST_DIR}/measurementAveragingStrategy.cpp
     ${CMAKE_CURRENT_LIST_DIR}/measurementAveragingStrategy.h
     PARENT_SCOPE)
