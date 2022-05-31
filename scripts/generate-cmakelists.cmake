cmake_minimum_required (VERSION 3.1.0)

# python includes
find_package (Python REQUIRED)

# define a macro to list all subdirectories a root directory
macro (subdirlist result curdir)
    file (GLOB children LIST_DIRECTORIES true RELATIVE ${curdir} ${curdir}/*)
    set (dirlist "")
    foreach (child ${children})
        if (IS_DIRECTORY ${curdir}/${child})
            list (APPEND dirlist ${curdir}/${child})
        endif ()
    endforeach ()
    set (${result} ${dirlist})
endmacro ()

# retrieve a list of source code subdirectories
subdirlist (source_code_subdirectories ${CMAKE_CURRENT_SOURCE_DIR}/source)

# sort the list
list (SORT source_code_subdirectories)

# build a list of commands
set (i "0")
set (commands "")
foreach (subdir ${source_code_subdirectories})
    # build an argument list
    set (arguments "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_cmakelists.py"
                   "${subdir}")

    # add a custom command
    add_custom_command (OUTPUT command_generate_cmake_lists_${i}
                        COMMAND ${PYTHON_EXECUTABLE}
                                ${arguments})

    # mark output of custom command as symbolic
    set_source_files_properties (command_generate_cmake_lists_${i} PROPERTIES SYMBOLIC true)

    # accumulate custom commands
    list (APPEND commands command_generate_cmake_lists_${i})

    math (EXPR i "${i} + 1")
endforeach ()

# add a custom target to automatically generate CMakeLists in the source code directory tree
add_custom_target (generate_cmakelists DEPENDS "${commands}")
