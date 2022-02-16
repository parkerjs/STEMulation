#ifndef TIME_SORTED_CONTAINER_H
#define TIME_SORTED_CONTAINER_H

#include "entry_container.h"

namespace containers
{

/**
 * A container class for storing and retrieving entries by time
 */
template<typename EntryType,
         typename TimeType = double,
         TimeType (EntryType::*getTime)(void) const = &EntryType::getAvailabilityTime,
         EntryType * (EntryType::*clone)(void) const = &EntryType::clone,
         void (*deleter)(EntryType *&) = nullptr>
class TimeSortedContainer
: public EntryContainer<EntryType>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<TimeSortedContainer<EntryType,
                                                               TimeType,
                                                               getTime,
                                                               clone,
                                                               deleter>>
{
public:

    /**
     * Typedef declarations
     */
    typedef TimeSortedContainer<EntryType,
                                TimeType,
                                getTime,
                                clone,
                                deleter> tTimeSortedContainer;

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
        inline virtual bool operator () (EntryType *pEntry,
                                         TimeType time) const final
        {
            return pEntry != nullptr && (pEntry->*getTime)() < time;
        }

        /**
         * Function call operator
         */
        inline virtual bool operator () (TimeType time,
                                         EntryType *pEntry) const final
        {
            return pEntry != nullptr && time < (pEntry->*getTime)();
        }
    };

    /**
     * Constructor
     */
    TimeSortedContainer(void)
    {

    }

    /**
     * Copy constructor
     */
    TimeSortedContainer(const tTimeSortedContainer &container)
    {
        operator = (container);
    }

    /**
     * Move constructor
     */
    TimeSortedContainer(tTimeSortedContainer &&container)
    {
        operator = (std::move(container));
    }

    /**
     * Destructor
     */
    virtual ~TimeSortedContainer(void) override
    {
        deleteEntries();
    }

    /**
     * Copy assignment operator
     */
    tTimeSortedContainer &operator = (const tTimeSortedContainer &container)
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
    tTimeSortedContainer &operator = (tTimeSortedContainer &&container)
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
            auto time = (pEntry->*getTime)();
            auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                              m_entries.cend(),
                                              time,
                                              m_comparator);

            EntryType *pThisEntry = nullptr;
            if (itEntry != m_entries.cend())
            {
                pThisEntry = *itEntry;
            }

            if (pThisEntry != nullptr &&
                pThisEntry == pEntry)
            {
                return false;
            }
            else if (pThisEntry != nullptr &&
                     (pThisEntry->*getTime)() == time)
            {
                // an entry with the same entry time already exists in this object's entries, so replace the
                // current entry with the new one
                if (deleter == nullptr)
                {
                    delete pThisEntry;
                }
                else
                {
                    (*deleter)(pThisEntry);
                }

                itEntry = m_entries.erase(itEntry);
            }

            m_entries.insert(itEntry,
                             pEntry);
            while (m_entries.size() > this->getMaxSize())
            {
                deleteEntry(m_entries.front());
            }
        }

        return bSuccess;
    }

    /**
     * Add an entry to this container
     */
    virtual void addEntry(const EntryType &entry) override
    {
        auto time = (entry.*getTime)();
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          time,
                                          m_comparator);

        EntryType *pEntry = nullptr;
        if (itEntry != m_entries.cend())
        {
            pEntry = *itEntry;
        }

        if (pEntry == nullptr ||
            (pEntry->*getTime)() != time)
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
                    {
                        *pEntry = entry;
                    }
                }
                else
                {
                    m_entries.emplace(itEntry,
                                      (entry.*clone)());
                    while (m_entries.size() > this->getMaxSize())
                    {
                        deleteEntry(m_entries.front());
                    }
                }
            }
            else
            {
                m_entries.emplace(itEntry,
                                  (entry.*clone)());
            }
        }
        else if (pEntry != nullptr)
        {
            // an entry with the same entry time already exists in this object's entries, so
            // update the current entry with the new one
            *pEntry = entry;
        }
    }

    /**
     * Copy the entries from the input container to this object
     */
    virtual void copyEntries(const tTimeSortedContainer &container) final
    {
        // delete this object's current entries before cloning
        deleteEntries();

        // clone or copy entries from the input container
        auto &&itEntry = m_entries.cbegin();
        while (itEntry != m_entries.cend())
        {
            auto *pEntry = *itEntry;
            if (pEntry != nullptr)
            {
                addEntry(*pEntry);
            }

            ++itEntry;
        }
    }

    /**
     * Delete all entries tagged between the specified starting and ending times
     */
    virtual void deleteEntries(TimeType startTime,
                               TimeType endTime) final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          startTime,
                                          m_comparator);
        auto &&itEntryEnd = std::upper_bound(itEntry,
                                             m_entries.cend(),
                                             endTime,
                                             m_comparator);
        if (itEntryEnd == itEntry)
        {
            itEntryEnd = m_entries.cend();
        }

        while (itEntry != itEntryEnd)
        {
            auto *pEntry = *itEntry;
            if (pEntry != nullptr)
            {
                auto time = (pEntry->*getTime)();
                if (time >= startTime &&
                    time <= endTime)
                {
                    if (pEntry != nullptr)
                    {
                        if (deleter == nullptr)
                        {
                            delete pEntry;
                        }
                        else
                        {
                            (*deleter)(pEntry);
                        }
                    }

                    itEntry = m_entries.erase(itEntry);
                    continue;
                }
            }

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
        while (bSuccess &&
               itEntry != entries.cend())
        {
            auto *pEntry = entries.front();
            bSuccess = deleteEntry(pEntry);
            if (bSuccess)
            {
                itEntry = entries.erase(itEntry);
            }
            else
            {
                ++itEntry;
            }
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
                {
                    delete pEntry;
                }
                else
                {
                    (*deleter)(pEntry);
                }
            }

            if (size != m_entries.size())
            {
                itEntry = m_entries.cbegin();
            }
            else
            {
                ++itEntry;
            }
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
                {
                    delete pEntry;
                }
                else
                {
                    (*deleter)(pEntry);
                }

                pEntry = nullptr;
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
        return "TimeSortedContainer";
    }

    /**
     * Retrieve all entries tagged between the specified starting and ending times; upon success, returns true
     * and entries are appended to input argument
     */
    virtual bool getEntries(TimeType startTime,
                            TimeType endTime,
                            std::vector<EntryType *> &entries) const final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          startTime,
                                          m_comparator);
        auto &&itEntryEnd = std::upper_bound(itEntry,
                                             m_entries.cend(),
                                             endTime,
                                             m_comparator);
        if (itEntryEnd == itEntry)
        {
            itEntryEnd = m_entries.cend();
        }

        bool bSuccess = (itEntry != itEntryEnd);
        if (bSuccess)
        {
            while (bSuccess &&
                   itEntry != itEntryEnd)
            {
                auto *pEntry = *itEntry;
                bSuccess = (pEntry != nullptr);
                if (bSuccess)
                {
                    auto time = (pEntry->*getTime)();
                    if (time >= startTime &&
                        time <= endTime)
                    {
                        entries.push_back(pEntry);
                    }
                }

                ++itEntry;
            }
        }

        return bSuccess;
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
     * Retrieve an entry tagged at specified time; returns non-null upon success
     */
    virtual EntryType *getEntry(TimeType time) const final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          time,
                                          m_comparator);
        if (itEntry != m_entries.cend())
        {
            auto *pEntry = *itEntry;
            if (pEntry != nullptr &&
                (pEntry->*getTime)() == time)
            {
                return pEntry;
            }
        }

        return nullptr;
    }

    /**
     * Retrieve the latest available entry; returns non-null upon success
     */
    virtual EntryType *getLatestEntry(void) const final
    {
        if (!m_entries.empty())
        {
            return m_entries.back();
        }

        return nullptr;
    }

    /**
     * Retrieve the most recently available entry; returns non-null upon success
     */
    virtual EntryType *getMostRecentAvailableEntry(double time) const final
    {
        if (!m_entries.empty())
        {
            auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                              m_entries.cend(),
                                              time,
                                              m_comparator);
            if (itEntry == m_entries.cend())
            {
                --itEntry;
            }

            for (;; --itEntry)
            {
                auto *pEntry = *itEntry;
                if (pEntry != nullptr &&
                    (pEntry->*getTime)() <= time)
                {
                    return pEntry;
                }
                else if (itEntry == m_entries.cbegin())
                {
                    break;
                }
            }
        }

        return nullptr;
    }

    /**
     * Remove all entries tagged between the specified starting and ending times
     */
    virtual void removeEntries(TimeType startTime,
                               TimeType endTime) final
    {
        auto &&itEntry = std::lower_bound(m_entries.cbegin(),
                                          m_entries.cend(),
                                          startTime,
                                          m_comparator);
        auto &&itEntryEnd = std::upper_bound(itEntry,
                                             m_entries.cend(),
                                             endTime,
                                             m_comparator);
        if (itEntryEnd == itEntry)
        {
            itEntryEnd = m_entries.cend();
        }

        while (itEntry != itEntryEnd)
        {
            auto *pEntry = *itEntry;
            if (pEntry != nullptr)
            {
                auto time = (pEntry->*getTime)();
                if (time >= startTime &&
                    time <= endTime)
                {
                    itEntry = m_entries.erase(itEntry);
                    continue;
                }
            }

            ++itEntry;
        }
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
     * Return the number of entries contained within this object
     */
    inline virtual std::size_t size(void) const override
    {
        return m_entries.size();
    }

    /**
     * Swap function
     */
    virtual void swap(tTimeSortedContainer &container) override final
    {
        EntryContainer<EntryType>::swap(container);

        std::swap(m_comparator,
                  container.m_comparator);
        m_entries.swap(container.m_entries);
    }

protected:

    /**
     * this object's entry comparator
     */
    EntryComparator m_comparator;

    /**
     * this container's entries sorted by time
     */
    std::vector<EntryType *> m_entries;
};

}

#endif
