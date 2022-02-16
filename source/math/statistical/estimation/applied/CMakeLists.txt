cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/convertedRadarEstimateTrackFilter.cpp
     ${CMAKE_CURRENT_LIST_DIR}/convertedRadarEstimateTrackFilter.h
     ${CMAKE_CURRENT_LIST_DIR}/convertedRadarMeasurementTrackFilter.cpp
     ${CMAKE_CURRENT_LIST_DIR}/convertedRadarMeasurementTrackFilter.h
     ${CMAKE_CURRENT_LIST_DIR}/radar_measurement_type.h
     ${CMAKE_CURRENT_LIST_DIR}/radarMeasurement.cpp
     ${CMAKE_CURRENT_LIST_DIR}/radarMeasurement.h
     ${CMAKE_CURRENT_LIST_DIR}/radarTrackEstimationFilterUser.cpp
     ${CMAKE_CURRENT_LIST_DIR}/radarTrackEstimationFilterUser.h
     ${CMAKE_CURRENT_LIST_DIR}/radarTrackFilter.cpp
     ${CMAKE_CURRENT_LIST_DIR}/radarTrackFilter.h
     PARENT_SCOPE)

