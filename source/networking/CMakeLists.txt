cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (authentication)
add_subdirectory (sockets)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/downloader.cpp
     ${CMAKE_CURRENT_LIST_DIR}/downloader.h
     ${CMAKE_CURRENT_LIST_DIR}/downloaderPreferences.cpp
     ${CMAKE_CURRENT_LIST_DIR}/downloaderPreferences.h
     ${CMAKE_CURRENT_LIST_DIR}/httpHeaders.cpp
     ${CMAKE_CURRENT_LIST_DIR}/httpHeaders.h
     ${CMAKE_CURRENT_LIST_DIR}/httpRequestMethod.h
     ${CMAKE_CURRENT_LIST_DIR}/transportLayerProtocol.h
     ${CMAKE_CURRENT_LIST_DIR}/URL.cpp
     ${CMAKE_CURRENT_LIST_DIR}/URL.h
     ${CMAKE_CURRENT_LIST_DIR}/URL_Field.h
     ${CMAKE_CURRENT_LIST_DIR}/URL_Parser.cpp
     ${CMAKE_CURRENT_LIST_DIR}/URL_Parser.h
     PARENT_SCOPE)

