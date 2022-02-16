cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/aligned.h
     ${CMAKE_CURRENT_LIST_DIR}/garbage_collector.h
     ${CMAKE_CURRENT_LIST_DIR}/virtual_enable_shared_from_this.h
     PARENT_SCOPE)

