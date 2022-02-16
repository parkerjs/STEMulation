cmake_minimum_required (VERSION 3.1.0)

foreach (source_file ${source_files})
    # get the containing folder and filename
    get_filename_component (parent_absolute_path ${source_file} DIRECTORY)
    get_filename_component (base_filename ${source_file} NAME)
    
    # extract sub-directories of the source directory
    string (REPLACE ${CMAKE_SOURCE_DIR}/ "" source_subdir ${parent_absolute_path})
    
    # build the destination filename
    set (destination_file 
         ${destination_dir}/${source_subdir}/${base_filename})

    message ("${destination_file}, ${source_file}, ${destination_dir}/${source_subdir}")

    if (NOT EXISTS ${destination_file} OR ${source_file} IS_NEWER_THAN ${destination_file})
        file (COPY ${source_file} DESTINATION ${destination_dir}/${source_subdir})
    endif ()
endforeach ()
