#ifndef IDENTIFIER_SORTED_CONTAINER_H
#define IDENTIFIER_SORTED_CONTAINER_H

#include "entry_container.h"
#include <algorithm>

namespace containers
{

/**
 * A container class for storing and retrieving entries by id
 */
template<typename EntryType,
         typename IdType = std::size_t,
         IdType (EntryType::*getEntryId)(void) const = &EntryType::getEntryId,
         EntryType * (EntryType::*clone)(void) const = &EntryType::clone,
         void (*deleter)(EntryType *&) = nullptr>
class IdentifierSortedContainer
: public EntryContainer<EntryType>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<IdentifierSortedContainer<EntryType,
                                                                     IdType,
                                                                     getEntryId,
                                                                     clone,
                                                                     deleter>>
{
public:

    /**
     * Typedef declarations
     */
    typedef IdentifierSortedContainer<EntryType,
                                      IdType,
                                      getEntryId,
                                      clone,
                                      deleter> tIdentifierSortedContainer;

    /**
     * Using declarations
     */
    using EntryContainer<EntryType>::deleteEntries;
    using EntryContainer<EntryType>::removeEntries;
    using EntryContainer<EntryType>::swap;

    /**
     * An entry comparison functor
     */
    struct EntryComparator
    {
        /**
         * Function call operator
         */
        inline bool operator () (EntryType *pEntry,
                                 IdType entryId) const
        {
            return pEntry != nullptr && (pEntry->*getEntryId)() < entryId;
        }

        /**
         * Function call operator
         */
        inline bool operator () (IdType entryId,
                                 EntryType *pEntry) const
        {
            return pEntry != nullptr && entryId < (pEntry->*getEntryId)();
        }
    };

    /**
     * Constructor
     */
    IdentifierSortedContainer(void)
    {

    }

    /**
     * Copy constructor
     */
    IdentifierSortedContainer(const tIdentifierSortedContainer &container)
    {
        operator = (container);
    }

    /**
     * Move constructor
     */
    IdentifierSortedContainer(tIdentifierSortedContainer &&container)
    {
        operator = (std::move(container));
    }

    /**
     * Destructor
     */
    virtual ~IdentifierSortedContainer(void) override
    {
        deleteEntries();
    }

    /**
     * Copy assignment operator
     */
    tIdentifierSortedContainer &operator = (const tIdentifierSortedContainer &container)
    {
        if (&container != this)
        {
            EntryContainer<EntryType>::operator = (container);

            copyEntries(container);
            m_comparator = container.m_comparator;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    tIdentifierSortedContainer &operator = (tIdentifierSortedContainer &&container)
    {
        if (&container != this)
        {
            container.swap(*this);
        }

        return *this;
    }

    /**
     * Add an entry to this container; returns true if the entry is successfully added and the current object
     * takes ownership of the input argument and becomes responsible for its destruction
     */
    virtual bool addEntry(EntryType *pEntry)
    {
        bool bSuccess = (pEntry != nullptr);
        if (bSuccess)
        {
            auto entryId = (pEntry->*getEntryId)();
            auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                              m_entries.cend(),
                                              entryId,
                                              m_comparator);

            EntryType *pThisEntry = nullptr;
            if (itEntry != m_entries.cend())
                pThisEntry = *itEntry;

            if (pThisEntry != nullptr && pThisEntry == pEntry)
            {
                return false;
            }
            else if (pThisEntry != nullptr && (pThisEntry->*getEntryId)() == entryId)
            {
                // an entry with the same entry id already exists in this object's entries, so replace the
                // current entry with the new one
                if (deleter == nullptr)
                    delete pThisEntry;
                else
                    (*deleter)(pThisEntry);

                itEntry = m_entries.erase(itEntry);
            }

            m_entries.insert(itEntry, pEntry);
            while (m_entries.size() > this->getMaxSize())
                deleteEntry(m_entries.front());
        }

        return bSuccess;
    }

    /**
     * Add an entry to this container
     */
    virtual void addEntry(const EntryType &entry) override
    {
        auto entryId = (entry.*getEntryId)();
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          entryId,
                                          m_comparator);

        EntryType *pEntry = nullptr;
        if (itEntry != m_entries.cend())
            pEntry = *itEntry;

        if (pEntry == nullptr || (pEntry->*getEntryId)() != entryId)
        {
            if (m_entries.size() >= this->getMaxSize())
            {
                if (itEntry == m_entries.cend())
                {
                    // perform a simple rotation and assign the current entry to the back
                    std::rotate(m_entries.begin(),
                                m_entries.begin() + 1,
                                m_entries.end());

                    pEntry = m_entries.back();
                    if (pEntry != nullptr)
                        *pEntry = entry;
                }
                else
                {
                    m_entries.emplace(itEntry, (entry.*clone)());
                    while (m_entries.size() > this->getMaxSize())
                    {
                        deleteEntry(m_entries.front());
                    }
                }
            }
            else
            {
                m_entries.emplace(itEntry, (entry.*clone)());
            }
        }
        else if (pEntry != nullptr)
        {
            // an entry with the same entry id already exists in this object's entries, so update the
            // current entry with the new one
            *pEntry = entry;
        }
    }

    /**
     * Copy the entries from the input container to this object
     */
    virtual void copyEntries(const tIdentifierSortedContainer &container) final
    {
        // delete this object's current entries before cloning
        deleteEntries();

        // clone or copy entries from the input container
        auto &&itEntry = m_entries.cbegin();
        while (itEntry != m_entries.cend())
        {
            auto *pEntry = *itEntry;
            if (pEntry != nullptr)
                addEntry(*pEntry);

            ++itEntry;
        }
    }

    /**
     * Delete entries by value from this container
     */
    virtual bool deleteEntries(std::vector<EntryType *> &entries) override
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
    virtual void deleteEntries(void) override
    {
        auto &&itEntry = m_entries.cbegin();
        while (itEntry != m_entries.cend())
        {
            auto size = m_entries.size();
            auto *pEntry = *itEntry;
            if (pEntry != nullptr)
            {
                if (deleter == nullptr)
                    delete pEntry;
                else
                    (*deleter)(pEntry);
            }

            if (size != m_entries.size())
                itEntry = m_entries.cbegin();
            else
                ++itEntry;
        }

        m_entries.clear();
    }

    /**
     * Delete an entry by value from this container; sets input argument to null upon success
     */
    virtual bool deleteEntry(EntryType *&pEntry) override
    {
        bool bSuccess = (pEntry != nullptr);
        if (bSuccess)
        {
            auto &&itEntry = std::find(m_entries.cbegin(),
                                       m_entries.cend(),
                                       pEntry);
            bSuccess = (itEntry != m_entries.cend());
            if (bSuccess)
            {
                if (deleter == nullptr)
                    delete pEntry;
                else
                    (*deleter)(pEntry);

                pEntry = nullptr;
                m_entries.erase(itEntry);
            }
        }

        return bSuccess;
    }

    /**
     * Delete an entry by entry id
     */
    virtual bool deleteEntry(IdType entryId) final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          entryId,
                                          m_comparator);

        bool bSuccess = (itEntry != m_entries.cend());
        if (bSuccess)
        {
            auto *pEntry = *itEntry;
            bSuccess = (pEntry != nullptr);
            if (bSuccess)
            {
                if (deleter == nullptr)
                    delete pEntry;
                else
                    (*deleter)(pEntry);

                m_entries.erase(itEntry);
            }
        }

        return bSuccess;
    }

    /**
     * Test for empty container
     */
    inline virtual bool empty(void) const final
    {
        return m_entries.empty();
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "IdentifierSortedContainer";
    }

    /**
     * Get this container's entries
     */
    inline virtual std::vector<EntryType *> &getEntries(void) final
    {
        return m_entries;
    }

    /**
     * Get this container's entries
     */
    inline virtual std::vector<EntryType *> getEntries(void) const final
    {
        return m_entries;
    }

    /**
     * Retrieve an entry by entry id; returns non-null upon success
     */
    virtual EntryType *getEntry(IdType entryId) const final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          entryId,
                                          m_comparator);
        if (itEntry != m_entries.cend())
            return *itEntry;

        return nullptr;
    }

    /**
     * Remove all entries from this container
     */
    inline virtual void removeEntries(void) override
    {
        m_entries.clear();
    }

    /**
     * Remove an entry by value from this container
     */
    virtual bool removeEntry(EntryType *pEntry) override
    {
        bool bSuccess = (pEntry != nullptr);
        if (bSuccess)
        {
            auto &&itEntry = std::find(m_entries.cbegin(),
                                       m_entries.cend(),
                                       pEntry);
            bSuccess = (itEntry != m_entries.cend());
            if (bSuccess)
            {
                m_entries.erase(itEntry);
            }
        }

        return bSuccess;
    }

    /**
     * Remove an entry by entry id
     */
    virtual bool removeEntry(IdType entryId) final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          entryId,
                                          m_comparator);

        bool bSuccess = (itEntry != m_entries.cend());
        if (bSuccess)
        {
            auto *pEntry = *itEntry;
            bSuccess = (pEntry != nullptr);
            if (bSuccess)
            {
                m_entries.erase(itEntry);
            }
        }

        return bSuccess;
    }

    /**
     * Return the number of entries contained within this object
     */
    inline virtual IdType size(void) const override
    {
        return m_entries.size();
    }

    /**
     * Swap function
     */
    virtual void swap(tIdentifierSortedContainer &container) override final
    {
        EntryContainer<EntryType>::swap(container);

        std::swap(m_comparator, container.m_comparator);
        m_entries.swap(container.m_entries);
    }

protected:

    /**
     * this object's entry comparator
     */
    EntryComparator m_comparator;

    /**
     * this container's entries sorted by entry id
     */
    std::vector<EntryType *> m_entries;
};

}

#endif
