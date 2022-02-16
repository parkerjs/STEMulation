cmake_minimum_required (VERSION 3.7.0)

# set the project name
project (STEMulation++)

# set target for this build
set (TARGET ${PROJECT_NAME})

# try to find OpenSSL
if (APPLE)
    # Apple has deprecated OpenSSL, so the following assumes OpenSSL was installed using Homebrew
    set (OPENSSL_ROOT_DIR /usr/local/opt/openssl)
    set (OPENSSL_LIBRARIES /usr/local/ssl/lib)
endif ()

# find required packages
if (WIN32 AND MINGW)
    find_package (OpenSSL QUIET PATHS "C:\\msys64\\mingw64\\lib\\")
else ()
    find_package (OpenSSL QUIET)
endif ()

# the following is an attempt to force cmake to find matching ssl and crypto libraries for mingw
if (MINGW AND OpenSSL_FOUND)
    # get the absolute path of the crypto library
    get_filename_component (openssl_crypto_library_path "${OPENSSL_CRYPTO_LIBRARY}" PATH)

    # get the absolute path of the ssl library
    get_filename_component (openssl_ssl_library_path "${OPENSSL_SSL_LIBRARY}" PATH)

    if (NOT "${openssl_crypto_library_path}" MATCHES "${openssl_ssl_library_path}")
        # get the absolute path of the crypto library
        get_filename_component (openssl_crypto_library_ext "${OPENSSL_CRYPTO_LIBRARY}" EXT)

        # get the absolute path of the ssl library
        get_filename_component (openssl_ssl_library_ext "${OPENSSL_SSL_LIBRARY}" EXT)

        if (NOT "${openssl_crypto_library_ext}" MATCHES "${openssl_ssl_library_ext}")
            message (STATUS "${openssl_crypto_library_ext}")
            message (STATUS "${openssl_ssl_library_ext}")
            message (STATUS "${openssl_crypto_library_path}")
            message (STATUS "${openssl_ssl_library_path}")
            message (STATUS "${CMAKE_FIND_LIBRARY_SUFFIXES}")

            find_package (OpenSSL QUIET HINTS PATHS ${openssl_ssl_library_path})

            message (STATUS "${OPENSSL_CRYPTO_LIBRARY}")
            message (STATUS "${OPENSSL_SSL_LIBRARY}")
        endif ()
    endif ()
endif ()

#set (CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".a" ".lib")

if (OpenSSL_FOUND)
    message (STATUS "Found OpenSSL include directory: ${OPENSSL_INCLUDE_DIR}")
    message (STATUS "Found OpenSSL library directory: ${OPENSSL_LIBRARIES}")
    set (OPENSSL_USE_STATIC_LIBS true)
    set (OPENSSL_MSVC_STATIC_RT true)
    if (WIN32 AND "${CMAKE_MAKE_PROGRAM}" STREQUAL "${JOM}")
        # qmake/jom cannot handle spaces in paths unless they are surrounded by double quotes, and OpenSSL
        # on windows is likely to be installed in Program Files; but CMake instead wants to escape the space characters;
        # to get around this issue, use the filesystem's shortened path name to the include and library directories
        if (DEFINED OPENSSL_INCLUDE_DIR)
            STRING (REGEX REPLACE "Program Files \\(x86\\)" "PROGRA~2" OPENSSL_INCLUDE_DIR "${OPENSSL_INCLUDE_DIR}")
            STRING (REGEX REPLACE "Program Files" "PROGRA~1" OPENSSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR})
        endif ()
        if (DEFINED OPENSSL_LIBRARIES)
            STRING (REGEX REPLACE "Program Files \\(x86\\)" "PROGRA~2" OPENSSL_LIBRARIES "${OPENSSL_LIBRARIES}")
            STRING (REGEX REPLACE "Program Files" "PROGRA~1" OPENSSL_LIBRARIES "${OPENSSL_LIBRARIES}")
        endif ()
    endif ()
    include_directories (${OPENSSL_INCLUDE_DIR})
endif ()

# initialize the list of sources
set (sources "")

# add additional sub-directories with more CMakeLists
add_subdirectory (attributes)
add_subdirectory (containers)
add_subdirectory (functional)
add_subdirectory (iterators)
add_subdirectory (library)
add_subdirectory (math)
add_subdirectory (memory)
add_subdirectory (messaging)
add_subdirectory (networking)
add_subdirectory (physics)
add_subdirectory (traits)
add_subdirectory (utilities)

# sort the list of sources
list (SORT sources)

# get the absolute path of the parent folder
get_filename_component (parent_absolute_path ${CMAKE_CURRENT_SOURCE_DIR}/../ ABSOLUTE)

# determine include paths, define source groups
foreach (source IN LISTS sources)
    get_filename_component (group "${source}" PATH)
    string (REPLACE ${parent_absolute_path} ".." group ${group})
    set (include_paths
         ${include_paths}
         ${group})
    string (REPLACE ".." "" group ${group})
    string (REPLACE "/" "\\" group ${group})
    source_group ("${group}" FILES "${source}")
endforeach ()

message (STATUS "Third Party Libraries: ${third_party_library_includes}")

# add additional include paths
set (include_paths
     ${include_paths}
     ${third_party_library_includes})

# remove duplicates from include paths
list (REMOVE_DUPLICATES include_paths)

# sort the list of include paths
list (SORT include_paths)

# add include paths
foreach (include_path IN LISTS include_paths)
    include_directories (${include_path})
endforeach ()

# add a shared library
add_library (${TARGET} SHARED ${sources})

if (WIN32)
    # add windows sockets libraries
    set (windows_sockets_libraries wsock32 ws2_32)

    # set libgcc, libstdc++ as static
    if (MINGW)
        target_link_libraries (${TARGET}
                               -static-libgcc
                               -static-libstdc++)
    endif ()
endif ()

target_link_libraries (${TARGET}
                       ${windows_sockets_libraries}
                       ${STATIC_LIBRARIES})

if (OpenSSL_FOUND)
    target_link_libraries (${TARGET}
                           ${OPENSSL_LIBRARIES})
endif ()

# organize header include files, if enabled
#organize_header_includes (${OPTION_ORGANIZE_HEADER_FILES})

# if compiling on windows, make sure to define UNICODE
if (WIN32)
    target_compile_definitions (${TARGET} PRIVATE
                                UNICODE)
endif ()

# set platform-specific compiler flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
    target_compile_definitions (${TARGET} PRIVATE
                                _CRT_SECURE_NO_WARNINGS
                                _SCL_SECURE_NO_WARNINGS
                                NOMINMAX)
endif ()

# set target compile definitions
target_compile_definitions (${TARGET} PRIVATE
                            STEMULATION_SHARED_LIBRARY
                            EXPORT_STEMULATION_DLL)

# include jsoncpp, if found
if (jsoncpp_FOUND)
    message (STATUS "Found jsoncpp include directory: ${jsoncpp_INCLUDE_DIR}")
    include_directories (${jsoncpp_INCLUDE_DIR})

    # set target compile definitions
    target_compile_definitions (${TARGET} PRIVATE
                                JSONCPP)
endif ()

# define target properties
set_target_properties (${TARGET}
                       PROPERTIES CXX_STANDARD 17
                                  FRAMEWORK TRUE)

if (NOT APPLE)
    set_target_properties (${TARGET}
                           PROPERTIES DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else ()
    # DEBUG_POSTFIX doesn't seem to work with MACOSX_BUNDLE enabled, do the following instead
    set_target_properties (${TARGET}
                           PROPERTIES DEBUG_OUTPUT_NAME "${TARGET}${CMAKE_DEBUG_POSTFIX}")
endif ()

# relay list of sources to the parent scope
set (sources
     ${sources}
     PARENT_SCOPE)

install (TARGETS ${TARGET}
         DESTINATION ${CMAKE_SOURCE_DIR}/${BINARY_INSTALL_DIR})

