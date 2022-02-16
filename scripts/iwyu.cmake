cmake_minimum_required (VERSION 3.1.0)

# try to find program "include-what-you-use" include file organizer
find_program (EXE_INCLUDE_WHAT_YOU_USE NAMES include-what-you-use iwyu)
if (NOT EXE_INCLUDE_WHAT_YOU_USE)
    set (include_what_you_use_found OFF)
else ()
    set (include_what_you_use_found ON)
endif ()

# function to set CMAKE's CXX_INCLUDE_WHAT_YOU_USE property, if header organization is enabled
function (organize_header_includes enabled)
    if (NOT enabled)
        return ()
    endif ()

    if (NOT include_what_you_use_found)
        message (FATAL_ERROR "Include-what-you-use is needed to organize header includes.")
    else ()
        set_property (TARGET ${PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${EXE_INCLUDE_WHAT_YOU_USE})
    endif ()
endfunction ()
