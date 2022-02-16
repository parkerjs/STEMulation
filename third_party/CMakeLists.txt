cmake_minimum_required (VERSION 3.7.0)

# set default locations for the local boost include directories stored within the repo;
# these will get overridden the minute find_package () is called
set (Local_Repo_Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set (Local_Repo_Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})

# find boost
find_package (Boost QUIET)

# if boost package not found, look for it in included third_party libraries
if (NOT Boost_FOUND)
    message (STATUS "System installation of Boost C++ libraries not found.")

    # add additional sub-directories with more CMakeLists
    add_subdirectory (boost)
else ()
    message (STATUS "System installation of Boost C++ libraries found.")
endif ()

if (Boost_FOUND)
    message (STATUS "Found Boost include directory: ${Boost_INCLUDE_DIRS}")
    message (STATUS "Found Boost library directory: ${Boost_LIBRARY_DIRS}")

    # add additional include paths
    set (third_party_library_includes
         ${third_party_library_includes}
         ${Boost_INCLUDE_DIRS})
endif ()

# find JsonCpp
# TODO: write a script to find JsonCpp
find_package (JsonCpp QUIET)

# if jsoncpp package not found, look for it in included third_party libraries
if (NOT JsonCpp_FOUND)
    # add additional sub-directories with more CMakeLists
    add_subdirectory (jsoncpp)
endif ()

if (JsonCpp_FOUND)
    message (STATUS "Found JsonCpp include directory: ${JsonCpp_INCLUDE_DIR}")

    # add additional include paths
    set (third_party_library_includes
         ${third_party_library_includes}
         ${JsonCpp_INCLUDE_DIR}) 
endif ()

# find RapidXML
# TODO: write a script to find RapidXML
find_package (RapidXML QUIET)

# if RapidXML package not found, look for it in included third_party libraries
if (NOT RapidXML_FOUND)
    # add additional sub-directories with more CMakeLists
    add_subdirectory (rapidxml)
endif ()

if (RapidXML_FOUND)
    message (STATUS "Found RapidXML include directory: ${RapidXML_INCLUDE_DIR}")

    # add additional include paths
    set (third_party_library_includes
         ${third_party_library_includes}
         ${RapidXML_INCLUDE_DIR})    
endif ()

# relay list of include paths to parent scope
set (third_party_library_includes
     ${third_party_library_includes}
     PARENT_SCOPE)
