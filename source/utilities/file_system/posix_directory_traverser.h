#if !defined POSIX_DIRECTORY_TRAVERSER_H && (defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)))
#define POSIX_DIRECTORY_TRAVERSER_H

#include "directory_traverser.h"
#include "file_system.h"
#include <regex>
#include <string.h>

namespace utilities
{

namespace file_system
{

/**
 * This class is used to recursively search the specified directory for all files in all sub-folders. A POSIX
 * operating system is assumed.
 */
class PosixDirectoryTraverser
: public DirectoryTraverser
{
public:

    /**
     * Typedef declarations
     */
    typedef bool (PosixDirectoryTraverser::*file_functor_type)(std::string &, std::stack<std::string> &,
                                                               std::stack<DIR *> &);

    /**
     * Iterator typedef declarations
     */
    typedef DirectoryTraverser::const_iterator const_iterator;
    typedef DirectoryTraverser::iterator iterator;

    /**
     * Constructor
     */
    PosixDirectoryTraverser(void)
    {

    }

    /**
     * Copy constructor
     */
    PosixDirectoryTraverser(const PosixDirectoryTraverser &traverser)
    {
        operator = (traverser);
    }

    /**
     * Move constructor
     */
    PosixDirectoryTraverser(PosixDirectoryTraverser &&traverser)
    {
        operator = (std::move(traverser));
    }

    /**
     * Destructor
     */
    virtual ~PosixDirectoryTraverser(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    PosixDirectoryTraverser &operator = (const PosixDirectoryTraverser &traverser)
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
    PosixDirectoryTraverser &operator = (PosixDirectoryTraverser &&traverser)
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
        iterator it(m_path, std::bind(static_cast<file_functor_type>(&PosixDirectoryTraverser::findNextFile),
                                      this, std::placeholders::_1, std::placeholders::_2,
                                      std::placeholders::_3));

        return ++it;
    }

    /**
     * clone() function
     */
    inline virtual PosixDirectoryTraverser *clone(void) const override
    {
        return new PosixDirectoryTraverser(*this);
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
    virtual bool findNextFile(std::string &file, std::stack<std::string> &folders,
                              std::stack<DIR *> &handles) final
    {
        bool bFound = false;
        if (!folders.empty())
        {
            DIR *&pDirectoryHandle = handles.top();
            if (!handles.empty() && pDirectoryHandle == nullptr)
                pDirectoryHandle = opendir(folders.top().c_str());

            bool bSuccess = (pDirectoryHandle != nullptr);
            while (bSuccess && !bFound)
            {
                dirent *pFileHandle = nullptr;
                if (pDirectoryHandle != nullptr)
                    pFileHandle = readdir(pDirectoryHandle);

                bSuccess = (pFileHandle != nullptr);
                if (bSuccess)
                {
                    const unsigned char FILE = 0x8;
                    std::string fileOrFolderName = folders.top() + "/" + pFileHandle->d_name;
                    if (pFileHandle->d_type != FILE)
                    {
                        if (strcmp(pFileHandle->d_name, ".") == 0 ||
                            strcmp(pFileHandle->d_name, "..") == 0)
                            continue;

                        folders.push(fileOrFolderName);
                        handles.push(nullptr);

                        bFound = findNextFile(file, folders, handles);
                    }
                    else
                    {
                        if (m_fileMask.empty())
                            bFound = true;
                        else
                        {
                            auto &&filename = FileSystem::getFilenameFromPath(fileOrFolderName);
                            if (std::regex_match(filename, std::regex(m_fileMask)))
                                bFound = true;
                        }

                        if (bFound)
                            file = fileOrFolderName;
                    }
                }
            }

            if (!bFound)
            {
                if (pDirectoryHandle != nullptr)
                {
                    closedir(pDirectoryHandle);
                    pDirectoryHandle = nullptr;
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

            if (!path.empty())
                m_path = path;

            m_folders = std::stack<std::string>(); // clear the folder stack
            m_folders.push(m_path);

            // clear the handle stack
            while (!m_handles.empty())
            {
                auto *pDirectoryHandle = m_handles.top();
                if (pDirectoryHandle)
                    closedir(pDirectoryHandle);

                m_handles.pop();
            }

            m_handles.push(nullptr);
        }

        return bSuccess;
    }

private:

    /**
     * a stack of folders which help to track the directory depth level of the current search
     */
    std::stack<std::string> m_folders;

    /**
     * a stack of handles which correspond to the stack of folders
     */
    std::stack<DIR *> m_handles;
};

/**
 * create() an instance of the directory traverser
 */
inline DirectoryTraverser *createTraverserInstance(void)
{
    return new PosixDirectoryTraverser();
}

}

}

#endif
