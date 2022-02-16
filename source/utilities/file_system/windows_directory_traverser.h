#if !defined WINDOWS_DIRECTORY_TRAVERSER_H && defined _WIN32
#define WINDOWS_DIRECTORY_TRAVERSER_H

#include "directory_traverser.h"
#include "file_system.h"
#include <regex>

namespace utilities
{

namespace file_system
{

/**
 * This class is used to recursively search the specified directory for all files in all sub-folders. A windows
 * operating system is assumed.
 */
class WindowsDirectoryTraverser
: public DirectoryTraverser
{
public:

    /**
     * Typedef declarations
     */
    typedef bool (WindowsDirectoryTraverser::*file_functor_type)(std::string &, std::stack<std::wstring> &,
                                                                 std::stack<HANDLE> &);

    /**
     * Iterator typedef declarations
     */
    typedef DirectoryTraverser::const_iterator const_iterator;
    typedef DirectoryTraverser::iterator iterator;

    /**
     * Constructor
     */
    WindowsDirectoryTraverser(void)
    {
        initialize("C:/", ".+"); // file mask is in regex format
    }

    /**
     * Copy constructor
     */
    WindowsDirectoryTraverser(const WindowsDirectoryTraverser &traverser)
    {
        operator = (traverser);
    }

    /**
     * Move constructor
     */
    WindowsDirectoryTraverser(WindowsDirectoryTraverser &&traverser)
    {
        operator = (traverser);
    }

    /**
     * Destructor
     */
    virtual ~WindowsDirectoryTraverser(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    WindowsDirectoryTraverser &operator = (const WindowsDirectoryTraverser &traverser)
    {
        if (&traverser != this)
        {
            DirectoryTraverser::operator = (traverser);

            m_folders = traverser.m_folders;
            m_handles = traverser.m_handles;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    WindowsDirectoryTraverser &operator = (WindowsDirectoryTraverser &&traverser)
    {
        if (&traverser != this)
        {
            DirectoryTraverser::operator = (std::move(traverser));

            m_folders = std::move(traverser.m_folders);
            m_handles = std::move(traverser.m_handles);
        }

        return *this;
    }

    /**
     * begin() override
     */
    inline virtual iterator begin(void) override
    {
        std::wstring path(m_path.begin(), m_path.end());
        iterator it(path, std::bind(static_cast<file_functor_type>(&WindowsDirectoryTraverser::findNextFile),
                                    this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3));

        return ++it;
    }

    /**
     * clone() function
     */
    inline virtual WindowsDirectoryTraverser *clone(void) const override
    {
        return new WindowsDirectoryTraverser(*this);
    }

    /**
     * end() override
     */
    inline virtual iterator end(void) override
    {
        return iterator();
    }

    /**
     * Find the next file
     */
    inline virtual bool findNextFile(std::string &file) override
    {
        return findNextFile(file, m_folders, m_handles);
    }

    /**
     * Find the next file
     */
    virtual bool findNextFile(std::string &file, std::stack<std::wstring> &folders,
                              std::stack<HANDLE> &handles) final
    {
        bool bFound = false;
        if (!folders.empty())
        {
            WIN32_FIND_DATA fileFinder;
            HANDLE &handle = handles.top();
            if (!handles.empty() && handle == nullptr)
            {
                auto &&path = folders.top() + L"/*.*";
                handle = FindFirstFile(path.c_str(), &fileFinder);
            }

            bool bSuccess = (handle != INVALID_HANDLE_VALUE);
            while (bSuccess && !bFound)
            {
                bSuccess = (handle != nullptr);
                if (bSuccess)
                    bSuccess = (FindNextFile(handle, &fileFinder) != 0);

                if (bSuccess)
                {
                    std::wstring fileOrFolderName = folders.top() + L"/" + fileFinder.cFileName;
                    if (fileFinder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        if (wcscmp(fileFinder.cFileName, L".") == 0 ||
                            wcscmp(fileFinder.cFileName, L"..") == 0)
                            continue;

                        folders.push(fileOrFolderName);
                        handles.push(nullptr);

                        bFound = findNextFile(file, folders, handles);
                    }
                    else
                    {
                        auto &&fileMask = std::wstring(m_fileMask.cbegin(), m_fileMask.cend());
                        if (m_fileMask.empty())
                            bFound = true;
                        else
                        {
                            auto &&filename = FileSystem::getFilenameFromPath(fileOrFolderName, L"/\\");
                            if (std::regex_match(filename, std::wregex(fileMask)))
                                bFound = true;
                        }

                        if (bFound)
                            file.assign(fileOrFolderName.begin(), fileOrFolderName.end());
                    }
                }
            }

            if (!bFound)
            {
                if (handle != nullptr)
                {
                    FindClose(handle);
                    handle = nullptr;
                }

                if (!folders.empty())
                {
                    folders.pop();
                    handles.pop();

                    bFound = findNextFile(file, folders, handles);
                }
            }
        }

        return bFound;
    }

    /**
     * Initialization function
     */
    virtual bool initialize(const std::string &path, const std::string &fileMask) override
    {
        bool bSuccess = FileSystem::directoryExists(path);
        if (bSuccess)
        {
            if (!fileMask.empty())
                m_fileMask = fileMask;

            m_folders = std::stack<std::wstring>(); // clear the folder stack
            m_folders.push(std::wstring(path.cbegin(), path.cend()));

            m_handles = std::stack<HANDLE>(); // clear the handle stack
            m_handles.push(nullptr);

            if (!path.empty())
                m_path = path;
        }

        return bSuccess;
    }

private:

    /**
     * a stack of folders which help to track the directory depth level of the current search
     */
    std::stack<std::wstring> m_folders;

    /**
     * a stack of handles which correspond to the stack of folders
     */
    std::stack<HANDLE> m_handles;
};

/**
 * create() an instance of the directory traverser
 */
inline DirectoryTraverser *createTraverserInstance(void)
{
    return new WindowsDirectoryTraverser();
}

}

}

#endif
