import os
import sys

##############################################################################################################
# This script will generate CMakeLists for each subdirectory containing source code in a directory tree
##############################################################################################################
directoryTree = os.path.abspath(sys.argv[1])
print('Directory Tree = ' + directoryTree)

for root, subdirs, files in os.walk(directoryTree):
    files.sort(key = lambda x : x.lower())
    subdirs.sort(key = lambda x : x.lower())
    print('--\nroot = ' + root)
    list_file_path = os.path.join(root,
                                  'CMakeLists.txt')
    print('list_file_path = ' + list_file_path)
    with open(list_file_path, 'w') as list_file:
        list_file.write('cmake_minimum_required (VERSION 3.7.0)\n')
        if subdirs:
            list_file.write('\n# add additional subdirectories with more CMakeLists\n')

        for subdir in subdirs:
            list_file.write('add_subdirectory (' + subdir + ')\n')
            print('\t- subdirectory ' + subdir)

        if files:
            list_file.write('\n# add sources to the project\n')
            list_file.write('set (sources\n')
            list_file.write('     ${sources}\n')

        for filename in files:
            if filename.lower().endswith(('.c', '.cpp', '.h', '.hpp')):
                list_file.write('     ${CMAKE_CURRENT_LIST_DIR}/' + filename + '\n')
                file_path = os.path.join(root,
                                         filename)
        if files:
            list_file.write('     PARENT_SCOPE)\n')
