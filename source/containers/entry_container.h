#ifndef ENTRY_CONTAINER_H
#define ENTRY_CONTAINER_H

#include "initializable.h"
#include "reflective.h"
#include "swappable.h"
#include <limits>
#include <vector>

namespace containers
{

/**
 * An abstract container class for storing and retrieving entries
 */
template<typename EntryType>
class EntryContainer
: public attributes::interfaces::Initializable,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<EntryContainer<EntryType>>
{
public:

    /**
     * Constructor
     */
    EntryContainer(void)
    : m_maxSize(std::numeric_limits<std::size_t>::max())
    {

    }

    /**
     * Copy constructor
     */
    EntryContainer(const EntryContainer<EntryType> &container)
    {
        operator = (container);
    }

    /**
     * Move constructor
     */
    EntryContainer(EntryContainer<EntryType> &&container)
    {
        operator = (std::move(container));
    }

    /**
     * Destructor
     */
    virtual ~EntryContainer(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    EntryContainer<EntryType> &operator = (const EntryContainer<EntryType> &container)
    {
        if (&container != this)
        {
            m_maxSize = container.m_maxSize;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    EntryContainer<EntryType> &operator = (EntryContainer<EntryType> &&container)
    {
        if (&container != this)
        {
            container.swap(*this);
        }

        return *this;
    }

    /**
     * Add an entry to this container
     */
    EXPORT_STEM virtual void addEntry(const EntryType &entry) = 0;

    /**
     * Delete entries by value from this container
     */
    virtual bool deleteEntries(std::vector<EntryType *> &entries)
    {
        bool bSuccess = true; // assume success
        auto &&itEntry = entries.cbegin();
        while (bSuccess && itEntry != entries.cend())
        {
            auto *pEntry = entries.front();
            bSuccess = deleteEntry(pEntry);
            if (bSuccess)
                itEntry = entries.erase(itEntry);
            else
                ++itEntry;
        }

        return bSuccess;
    }

    /**
     * Delete all entries from this container
     */
    EXPORT_STEM virtual void deleteEntries(void) = 0;

    /**
     * Delete an entry by value from this container; sets input argument to null upon success
     */
    EXPORT_STEM virtual bool deleteEntry(EntryType *&pEntry) = 0;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "EntryContainer";
    }

    /**
     * Get the maximum number of most entries maintained by this container
     */
    inline virtual std::size_t getMaxSize(void) const final
    {
        return m_maxSize;
    }

    /**
     * Initialization function
     */
    virtual bool initialize(void) override
    {
        deleteEntries();

        return true;
    }

    /**
     * Remove entries by value from this container
     */
    virtual bool removeEntries(std::vector<EntryType *> &entries)
    {
        bool bSuccess = true; // assume success
        auto &&itEntry = entries.cbegin();
        while (bSuccess && itEntry != entries.cend())
        {
            auto *pEntry = entries.front();
            bSuccess = removeEntry(pEntry);
            if (bSuccess)
                itEntry = entries.erase(itEntry);
            else
                ++itEntry;
        }

        return bSuccess;
    }

    /**
     * Remove all entries from this container
     */
    EXPORT_STEM virtual void removeEntries(void) = 0;

    /**
     * Remove an entry by value from this container
     */
    EXPORT_STEM virtual bool removeEntry(EntryType *pEntry) = 0;

    /**
     * Set the maximum number of entries maintained by this container
     */
    inline virtual void setMaxSize(std::size_t maxSize) final
    {
        m_maxSize = maxSize;
    }

    /**
     * Return the number of entries contained within this object
     */
    EXPORT_STEM virtual std::size_t size(void) const = 0;

    /**
     * Swap function
     */
    virtual void swap(EntryContainer<EntryType> &container) override final
    {
        std::swap(m_maxSize, container.m_maxSize);
    }

protected:

    /**
     * the maximum number of entries maintained by this container
     */
    std::size_t m_maxSize;
};

}

#endif
