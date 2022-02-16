#ifndef IDENTIFIER_AND_TIME_SORTED_CONTAINER_H
#define IDENTIFIER_AND_TIME_SORTED_CONTAINER_H

#include "entry_container.h"
#include <algorithm>
#include <map>

namespace containers
{

/**
 * A container class for storing and retrieving entries sorted first by an associated identifier and then by
 * time
 */
template<typename EntryType,
         typename IdType = std::size_t,
         typename TimeType = double,
         IdType (EntryType::*getEntryId)(void) const = &EntryType::getEntryId,
         TimeType (EntryType::*getTime)(void) const = &EntryType::getAvailabilityTime,
         EntryType * (EntryType::*clone)(void) const = &EntryType::clone,
         void (*deleter)(EntryType *&) = nullptr>
class IdentifierAndTimeSortedContainer
: public EntryContainer<EntryType>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<IdentifierAndTimeSortedContainer<EntryType,
                                                                            IdType,
                                                                            TimeType,
                                                                            getEntryId,
                                                                            getTime,
                                                                            clone,
                                                                            deleter>>
{
public:

    /**
     * Typedef declarations
     */
    typedef IdentifierAndTimeSortedContainer<EntryType,
                                             IdType,
                                             TimeType,
                                             getEntryId,
                                             getTime,
                                             clone,
                                             deleter> tIdentifierAndTimeSortedContainer;

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
    IdentifierAndTimeSortedContainer(void)
    {

    }

    /**
     * Copy constructor
     */
    IdentifierAndTimeSortedContainer(const tIdentifierAndTimeSortedContainer &container)
    {
        operator = (container);
    }

    /**
     * Move constructor
     */
    IdentifierAndTimeSortedContainer(tIdentifierAndTimeSortedContainer &&container)
    {
        operator = (std::move(container));
    }

    /**
     * Destructor
     */
    virtual ~IdentifierAndTimeSortedContainer(void) override
    {
        deleteEntries();
    }

    /**
     * Copy assignment operator
     */
    tIdentifierAndTimeSortedContainer &operator = (const tIdentifierAndTimeSortedContainer &container)
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
    tIdentifierAndTimeSortedContainer &operator = (tIdentifierAndTimeSortedContainer &&container)
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
            auto entryId = (pEntry->*getEntryId)();
            auto &&entries = m_entries[entryId];
            auto &&itEntry = std::lower_bound(entries.cbegin(),
                                              entries.cend(),
                                              time,
                                              m_comparator);

            EntryType *pThisEntry = nullptr;
            if (itEntry != entries.cend())
                pThisEntry = *itEntry;

            if (pThisEntry != nullptr && pThisEntry == pEntry)
            {
                return false;
            }
            else if (pThisEntry != nullptr && (pThisEntry->*getTime)() == time)
            {
                // an entry with the same entry id already exists in this object's entries, so replace the
                // current entry with the new one
                if (deleter == nullptr)
                    delete pThisEntry;
                else
                    (*deleter)(pThisEntry);

                itEntry = entries.erase(itEntry);
            }

            entries.insert(itEntry, pEntry);
            while (entries.size() > this->getMaxSize())
                deleteEntry(entries.front());
        }

        return bSuccess;
    }

    /**
     * Add an entry to this container
     */
    virtual void addEntry(const EntryType &entry) override
    {
        auto time = (entry.*getTime)();
        auto entryId = (entry.*getEntryId)();
        auto &&entries = m_entries[entryId];
        auto &&itEntry = std::lower_bound(entries.cbegin(),
                                          entries.cend(),
                                          time,
                                          m_comparator);

        EntryType *pEntry = nullptr;
        if (itEntry != entries.cend())
            pEntry = *itEntry;

        if (pEntry == nullptr || (pEntry->*getTime)() != time)
        {
            if (entries.size() >= this->getMaxSize())
            {
                if (itEntry == entries.cend())
                {
                    // perform a simple rotation and assign the current entry to the back
                    std::rotate(entries.begin(),
                                entries.begin() + 1,
                                entries.end());

                    pEntry = entries.back();
                    if (pEntry != nullptr)
                        *pEntry = entry;
                }
                else
                {
                    entries.emplace(itEntry, (entry.*clone)());
                    while (entries.size() > this->getMaxSize())
                    {
                        deleteEntry(entries.front());
                    }
                }
            }
            else
            {
                entries.emplace(itEntry, (entry.*clone)());
            }
        }
        else if (pEntry != nullptr)
        {
            // an entry with the same entry id and time already exists in this object's
            // entry map, so update the current entry with the new one
            *pEntry = entry;
        }
    }

    /**
     * Copy the entries from the input container to this object
     */
    virtual void copyEntries(const tIdentifierAndTimeSortedContainer &container) final
    {
        // delete this object's current entries before cloning
        deleteEntries();

        // clone or copy entries from the input container
        auto &&itEntries = container.m_entries.cbegin();
        while (itEntries != container.m_entries.cend())
        {
            auto &&entries = itEntries->second;
            auto &&itEntry = entries.cbegin();
            while (itEntry != entries.cend())
            {
                auto *pEntry = *itEntry;
                if (pEntry != nullptr)
                    addEntry(*pEntry);

                ++itEntry;
            }

            ++itEntries;
        }
    }

    /**
     * Delete all entries associated with the specified entry id
     */
    virtual void deleteEntries(IdType entryId) final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            while (!entries.empty())
            {
                deleteEntry(entries.front());
            }

            m_entries.erase(itEntries);
        }
    }

    /**
     * Delete all entries tagged between the specified starting and ending times
     */
    virtual void deleteEntries(TimeType startTime,
                               TimeType endTime) final
    {
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            auto &&entryId = itEntries->first;
            itEntries = deleteEntries(entryId, startTime, endTime);
            if (itEntries != m_entries.cend() && itEntries->first == entryId)
                ++itEntries;
        }
    }

    /**
     * Delete all entries with the specified entry id tagged between the specified starting and ending times;
     * returns an iterator to that which follows the last element erased by this function
     */
    virtual typename std::map<IdType, std::vector<EntryType *>>::iterator
    deleteEntries(IdType entryId,
                  TimeType startTime,
                  TimeType endTime) final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            auto &&itEntry = std::lower_bound(entries.cbegin(),
                                              entries.cend(),
                                              startTime,
                                              m_comparator);
            auto &&itEntryEnd = std::upper_bound(itEntry,
                                                 entries.cend(),
                                                 endTime,
                                                 m_comparator);
            if (itEntryEnd == itEntry)
                itEntryEnd = entries.cend();

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
                                delete pEntry;
                            else
                                (*deleter)(pEntry);
                        }

                        itEntry = entries.erase(itEntry);
                        continue;
                    }
                }

                ++itEntry;
            }

            if (entries.empty())
                itEntries = m_entries.erase(itEntries);
        }

        return itEntries;
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
        auto size_i = m_entries.size();
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            auto &&itEntry = entries.cbegin();
            while (itEntry != entries.cend())
            {
                auto size_j = entries.size();
                auto *pEntry = *itEntry;
                if (pEntry != nullptr)
                {
                    if (deleter == nullptr)
                        delete pEntry;
                    else
                        (*deleter)(pEntry);
                }

                if (size_i != m_entries.size())
                    break;

                if (size_j != entries.size())
                    itEntry = entries.cbegin();
                else
                    ++itEntry;
            }

            if (size_i != m_entries.size())
                itEntries = m_entries.cbegin();
            else
                ++itEntries;
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
            auto &&entryId = (pEntry->*getEntryId)();
            auto &&itEntries = m_entries.find(entryId);
            if (itEntries != m_entries.cend())
            {
                auto &&entries = itEntries->second;
                auto &&itEntry = std::find(entries.cbegin(),
                                           entries.cend(),
                                           pEntry);
                bSuccess = (itEntry != entries.cend());
                if (bSuccess)
                {
                    if (deleter == nullptr)
                        delete pEntry;
                    else
                        (*deleter)(pEntry);

                    pEntry = nullptr;
                    entries.erase(itEntry);
                    if (entries.empty())
                        m_entries.erase(itEntries);
                }
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
        return "IdentifierAndTimeSortedContainer";
    }

    /**
     * Retrieve all entries associated with the specified entry id; upon success, entries are appended to input
     * argument
     */
    virtual void getEntries(IdType entryId,
                            std::vector<EntryType *> &entries) const final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            entries.insert(entries.cend(),
                           itEntries->second.cbegin(),
                           itEntries->second.cend());
        }
    }

    /**
     * Retrieve all entries tagged between the specified starting and ending times; upon success, entries are
     * appended to input argument
     */
    virtual void getEntries(TimeType startTime,
                            TimeType endTime,
                            std::vector<EntryType *> &entries) const final
    {
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            auto &&entryId = itEntries->first;
            getEntries(entryId, startTime, endTime, entries);
            ++itEntries;
        }
    }

    /**
     * Retrieve all entries associated with the specified entry id tagged between the specified starting and
     * ending times; upon success, returns true and entries are appended to input argument
     */
    virtual bool getEntries(IdType entryId,
                            TimeType startTime,
                            TimeType endTime,
                            std::vector<EntryType *> &entries) const final
    {
        auto &&itEntries = m_entries.find(entryId);
        bool bSuccess = (itEntries != m_entries.cend());
        if (bSuccess)
        {
            auto &&itEntry = std::lower_bound(itEntries->second.cbegin(),
                                              itEntries->second.cend(),
                                              startTime,
                                              m_comparator);
            auto &&itEntryEnd = std::upper_bound(itEntry,
                                                 itEntries->second.cend(),
                                                 endTime,
                                                 m_comparator);
            if (itEntryEnd == itEntry)
                itEntryEnd = itEntries->second.cend();

            bSuccess = (itEntry != itEntryEnd);
            if (bSuccess)
            {
                while (bSuccess && itEntry != itEntryEnd)
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
        }

        return bSuccess;
    }

    /**
     * Get this container's entry map
     */
    inline virtual std::map<IdType, std::vector<EntryType *>> &getEntries(void) final
    {
        return m_entries;
    }

    /**
     * Get this container's entry map
     */
    inline virtual std::map<IdType, std::vector<EntryType *>> getEntries(void) const final
    {
        return m_entries;
    }

    /**
     * Retrieve an entry tagged at the specified time with the given entry id; returns non-null upon success
     */
    virtual EntryType *getEntry(IdType entryId,
                                TimeType time) const final
    {
        auto &&itEntries = m_entries.find(entryId);
        bool bSuccess = (itEntries != m_entries.cend());
        if (bSuccess)
        {
            auto &&entries = itEntries->second;
            auto &&itEntry = std::lower_bound(entries.cbegin(),
                                              entries.cend(),
                                              time,
                                              m_comparator);
            if (itEntry != entries.cend())
            {
                auto *pEntry = *itEntry;
                if (pEntry != nullptr && (pEntry->*getTime)() == time)
                    return pEntry;
            }
        }

        return nullptr;
    }

    /**
     * Retrieve the latest entries for all entry ids; upon success, entries are appended to input argument
     */
    virtual void getLatestEntries(std::vector<EntryType *> &entries) const final
    {
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            auto &&entryId = itEntries->first;
            auto *pEntry = getLatestEntry(entryId);
            if (pEntry != nullptr)
                entries.push_back(pEntry);

            ++itEntries;
        }
    }

    /**
     * Retrieve the latest entry associated with the specified entry id; returns non-null upon success
     */
    virtual EntryType *getLatestEntry(IdType entryId) const final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            if (!entries.empty())
                return entries.back();
        }

        return nullptr;
    }

    /**
     * Retrieve the most recently available entries for all entry ids; matching entries are appended to input
     * argument
     */
    virtual void getMostRecentAvailableEntries(double time,
                                               std::vector<EntryType *> &entries) const final
    {
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            auto &&entryId = itEntries->first;
            auto *pEntry = getMostRecentAvailableEntry(entryId, time);
            if (pEntry != nullptr)
                entries.push_back(pEntry);

            ++itEntries;
        }
    }

    /**
     * Retrieve the most recently available entry associated with the specified entry id; returns non-null upon
     * success
     */
    virtual EntryType *getMostRecentAvailableEntry(IdType entryId,
                                                   double time) const final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            if (!entries.empty())
            {
                auto &&itEntry = std::lower_bound(entries.cbegin(),
                                                  entries.cend(),
                                                  time,
                                                  m_comparator);
                if (itEntry == entries.cend())
                    --itEntry;

                for (;; --itEntry)
                {
                    auto *pEntry = *itEntry;
                    if (pEntry != nullptr && (pEntry->*getTime)() <= time)
                        return pEntry;
                    else if (itEntry == entries.cbegin())
                        break;
                }
            }
        }

        return nullptr;
    }

    /**
     * Remove all entries associated with the specified entry id
     */
    virtual void removeEntries(IdType entryId) final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            while (!entries.empty())
            {
                removeEntry(entries.front());
            }

            m_entries.erase(itEntries);
        }
    }

    /**
     * Remove all entries tagged between the specified starting and ending times
     */
    virtual void removeEntries(TimeType startTime,
                               TimeType endTime) final
    {
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            auto &&entryId = itEntries->first;
            itEntries = removeEntries(entryId, startTime, endTime);
            if (itEntries != m_entries.cend() && itEntries->first == entryId)
                ++itEntries;
        }
    }

    /**
     * Remove all entries with the specified entry id tagged between the specified starting and ending times;
     * returns an iterator to that which follows the last element erased by this function
     */
    virtual typename std::map<IdType, std::vector<EntryType *>>::iterator
    removeEntries(IdType entryId,
                  TimeType startTime,
                  TimeType endTime) final
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
        {
            auto &&entries = itEntries->second;
            auto &&itEntry = std::lower_bound(entries.cbegin(),
                                              entries.cend(),
                                              startTime,
                                              m_comparator);
            auto &&itEntryEnd = std::upper_bound(itEntry,
                                                 entries.cend(),
                                                 endTime,
                                                 m_comparator);
            if (itEntryEnd == itEntry)
                itEntryEnd = entries.cend();

            while (itEntry != itEntryEnd)
            {
                auto *pEntry = *itEntry;
                if (pEntry != nullptr)
                {
                    auto time = (pEntry->*getTime)();
                    if (time >= startTime &&
                        time <= endTime)
                    {
                        itEntry = entries.erase(itEntry);
                        continue;
                    }
                }

                ++itEntry;
            }

            if (entries.empty())
                itEntries = m_entries.erase(itEntries);
        }

        return itEntries;
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
            auto &&entryId = (pEntry->*getEntryId)();
            auto &&itEntries = m_entries.find(entryId);
            if (itEntries != m_entries.cend())
            {
                auto &&entries = itEntries->second;
                auto &&itEntry = std::find(entries.cbegin(),
                                           entries.cend(),
                                           pEntry);
                bSuccess = (itEntry != entries.cend());
                if (bSuccess)
                {
                    entries.erase(itEntry);
                    if (entries.empty())
                        m_entries.erase(itEntries);
                }
            }
        }

        return bSuccess;
    }

    /**
     * Return the number of entries contained within this object
     */
    virtual std::size_t size(void) const override
    {
        std::size_t numEntries = 0;
        auto &&itEntries = m_entries.cbegin();
        while (itEntries != m_entries.cend())
        {
            numEntries += size(itEntries->first);
            ++itEntries;
        }

        return numEntries;
    }

    /**
     * Return the number of entries associated with the given entry id contained within this object
     */
    virtual std::size_t size(IdType entryId) const
    {
        auto &&itEntries = m_entries.find(entryId);
        if (itEntries != m_entries.cend())
            return itEntries->second.size();

        return 0;
    }

    /**
     * Swap function
     */
    virtual void swap(tIdentifierAndTimeSortedContainer &container) override final
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
     * this container's entries sorted first by entry id and then time
     */
    std::map<IdType, std::vector<EntryType *>> m_entries;
};

}

#endif
