#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "size.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "string_replacer.h"
#include "string_utilities.h"
#include <fstream>
#include <regex>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include "windows.h"
#include <direct.h>
#define getcwd _getcwd
#define mkdir(path) _mkdir(path)
#elif defined POSIX
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#define mkdir(path) mkdir(path, 0777)
#endif

namespace utilities
{

namespace file_system
{

/**
 * This class implements a Windows/Posix cross-platform utility that provides methods to query and manipulate
 * files and paths within file-system
 */
class FileSystem final
: public attributes::concrete::StaticMutexMappable<FileSystem, int, std::mutex>,
  private attributes::concrete::StaticSynchronizable<FileSystem>
{
public:

    /**
     * Enumerations
     */
    enum class PathSeparatorType { BackSlash, ForwardSlash };

    /**
     * Constructor
     */
    FileSystem(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    FileSystem(const FileSystem &system) = delete;

    /**
     * Move constructor
     */
    FileSystem(FileSystem &&system) = delete;

public:

    /**
     * Destructor
     */
    virtual ~FileSystem(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    FileSystem &operator = (const FileSystem &system) = delete;

    /**
     * Move assignment operator
     */
    FileSystem &operator = (FileSystem &&system) = delete;

public:

    /**
     * Convert the path separator to the indicated type
     * @param path the path to a file or folder on the file-system
     * @param type the desired path separator (BackSlash, ForwardSlash)
     */
    template<typename T>
    static bool convertPathSeparator(T &path, const PathSeparatorType &type)
    {
        switch (type)
        {
            case PathSeparatorType::BackSlash:
            std::regex_replace(path.begin(), path.begin(), path.end(), std::regex("/"), "\\");
            break;

            case PathSeparatorType::ForwardSlash:
            std::regex_replace(path.begin(), path.begin(), path.end(), std::regex("\\\\"), "/");
            break;

            default:
            return false;
        }

        return true;
    }

    /**
     * Function to create a directory. Return true upon successful creation, or if the directory already exists
     */
    template<typename T>
    static bool createDirectory(T &&path)
    {
        bool bSuccess = true;
        if (!directoryExists(std::forward<T>(path)))
        {
            int subs[] = { 1 };
            typename std::decay<T>::type directory(std::forward<T>(path).find_first_of("\\/") == 0 ? "/" : "");
            std::regex criteria("(?:[\\\\/]*)([^\\\\/]+)(?:[\\\\/]*)");
            std::sregex_token_iterator itRegex(std::forward<T>(path).cbegin(),
                                               std::forward<T>(path).cend(), criteria, subs), itRegexEnd;
            while (itRegex != itRegexEnd)
            {
                if (itRegex->matched)
                {
                    directory += itRegex->str() + "/";
                    lock(0);
                    bool bDirectoryExists = directoryExists(directory);
                    if (!bDirectoryExists)
                        bSuccess &= (mkdir(directory.c_str()) == 0);

                    unlock(0);

                    ++itRegex;
                }
            }
        }

        return bSuccess;
    }

    /**
     * Function to determine if a directory already exists
     */
    template<typename T>
    static bool directoryExists(T &&path)
    {
#ifdef _WIN32
        auto attributes = getFileAttributes(std::forward<T>(path));
        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
            return true;
#elif defined POSIX
        struct stat attributes;
        if (stat(std::forward<T>(path).c_str(), &attributes) == 0 && S_ISDIR(attributes.st_mode) != 0)
            return true;
#endif
        return false;
    }

    /**
     * Function to determine if a file exists
     */
    template<typename T>
    static bool fileExists(T &&filename)
    {
#ifdef _WIN32
        auto attributes = getFileAttributes(std::forward<T>(filename));
        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            return true;
#elif defined POSIX
        struct stat attributes;
        if (stat(std::forward<T>(filename).c_str(), &attributes) == 0 && S_ISDIR(attributes.st_mode) == 0)
            return true;
#endif
        return false;
    }

    /**
     * Function searches the user-specified relative paths to find the absolute path of a file or directory;
     * upon success, function returns true and the first argument has been populated with the absolute path.
     * Paths are searched in the order in which they are specified; the function first looks for the file
     * assuming an absolute path, and then proceeds to search the user-specified paths. If the path does not
     * exist or another error occurs, function returns false
     * @param absolutePath  updated upon success to reflect the absolute path
     * @param relativePaths a variadic list of relative paths in which a search is performed
     */
    template<typename T, typename ... Paths, typename std::enable_if<(sizeof ... (Paths) > 0), int>::type = 0>
    static bool getAbsolutePath(T &absolutePath, Paths && ... relativePaths)
    {
        bool bSuccess = false;
        typedef typename std::decay<T>::type type;
        for (auto &&relativePath : std::vector<type>{ "", static_cast<type>(relativePaths) ... })
        {
            bSuccess = fileExists(relativePath);
            if (!bSuccess)
                bSuccess = directoryExists(relativePath);

            if (bSuccess)
            {
#ifdef _WIN32
                char buffer[_MAX_PATH];
                bSuccess = (_fullpath(buffer, relativePath.c_str(), _MAX_PATH) != nullptr);
                if (bSuccess)
                {
                    absolutePath = buffer;
                    break;
                }
#elif defined POSIX
                char *directory = realpath(relativePath.c_str(), nullptr);
                bSuccess = (directory != nullptr);
                if (bSuccess)
                {
                    absolutePath = directory;
                    free(directory);
                    break;
                }
#else
                bSuccess = false;
                break;
#endif
            }
        }

        removeSpuriousSlashesFromPath(absolutePath);

        return bSuccess;
    }

    /**
     * Get the current working directory; upon success, return a non-empty string
     */
    static std::string getCurrentWorkingDirectory(void)
    {
        std::string path;
#ifndef _WIN32
        typedef std::pair<dev_t, ino_t> file_id;
        int start_fd = open(".", O_RDONLY); // keep track of start directory, so can jump back to it later
        if (start_fd != -1)
        {
            struct stat sb;
            if (!fstat(start_fd, &sb))
            {
                file_id current_id(sb.st_dev, sb.st_ino);
                if (!stat("/", &sb)) // get info for root directory, so we can determine when we hit it
                {
                    std::vector<std::string> pathComponents;
                    file_id root_id(sb.st_dev, sb.st_ino);

                    // if they're equal, we've obtained enough info to build the path
                    while (current_id != root_id)
                    {
                        bool bPushed = false;
                        if (chdir("..") == 0) // keep recursing toward root on each iteration
                        {
                            auto *pDirectory = opendir(".");
                            if (pDirectory)
                            {
                                // we loop through each entry trying to find the starting location
                                dirent *pEntry = nullptr;
                                while ((pEntry = readdir(pDirectory)))
                                {
                                    if ((strcmp(pEntry->d_name, ".") &&
                                         strcmp(pEntry->d_name, "..") && !lstat(pEntry->d_name, &sb)))
                                    {
                                        // found the starting location, add its name to the list
                                        file_id child_id(sb.st_dev, sb.st_ino);
                                        if (child_id == current_id)
                                        {
                                            pathComponents.push_back(pEntry->d_name);
                                            bPushed = true;
                                            break;
                                        }
                                    }
                                }

                                closedir(pDirectory);

                                // if we have a reason to continue, we update the current directory id
                                if (bPushed && !stat(".", &sb))
                                    current_id = file_id(sb.st_dev, sb.st_ino);
                            } // else, can't read information at this level
                        }

                        if (!bPushed)
                            break; // if we didn't obtain any info this pass, no reason to continue
                    }

                    if (current_id == root_id) // unless they're equal, we failed above
                    {
                        // build the path, will always end with a slash
                        path = "/";
                        for (auto &&pathComponent  : pathComponents)
                            path += pathComponent + "/";
                    }
                }

                fchdir(start_fd);
            }
        }

        close(start_fd);
#else
        char buffer[_MAX_PATH];
        char *directory = getcwd(buffer, _MAX_PATH);
        if (directory != nullptr )
            path = std::string(directory);
#endif
        return path;
    }

#ifdef _WIN32
private:

    /**
     * Function to get file attributes in Win32
     */
    template<typename T, typename std::enable_if<std::is_convertible<T, std::wstring>::value, int>::type = 0>
    inline static auto getFileAttributes(T &&path)
    {
        return GetFileAttributesW(std::forward<T>(path).c_str());
    }

    /**
     * Function to get file attributes in Win32
     */
    template<typename T, typename std::enable_if<!std::is_convertible<T, std::wstring>::value, int>::type = 0>
    inline static auto getFileAttributes(T &&path)
    {
        return GetFileAttributesA(std::forward<T>(path).c_str());
    }

public:
#endif
    /**
     * Retrieve the filename from the specified path
     */
    template<typename T>
    static auto getFilenameFromPath(T &&path)
    {
        if (fileExists(std::forward<T>(path)))
            return path.substr(std::forward<T>(path).find_last_of("/\\") + 1);

        return path;
    }

    /**
     * Retrieve the filename from the specified path
     */
    template<typename T, typename PathDelimiters>
    static auto getFilenameFromPath(T &&path, PathDelimiters &&delims)
    {
        if (fileExists(std::forward<T>(path)))
            return path.substr(std::forward<T>(path).find_last_of(delims) + 1);

        return path;
    }

    /**
     * Function determines the path of a file or folder relative to another path
     * @param[in]  path         the path for which a relative path is desired
     * @param      relativePath the path to which the first argument will be made relative
     * @param[out] path         updated upon success to reflect the relative path
     */
    template<typename T>
    inline static bool getRelativePath(T &path, T relativePath)
    {
        typedef typename std::decay<T>::type type;
        type absolutePath = path;
        bool bSuccess = getAbsolutePath(absolutePath, absolutePath) &&
                        getAbsolutePath(relativePath, relativePath);
        if (bSuccess)
        {
            removeFilenameFromPath(relativePath);
            if (relativePath.back() != '/' || relativePath.back() != '\\')
                relativePath += "/";

            type rootPath;
            std::set_intersection(path.cbegin(), path.cend(),
                                  relativePath.cbegin(), relativePath.cend(),
                                  std::back_inserter(rootPath));

            bSuccess = (!rootPath.empty());
            if (bSuccess)
            {
                path = absolutePath.substr(rootPath.size(), std::string::npos);
                relativePath.erase(0, rootPath.size());

                std::size_t numParentPaths = std::count_if(relativePath.cbegin(),
                                                           relativePath.cend(),
                                                           [] (auto &&ch) { return ch == '/'; });

                for (std::size_t i = 0; i < numParentPaths; ++i)
                    path = "../" + path;
            }
        }

        return bSuccess;
    }

    /**
     * Function to convert a path from relative to absolute
     */
    template<typename T>
    inline static bool makePathAbsolute(T &path)
    {
        return getAbsolutePath(path, path);
    }

    /**
     * Search the user-specified paths for the specified file and open the input file stream, if found on the
     * file-system; upon success, the function returns true. Paths are searched in the order in which they are
     * specified; the function first tries to open the file assuming an absolute path, and then proceeds to
     * search the user-specified paths. The first successful open file stream terminates the search
     * @param filename the path to the file for which, if found, a file stream will be opened
     * @param stream   a reference to an std::ifstream object
     * @param paths    a variadic list of directories in which to search for the specified file
     */
    template<typename T,
             typename Stream,
             typename std::enable_if<!std::is_convertible<
             typename std::decay<Stream>::type, std::ios_base::openmode>::value, int>::type = 0,
             typename ... Paths>
    inline static bool openFileStream(T &&filename, Stream &&stream, Paths && ... paths)
    {
        return openFileStream(std::forward<T>(filename),
                              std::ios_base::in,
                              stream,
                              std::forward<Paths>(paths) ...);
    }

    /**
     * Search the user-specified paths for the specified file and open the input file stream, if found on the
     * file-system; upon success, the function returns true. Paths are searched in the order in which they are
     * specified; the function first tries to open the file assuming an absolute path, and then proceeds to
     * search the user-specified paths. The first successful open file stream terminates the search
     * @param filename the path to the file for which, if found, a file stream will be opened
     * @param mode     the file open mode
     * @param stream   a reference to an std::ifstream object
     * @param paths    a variadic list of directories in which to search for the specified file
     */
    template<typename T, typename Stream, typename ... Paths>
    static bool openFileStream(T &&filename,
                               std::ios_base::openmode mode,
                               Stream &&stream,
                               Paths && ... paths)
    {
        bool bSuccess = false;
        typedef typename std::decay<T>::type type;
        for (auto &&path : std::vector<type>{ "", static_cast<type>(paths) ... })
        {
            stream.clear();

            if (traits::helpers::size(filename) > 0)
            {
                // don't use std::forward<T>(filename) here; the data in filename mysteriously vanishes...
                if (traits::helpers::size(path) > 0)
                    stream.open(utilities::string::StringUtilities::concatenate(path, "/", filename), mode);
                else
                    stream.open(utilities::string::StringUtilities::concatenate(path, filename), mode);

                bSuccess = (bool)stream;
                if (bSuccess)
                    break;
            }
        }

        return bSuccess;
    }

    /**
     * Function to remove the extension from a filename
     */
    template<typename T>
    inline static void removeExtensionFromFilename(T &filename)
    {
        auto posPeriod = std::forward<T>(filename).find_last_of('.');
        if (posPeriod != std::string::npos)
        {
            auto posSlash = std::forward<T>(filename).find_last_of("/\\");
            if (posSlash == std::string::npos || (posSlash != std::string::npos && posPeriod > posSlash))
            {
                // if we get here, then the period is not part of a directory name
                std::forward<T>(filename).erase(posPeriod, std::string::npos);
            }
        }
    }

    /**
     * Function to remove the filename from a path
     */
    template<typename T>
    inline static void removeFilenameFromPath(T &path)
    {
        if (fileExists(path))
        {
            auto posSlash = std::forward<T>(path).find_last_of("/\\");
            if (posSlash != std::string::npos)
                path.erase(posSlash, std::string::npos);
        }
    }

    /**
     * Remove spurious slashes from the specified path
     */
    template<typename T>
    inline static void removeSpuriousSlashesFromPath(T &path)
    {
        string::StringReplacer<std::regex> stringReplacer;
        stringReplacer.add("/+", "/");
        stringReplacer.add("(\\\\){1,}", "\\");
        stringReplacer.searchAndReplace(path);
    }
};

}

}

#endif
