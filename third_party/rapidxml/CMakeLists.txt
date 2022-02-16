cmake_minimum_required (VERSION 3.7.0)

# set the project name
project (rapidxml)

# set RapidXML_FOUND, RapidXML_INCLUDE_DIR, RapidXML_DIR for GUI
set (RapidXML_FOUND
     TRUE
     PARENT_SCOPE)
set (RapidXML_INCLUDE_DIR
     ${CMAKE_CURRENT_LIST_DIR}
     PARENT_SCOPE)
set (RapidXML_DIR
     ${CMAKE_CURRENT_LIST_DIR}
     CACHE PATH "" FORCE)

