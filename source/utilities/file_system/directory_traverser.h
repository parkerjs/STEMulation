#ifndef DIRECTORY_TRAVERSER_H
#define DIRECTORY_TRAVERSER_H

#include "cloneable.h"
#include "directory_iterator.h"
#include "iterable.h"
#include <set>

namespace utilities
{

namespace file_system
{

// forward declarations
extern class DirectoryTraverser *createTraverserInstance(void);

/**
 * This class is used to recursively search the specified directory for all files in all sub-folders
 */
class DirectoryTraverser
: public attributes::interfaces::Cloneable<DirectoryTraverser>,
  public attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::directory_iterator_tag>
{
public:

    /**
     * Iterator typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::directory_iterator_tag>
                      ::const_iterator const_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::directory_iterator_tag>
                      ::iterator iterator;

protected:

    /**
     * Constructor
     */
    DirectoryTraverser(void)
    {

    }

public:

    /**
     * Copy constructor
     */
    DirectoryTraverser(const DirectoryTraverser &traverser)
    {
        operator = (traverser);
    }

    /**
     * Move constructor
     */
    DirectoryTraverser(DirectoryTraverser &&traverser)
    {
        operator = (std::move(traverser));
    }

    /**
     * Destructor
     */
    virtual ~DirectoryTraverser(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    DirectoryTraverser &operator = (const DirectoryTraverser &traverser)
    {
        if (&traverser != this)
        {
            m_fileMask = traverser.m_fileMask;
            m_path = traverser.m_path;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    DirectoryTraverser &operator = (DirectoryTraverser &&traverser)
    {
        if (&traverser != this)
        {
            m_fileMask = std::move(traverser.m_fileMask);
            m_path = std::move(traverser.m_path);
        }

        return *this;
    }

    /**
     * cbegin() override
     */
    inline virtual const_iterator cbegin(void) const override
    {
        return const_cast<DirectoryTraverser *>(this)->begin();
    }

    /**
     * cend() override
     */
    inline virtual const_iterator cend(void) const override
    {
        return const_cast<DirectoryTraverser *>(this)->end();
    }

    /**
     * clone() function
     */
    virtual DirectoryTraverser *clone(void) const override = 0;

    /**
     * create() function
     */
    inline static DirectoryTraverser *create(void)
    {
        return createTraverserInstance();
    }

    /**
     * Traversal function to recursively find all files matching the specified file mask in the folder
     * specified by input argument path and all its sub-folders
     */
    inline virtual std::set<std::string> findFiles(const std::string &path = "")
    {
        initialize(path, m_fileMask);

        std::set<std::string> files;
        for (auto &&itFile = cbegin(); itFile != cend(); ++itFile)
            if (!itFile->empty())
                files.insert(*itFile);

        return files;
    }

    /**
     * Find the next file
     */
    virtual bool findNextFile(std::string &file) = 0;

    /**
     * Get the regular expression used to select files of interest that match the given pattern
     */
    inline virtual std::string getFileMask(void) const final
    {
        return m_fileMask;
    }

    /**
     * Get the top-level path to be traversed
     */
    inline virtual std::string getPath(void) const final
    {
        return m_path;
    }

    /**
     * Initialization function
     */
    virtual bool initialize(const std::string &path, const std::string &fileMask) = 0;

    /**
     * Set the regular expression used to select files of interest that match the given pattern
     */
    inline virtual void setFileMask(const std::string &fileMask) final
    {
        if (fileMask != m_fileMask)
            initialize(m_path, fileMask);
    }

    /**
     * Set the top-level path to be traversed
     */
    inline virtual bool setPath(const std::string &path) final
    {
        bool bSuccess = true;
        if (path != m_path)
            bSuccess = initialize(path, m_fileMask);

        return bSuccess;
    }

protected:

    /**
     * regular expression used to select files of interest that match the given pattern
     */
    std::string m_fileMask;

    /**
     * the top-level path to be traversed
     */
    std::string m_path;
};

}

}

// additional includes
#include "posix_directory_traverser.h"
#include "windows_directory_traverser.h"

#endif
