cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/date.h
     ${CMAKE_CURRENT_LIST_DIR}/date_mmddyyyy.h
     ${CMAKE_CURRENT_LIST_DIR}/date_yyyymmdd.h
     PARENT_SCOPE)

