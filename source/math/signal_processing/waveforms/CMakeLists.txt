cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/chirp.h
     ${CMAKE_CURRENT_LIST_DIR}/delta.h
     ${CMAKE_CURRENT_LIST_DIR}/sinusoid.h
     ${CMAKE_CURRENT_LIST_DIR}/square.h
     ${CMAKE_CURRENT_LIST_DIR}/step.h
     ${CMAKE_CURRENT_LIST_DIR}/waveform.h
     PARENT_SCOPE)

