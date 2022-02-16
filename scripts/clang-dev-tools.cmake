cmake_minimum_required (VERSION 3.1.0)

# try to find clang-format
find_program (EXE_CLANG_FORMAT "clang-format")

# clean/remove the current formatted source code directory, if it exists
add_custom_command (OUTPUT clean_formatted_source
                    COMMAND ${CMAKE_COMMAND} -E rm -Rf
                    "${CMAKE_BINARY_DIR}/formatted_source_code")

# mark output of custom command as symbolic
set_source_files_properties (clean_formatted_source PROPERTIES SYMBOLIC true)

if (EXE_CLANG_FORMAT)
    set (clang_format_found ON)
    set (path_to_clang_format_cfg "${CMAKE_SOURCE_DIR}/configuration/.clang-format")

    # copy the clang-format configuration file to the destination directory                            
    if (EXISTS "${path_to_clang_format_cfg}")
        # get the filename of the clang format file from the path
        get_filename_component (clang_format_cfg ${path_to_clang_format_cfg} NAME)
                                    
        add_custom_command (OUTPUT clean_formatted_source APPEND
                            COMMAND ${CMAKE_COMMAND} -E rm -f
                            ${CMAKE_BINARY_DIR}/${clang_format_cfg})

        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${clang_format_cfg}
                            COMMAND ${CMAKE_COMMAND} -E copy ${path_to_clang_format_cfg} ${CMAKE_BINARY_DIR}
                            DEPENDS clean_formatted_source)
    endif ()

    message (STATUS "Found clang-format: ${EXE_CLANG_FORMAT}")

    # build an argument list for clang-format
    set (clang_format_arguments "-style=file"
                                "-i"
                                "${partial_file_list}")
else ()
    set (clang_format_found OFF)
endif ()

# try to find clang-tidy
find_program (EXE_CLANG_TIDY "clang-tidy")

if (EXE_CLANG_TIDY)
    set (clang_tidy_found ON)
    set (path_to_clang_tidy_cfg "${CMAKE_SOURCE_DIR}/configuration/.clang-tidy")

    # copy the clang-format configuration file to the destination directory                            
    if (EXISTS "${path_to_clang_tidy_cfg}")    
        # get the filename of the clang tidy file from the path
        get_filename_component (clang_tidy_cfg ${path_to_clang_tidy_cfg} NAME)

        add_custom_command (OUTPUT clean_formatted_source APPEND
                            COMMAND ${CMAKE_COMMAND} -E rm -f
                            ${CMAKE_BINARY_DIR}/${clang_tidy_cfg})

        add_custom_command (OUTPUT ${CMAKE_BINARY_DIR}/${clang_tidy_cfg}
                            COMMAND ${CMAKE_COMMAND} -E copy ${path_to_clang_tidy_cfg} ${CMAKE_BINARY_DIR}
                            DEPENDS clean_formatted_source)
    endif ()

    message (STATUS "Found clang-tidy: ${EXE_CLANG_TIDY}")

    # build an argument list for clang-format
    set (clang_tidy_arguments "-style=file"
                              "-i"
                              "${partial_file_list}")
else ()
    set (clang_tidy_found OFF)
endif ()

# TODO add comment
function (add_dev_tool dev_tool_exe arguments source_files)
    # passing all files to command line at once produces an argument string that is too
    # long, so it must be broken up into multiple commands
    set (num_files_per_command 25)
    set (i "0")
    set (k "0")
    set (${dev_tool}_commands "")
    list (LENGTH source_files num_files)

    # determine top-level source folders from list of input source files and build a destination file list
    foreach (source_file IN LISTS source_files)
        # get the containing folder and filename
        get_filename_component (parent_absolute_path ${source_file} DIRECTORY)
        get_filename_component (filename ${source_file} NAME)

        # extract top-level source folders
        string (REPLACE ${CMAKE_SOURCE_DIR}/ "" top_level_source_path ${parent_absolute_path})

        # build the destination filename
        set (destination_file 
             ${CMAKE_BINARY_DIR}/formatted_source_code/${top_level_source_path}/${filename})

        # accumulate list of destination files
        list (APPEND destination_files ${destination_file})

        string (REGEX REPLACE "([^/\\]+).*" "\\1" top_level_source_path ${top_level_source_path})
        list (APPEND top_level_source_paths "${top_level_source_path}")
    endforeach ()

    # remove duplicates from include paths
    list (REMOVE_DUPLICATES top_level_source_paths)
    
    # get the base name of the dev tool without the path and without the extension
    get_filename_component (dev_tool ${dev_tool_exe} NAME_WLE)
    
    # replace non-alphanum characters in the dev tool name
    string (REGEX REPLACE "[^a-zA-Z_0-9]" "-" dev_tool ${dev_tool})
    
     # add a command to copy top-level source files to destination directory prior to formatting
    add_custom_command (OUTPUT copy_files_for_${dev_tool}
                        DEPENDS clean_formatted_source)

    foreach (top_level_source_path IN LISTS top_level_source_paths)
        add_custom_command (OUTPUT copy_files_for_${dev_tool} APPEND
                            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${top_level_source_path}"
                                                                       "${CMAKE_BINARY_DIR}/formatted_source_code/${top_level_source_path}"
                            DEPENDS clean_formatted_source)
    endforeach ()

    while (i LESS ${num_files})
        math (EXPR i "${i} + ${num_files_per_command}")

        # build a partial list of files for passage to command line
        set (j "0")
        while (j LESS ${num_files_per_command})
            if (k GREATER ${num_files} OR
                k EQUAL ${num_files})
                break ()
            endif ()

            # get the kth file from the list           
            list (GET destination_files ${k} destination_file) 

            set (partial_file_list ${partial_file_list} ${destination_file})
            math (EXPR j "${j} + 1")
            math (EXPR k "${k} + 1")
        endwhile ()

        # add a custom command to run source code formatter
        math (EXPR j "${i} / ${num_files_per_command}")
        add_custom_command (OUTPUT ${dev_tool}_command_${j}
                            DEPENDS copy_files_for_${dev_tool})

        # target dependencies are assumed to be in the optional arguments
        set(dependencies "${ARGN}")

        list (APPEND arguments ${partial_file_list})

        # add a custom command for the partial file list
        add_custom_command (OUTPUT ${dev_tool}_command_${j} APPEND
                            COMMAND ${dev_tool_exe}
                                    ${arguments}
                            DEPENDS ${dependencies}
                                    copy_files_for_${dev_tool})

        # mark output of custom command as symbolic
        set_source_files_properties (${dev_tool}_command_${j} PROPERTIES SYMBOLIC true)

        # accumulate custom commands
        list (APPEND ${dev_tool}_commands ${dev_tool}_command_${j})

        unset (partial_file_list)
    endwhile ()

    # add a custom target to perform source code formatting
    add_custom_target (run-${dev_tool} DEPENDS "${${dev_tool}_commands}")
endfunction ()
