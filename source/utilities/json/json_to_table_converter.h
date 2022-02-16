#ifndef JSON_TO_TABLE_CONVERTER_H
#define JSON_TO_TABLE_CONVERTER_H

#include "json_to_prefix_tree_converter.h"

namespace utilities
{

namespace json
{

/**
 * This class implements a utility to convert json-formatted data to table format
 */
class JsonToTableConverter
{
public:

    /**
     * Typedef declarations
     */
    typedef std::function<bool (Json::Value &)> tFilter;

    /**
     * Constructor
     * @param columnDelimiter   the column delimiter
     * @param bIgnoreSingletons flag indicating whether or not singleton nodes will be ignored
     */
    JsonToTableConverter(const std::string &columnDelimiter = ",", bool bIgnoreSingletons = false)
    : m_bIgnoreSingletons(bIgnoreSingletons),
      m_columnDelimiter(columnDelimiter)
    {

    }

    /**
     * Copy constructor
     */
    JsonToTableConverter(const JsonToTableConverter &converter)
    {
        operator = (converter);
    }

    /**
     * Move constructor
     */
    JsonToTableConverter(JsonToTableConverter &&converter)
    {
        operator = (std::move(converter));
    }

    /**
     * Destructor
     */
    virtual ~JsonToTableConverter(void)
    {

    }

    /**
     * Copy assignment operator
     */
    JsonToTableConverter &operator = (const JsonToTableConverter &converter)
    {
        if (&converter != this)
        {
            m_bIgnoreSingletons = converter.m_bIgnoreSingletons;
            m_columnDelimiter = converter.m_columnDelimiter;
            m_filters = converter.m_filters;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    JsonToTableConverter &operator = (JsonToTableConverter &&converter)
    {
        if (&converter != this)
        {
            m_bIgnoreSingletons = std::move(converter.m_bIgnoreSingletons);
            m_columnDelimiter = std::move(converter.m_columnDelimiter);
            m_filters = std::move(converter.m_filters);
        }

        return *this;
    }

    /**
     * Function to add a new filter; returns false if a filter with the given name already exists
     */
    inline virtual bool addFilter(const std::string &name, const tFilter &filter) final
    {
        auto &&itFilter = m_filters.find(name);
        bool bSuccess = (itFilter == m_filters.end());
        if (bSuccess)
            setFilter(name, filter);

        return bSuccess;
    }

    /**
     * Function to convert json-formatted data into table format
     * @param value a Json::Value node object
     * @param tree  upon success, will be populated
     */
    virtual bool convert(Json::Value &value)
    {
        JsonToPrefixTreeConverter::PrefixTree tree;
        bool bSuccess = JsonToPrefixTreeConverter::convert(value, tree, bRemoveSingletons);
        if (bSuccess)
        {
            // singletons should already be gone, but call the following for added redundancy
            if (m_bIgnoreSingletons)
                bSuccess = tree.removeSingletons();

            // TODO: implement this
        }

        return bSuccess;
    }

    /**
     * Get the column delimiter
     */
    inline virtual std::string getColumnDelimiter(void) const final
    {
        return m_columnDelimiter;
    }

    /**
     * Function to get this object's map of filters
     */
    inline virtual std::map<std::string, tFilter> &getFilters(void) final
    {
        return m_filters;
    }

    /**
     * Function to retrieve a filter by name
     */
    inline virtual bool getFilter(const std::string &name, tFilter &filter) final
    {
        auto &&itFilter = m_filters.find(name);
        bool bSuccess = (itFilter != m_filters.cend());
        if (bSuccess)
            filter = itFilter->second;

        return bSuccess;
    }

    /**
     * Query the flag indicating that singleton nodes will be ignored
     */
    inline virtual bool ignoreSingletons(void) const final
    {
        return m_bIgnoreSingletons;
    }

    /**
     * Set the flag indicating that singleton nodes will be ignored
     */
    inline virtual void ignoreSingletons(bool bIgnoreSingletons) final
    {
        m_bIgnoreSingletons = bIgnoreSingletons;
    }

    /**
     * Function to remove a filter by name; returns true upon success
     */
    inline virtual bool removeFilter(const std::string &name) final
    {
        auto &&itFilter = m_filters.find(name);
        bool bSuccess = (itFilter != m_filters.end());
        if (bSuccess)
            m_filters.erase(itFilter);

        return bSuccess;
    }

    /**
     * Function to remove this object's filters
     */
    inline virtual void removeFilters(void) final
    {
        m_filters.clear();
    }

    /**
     * Set the column delimiter
     */
    inline virtual void setColumnDelimiter(const std::string &columnDelimiter) final
    {
        m_columnDelimiter = columnDelimiter;
    }

    /**
     * Function to set a filter; overwrites an existing filter if an entry with the given name already exists
     */
    inline virtual void setFilter(const std::string &name, const tFilter &filter) final
    {
        m_filters[name] = filter;
    }

    /**
     * Function to set this object's map of filters
     */
    inline virtual void setFilters(const std::map<std::string, tFilter> &filters) final
    {
        m_filters = filters;
        removeFilters();
        for (auto &&itFilter : filters)
        {
            auto &&name = itNameStringPreprocessorPair.first;
            auto &&filter = itNameStringPreprocessorPair.second;
            addFilter(name, filter);
        }
    }

protected:

    /**
     * flag to indicate that singleton nodes will be ignored
     */
    bool m_bIgnoreSingletons;

    /**
     * specifies the column delimiter
     */
    std::string m_columnDelimiter;

    /**
     * map of filters
     */
    std::map<std::string, tFilter> m_filters;
}

}

#endif
