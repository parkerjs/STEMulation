#ifndef VARIABLE_REGISTRY_H
#define VARIABLE_REGISTRY_H

#include "iterable.h"
#include "inherited_iterator.h"
#include "output_streamable.h"
#include "reverse_iterable.h"
#include "swappable.h"
#include "variable_registry_entry.h"
#include <vector>

namespace utilities
{

/**
 * This class facilitates variable registration within a variable map and can be used in conjunction with a
 * variable dictionary
 */
class VariableRegistry
: public attributes::abstract::Iterable<iterators::Iterator, std::map<std::string, VariableRegistryEntry>,
                                        iterators::inherited_iterator_tag>,
  public attributes::concrete::OutputStreamable<VariableRegistry>,
  public attributes::abstract::ReverseIterable<iterators::Iterator, std::map<std::string,
                                                                             VariableRegistryEntry>,
                                               iterators::inherited_iterator_tag>,
  public attributes::interfaces::Swappable<VariableRegistry>
{
public:

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, VariableRegistry, iterators
                      ::inherited_iterator_tag>::const_iterator const_iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, VariableRegistry, iterators
                      ::inherited_iterator_tag>::const_reverse_iterator const_reverse_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, VariableRegistry, iterators
                      ::inherited_iterator_tag>::iterator iterator;
    typedef attributes::abstract::ReverseIterable<iterators::Iterator, VariableRegistry, iterators
                      ::inherited_iterator_tag>::reverse_iterator reverse_iterator;
    typedef std::map<std::string, VariableRegistryEntry,
                     std::function<bool (const std::string &, const std::string &)>> tRegistryEntries;
    typedef std::map<std::string, std::string> tTokenMap;

    /**
     * Constructor
     * @param keyComparator a function object which defines a custom key comparator
     */
    template<typename KeyComparator = std::less<std::string>,
             typename std::enable_if<!std::is_convertible<
             typename std::decay<KeyComparator>::type, tRegistryEntries>::value && !std::is_same<
             typename std::decay<KeyComparator>::type, VariableRegistry>::value, int>::type = 0>
    VariableRegistry(KeyComparator &&keyComparator = KeyComparator())
    : m_entries(std::forward<KeyComparator>(keyComparator))
    {

    }

    /**
     * Constructor (initializer list)
     * @param keyComparator a function object which defines a custom key comparator
     */
    template<typename KeyComparator = std::less<std::string>>
    VariableRegistry(std::initializer_list<std::pair<std::string, VariableRegistryEntry>> &&entries,
                     KeyComparator &&keyComparator = KeyComparator())
    : VariableRegistry(std::forward<KeyComparator>(keyComparator))
    {
        for (auto &&entry : entries)
            m_entries.emplace(std::move(entry));
    }

    /**
     * Constructor
     */
    VariableRegistry(const tRegistryEntries &entries)
    : m_entries(entries)
    {

    }

    /**
     * Copy constructor
     */
    VariableRegistry(const VariableRegistry &registry)
    {
        operator = (registry);
    }

    /**
     * Move constructor
     */
    VariableRegistry(VariableRegistry &&registry)
    {
        operator = (std::move(registry));
    }

    /**
     * Destructor
     */
    virtual ~VariableRegistry(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    VariableRegistry &operator = (const VariableRegistry &registry)
    {
        if (&registry != this)
        {
            m_entries = registry.m_entries;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    VariableRegistry &operator = (VariableRegistry &&registry)
    {
        if (&registry != this)
        {
            m_entries = std::move(registry.m_entries);
        }

        return *this;
    }

    /**
     * Subscript operator
     */
    inline virtual VariableRegistryEntry &operator [] (const std::string &name) final
    {
        return m_entries[name];
    }

    /**
     * Add a variable to the registry
     */
    template<typename T>
    inline void add(const std::string &name,
                    T &variable)
    {
        m_entries.emplace(name, VariableRegistryEntry(variable));
    }

    /**
     * Assign a value to a variable within the registry
     */
    template<typename T>
    bool assign(const std::string &name,
                T &value)
    {
        auto &&itRegistryEntry = m_entries.find(name);
        bool bSuccess = (itRegistryEntry != m_entries.end());
        if (bSuccess)
            bSuccess = itRegistryEntry->second.assign(value);

        return bSuccess;
    }

    /**
     * begin() overload
     */
    inline virtual iterator begin(void) override
    {
        return m_entries.begin();
    }

    /**
     * cbegin() overload
     */
    inline virtual const_iterator cbegin(void) const override
    {
        return m_entries.cbegin();
    }

    /**
     * cend() overload
     */
    inline virtual const_iterator cend(void) const override
    {
        return m_entries.cend();
    }

    /**
     * Clear the registry of all entries
     */
    inline virtual void clear(void) final
    {
        m_entries.clear();
    }

    /**
     * Determine if this registry contains an entry associated with the given name
     */
    inline virtual bool contains(const std::string &name) const final
    {
        return m_entries.find(name) != m_entries.cend();
    }

    /**
     * crbegin() overload
     */
    inline virtual const_reverse_iterator crbegin(void) const override
    {
        return m_entries.crbegin();
    }

    /**
     * Function to create token pairs from the input variable registry
     */
    tTokenMap createTokenPairs(void)
    {
        tTokenMap tokenMap;
        for (auto &&itRegistryEntry : m_entries)
        {
            auto &&variable = itRegistryEntry.first;
            auto &&value = itRegistryEntry.second;
            tokenMap.emplace(variable, value.toString());
        }

        return tokenMap;
    }

    /**
     * crend() overload
     */
    inline virtual const_reverse_iterator crend(void) const override
    {
        return m_entries.crend();
    }

    /**
     * Test for empty variable registry
     */
    inline virtual bool empty(void) const final
    {
        return m_entries.empty();
    }

    /**
     * end() overload
     */
    inline virtual iterator end(void) override
    {
        return m_entries.end();
    }

    /**
     * Return this registry's set of entries
     */
    inline virtual tRegistryEntries &entries(void) final
    {
        return m_entries;
    }

    /**
     * Return this registry's set of entries
     */
    inline virtual tRegistryEntries entries(void) const final
    {
        return m_entries;
    }

    /**
     * Erase the entry in this registry that is associated with the supplied iterator
     */
    inline virtual iterator erase(iterator it) final
    {
        return m_entries.erase(it);
    }

    /**
     * Searches the registry for an entry associated with the given name and returns an iterator to it if
     * found; otherwise it returns an iterator to end()
     */
    inline virtual iterator findByName(const std::string &name) final
    {
        return m_entries.find(name);
    }

    /**
     * Searches the registry for an entry associated with the given name and returns a constant iterator to it
     * if found; otherwise it returns an iterator to end()
     */
    inline virtual const_iterator findByName(const std::string &name) const final
    {
        return m_entries.find(name);
    }

    /**
     * Searches the registry for an entry associated with the given variable and returns an iterator to it if
     * found; otherwise it returns an iterator to end()
     */
    template<typename T>
    iterator findByVariable(T &variable)
    {
        auto &&itRegistryEntry = m_entries.begin();
        while (itRegistryEntry != m_entries.end())
        {
            T *pVariable = nullptr;
            try
            {
                pVariable = &registry_entry_cast<T &>(itRegistryEntry->second);
            }
            catch (bad_registry_entry_cast &)
            {
                // do nothing
            }

            if (pVariable == &variable)
                return itRegistryEntry;
            else
                ++itRegistryEntry;
        }

        return m_entries.end();
    }

    /**
     * Searches the registry for an entry associated with the given variable and returns a constant iterator to
     * it if found; otherwise it returns an iterator to end()
     */
    template<typename T>
    inline const_iterator findByVariable(const T &variable) const
    {
        auto *pRegistry = const_cast<VariableRegistry *>(this);

        return pRegistry->findByVariable(variable);
    }

    /**
     * Function to merge the input variable registry into this object's variable registry
     * @param registry   a reference to a VariableRegistry object
     * @param bOverwrite flag that specifies whether or not entries will be overwritten by the those within
     *                   the input map for which keys already exist in the current object's variable registry
     */
    inline virtual void merge(VariableRegistry &registry,
                              bool bOverwrite) final
    {
        merge(registry.entries(), bOverwrite);
    }

    /**
     * Function to merge the input variable registry entries into this object's variable registry
     * @param entries    a reference to a registry of variable entries
     * @param bOverwrite flag that specifies whether or not entries will be overwritten by the those within
     *                   the input map for which keys already exist in the current object's variable registry
     */
    virtual void merge(VariableRegistry::tRegistryEntries &entries,
                       bool bOverwrite) final
    {
        for (auto &&itRegistryEntry : entries)
        {
            auto &&variable = itRegistryEntry.first;
            auto &&itThisRegistryEntry = m_entries.find(variable);
            if (itThisRegistryEntry != m_entries.cend() && bOverwrite)
                itThisRegistryEntry->second.assign(itRegistryEntry.second);
            else
                m_entries[variable] = itRegistryEntry.second;
        }
    }

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    virtual std::ostream &print(std::ostream &stream) const override
    {
        if (stream)
        {
            auto &&registryEntries = entries();
            for (auto &&itRegistryEntry : registryEntries)
            {
                auto &&variable = itRegistryEntry.first;
                std::ostringstream oss;
                itRegistryEntry.second.print(oss);
                auto &&value = oss.str();
                if (!variable.empty() && !value.empty())
                    stream << " - " << itRegistryEntry.first
                           << " = " << value << std::endl;
            }
        }

        return stream;
    }

    /**
     * rbegin() overload
     */
    inline virtual reverse_iterator rbegin(void) override
    {
        return m_entries.rbegin();
    }

    /**
     * Remove an entry from the registry by name
     * @param bRecursive flag indicating whether or not to recursively search for and remove within the map all entries that link keys to the
     *                   same variable to which the input name is also mapped
     */
    virtual bool remove(const std::string &name,
                        bool bRecursive = false) final
    {
        auto &&itRegistryEntry = m_entries.find(name);
        bool bSuccess = (itRegistryEntry != m_entries.end());
        if (bSuccess)
            m_entries.erase(itRegistryEntry);

        return bSuccess;
    }

    /**
     * Remove all entries associated with the specified variable(s)
     */
    template<typename ... Args>
    bool removeVariables(Args && ... variables)
    {
        bool bFound = false;
        std::vector<VariableRegistryEntry> entries = { static_cast<VariableRegistryEntry>(variables) ... };
        for (auto &&entry : entries)
        {
            auto &&itRegistryEntry = m_entries.begin();
            while (itRegistryEntry != m_entries.end())
            {
                if (entry == itRegistryEntry->second)
                {
                    itRegistryEntry = m_entries.erase(itRegistryEntry);
                    bFound = true;
                }
                else
                    ++itRegistryEntry;
            }
        }

        return bFound;
    }

    /**
     * rend() overload
     */
    inline virtual reverse_iterator rend(void) override
    {
        return m_entries.rend();
    }

    /**
     * Retrieve a value from an entry within the registry
     */
    template<typename T>
    bool retrieveValue(const std::string &input,
                       T &value)
    {
        auto &&itRegistryEntry = m_entries.find(input);
        bool bSuccess = (itRegistryEntry != m_entries.cend());
        if (bSuccess)
            value = registry_entry_cast<T &>(itRegistryEntry->second);

        return bSuccess;
    }

    /**
     * Retrieve a pointer to a variable within the dictionary; returns non-null if it exists
     */
    template<typename T>
    T *retrieveVariable(const std::string &name)
    {
        T *pVariable = nullptr;
        auto &&itRegistryEntry = m_entries.find(name);
        if (itRegistryEntry != m_entries.cend())
            pVariable = &registry_entry_cast<T &>(itRegistryEntry->second);

        return pVariable;
    }

    /**
     * Return the number of entries contained within this registry
     */
    inline virtual size_t size(void) const final
    {
        return m_entries.size();
    }

    /**
     * Swap function
     */
    virtual void swap(VariableRegistry &registry) override final
    {
        m_entries.swap(registry.m_entries);
    }

    /**
     * Update this object's variable registry with entries in the input variable map
     */
    virtual bool update(const tRegistryEntries &entries) final
    {
        bool bSuccess = true; // assume success
        for (auto &&itThisRegistryEntry : m_entries)
        {
            auto &&variable = itThisRegistryEntry.first;
            auto &&itRegistryEntry = entries.find(variable);
            if (itRegistryEntry != entries.cend())
                bSuccess &= itThisRegistryEntry.second.assign(itRegistryEntry->second);

            if (!bSuccess)
                break;
        }

        return bSuccess;
    }

    /**
     * Update this object's variable registry with entries in another variable registry
     */
    inline virtual bool update(const VariableRegistry &registry) final
    {
        return update(registry.entries());
    }

private:

    /**
     * map of token-variable registry entry object pairs
     */
    tRegistryEntries m_entries;
};

}

#endif
