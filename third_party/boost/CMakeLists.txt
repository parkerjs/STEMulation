cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (preprocessor)

# set Boost_FOUND, Boost_INCLUDE_DIRS, Boost_DIR for GUI
set (Boost_FOUND
     TRUE
     PARENT_SCOPE)
set (Boost_INCLUDE_DIRS
     ${CMAKE_CURRENT_LIST_DIR}/../
     PARENT_SCOPE)
set (Boost_DIR
     ${CMAKE_CURRENT_LIST_DIR}/../
     CACHE PATH "" FORCE)

