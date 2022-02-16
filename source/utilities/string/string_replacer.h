#ifndef STRING_REPLACER_H
#define STRING_REPLACER_H

#include <functional>
#include <map>
#include <regex>

namespace utilities
{

namespace string
{

/**
 * This class contains methods to facilitate substring search and replace operations on std::string objects (T
 * = std::string or std::regex)
 */
template<typename T = std::string>
class StringReplacer final
{
public:

    /**
     * Typedef declarations
     */
    typedef std::function<bool (std::string &, std::string::size_type,
                                std::string::size_type)> tStringManipulator;

    /**
     * Constructor
     */
    template<typename U = T, typename std::enable_if<std::is_same<U, std::string>::value ||
                                                     std::is_same<U, std::regex>::value, int>::type = 0>
    StringReplacer(void)
    {

    }

    /**
     * Construct a StringReplacer object with a map of find and replace substrings
     */
    template<typename U = T, typename std::enable_if<std::is_same<U, std::string>::value ||
                                                     std::is_same<U, std::regex>::value, int>::type = 0>
    StringReplacer(const std::map<std::string, std::string> &searchAndReplaceMap)
    : m_searchAndReplaceMap(searchAndReplaceMap)
    {

    }

    /**
     * Construct a StringReplacer object with a map of find and replace substrings
     */
    StringReplacer(const std::string &search, const std::string &replace)
    {
        add(search, replace);
    }

    /**
     * Copy constructor
     */
    StringReplacer(const StringReplacer<T> &replacer)
    : m_searchAndReplaceMap(replacer.m_searchAndReplaceMap)
    {

    }

    /**
     * Move constructor
     */
    StringReplacer(StringReplacer<T> &&replacer)
    : m_searchAndReplaceMap(std::move(replacer.m_searchAndReplaceMap))
    {

    }

    /**
     * Destructor
     */
    virtual ~StringReplacer(void)
    {

    }

    /**
     * Add a pair of search and replace substrings to the map
     * @param search  the search string
     * @param replace the string that will replace the search string, if found
     */
    inline virtual void add(const std::string &search, const std::string &replace) final
    {
        m_searchAndReplaceMap[search] = replace;
    }

    /**
     * Get this object's map of search and replace pairs
     */
    inline virtual std::map<std::string, std::string> &getSearchAndReplaceMap(void) final
    {
        return m_searchAndReplaceMap;
    }

    /**
     * Function to replace a string of characters between the specified delimiters with another string
     * @param string            the character sequence upon which the search and replace will be
     *                          performed
     * @param replace           the string that will replace the string bounded by the starting and
     *                          ending delimiters, if found
     * @param startDelimiter    the starting delimiter for which to search
     * @param endDelimiter      the ending delimiter for which to search
     * @param bConsumeDelimiter flag, if true, instructs the function to consume both delimiters (if found)
     */
    template<typename U, typename V = T, typename std::enable_if<std::is_same<V, std::string>::value &&
             std::is_convertible<typename std::decay<U>::type, std::string>::value, int>::type = 0>
    inline static void searchAndReplace(std::string &string, U &&replace,
                                        const std::string &startDelimiter, const std::string &endDelimiter,
                                        bool bConsumeDelimiters = true)
    {
        auto &&manipulator = [&] (std::string &, std::string::size_type start, std::string::size_type length)
        {
            auto &&begin = string.begin() + start;
            string.erase(begin, begin + length);
            string.insert(start, replace);

            return true;
        };

        searchAndReplace(string, manipulator, startDelimiter, endDelimiter, bConsumeDelimiters);
    }

    /**
     * Function to manipulate a substring bounded by the specified delimiters
     * @param string            the character sequence upon which the search will be performed
     * @param manipulator       an std::function object that manipulates the substring found during the
     *                          search
     * @param startDelimiter    the starting delimiter for which to search
     * @param endDelimiter      the ending delimiter for which to search
     * @param bConsumeDelimiter flag, if true, instructs the function to consume both delimiters (if found)
     */
    template<typename U, typename V = T, typename std::enable_if<std::is_same<V, std::string>::value &&
             std::is_convertible<typename std::decay<U>::type, tStringManipulator>::value &&
            !std::is_convertible<typename std::decay<U>::type, std::string>::value, int>::type = 0>
    static void searchAndReplace(std::string &string, U &&manipulator, const std::string &startDelimiter,
                                 const std::string endDelimiter, bool bConsumeDelimiters = true)
    {
        std::string::size_type endPos = 0;
        auto &&sizeEndDelimiter = endDelimiter.size();
        auto &&sizeStartDelimiter = startDelimiter.size();
        while (true)
        {
            auto &&startPos = string.find(startDelimiter, endPos);
            if (startPos != std::string::npos)
            {
                endPos = string.find(endDelimiter, startPos + sizeStartDelimiter);
                if (endPos != std::string::npos)
                {
                    if (bConsumeDelimiters)
                    {
                        string.erase(endPos, sizeEndDelimiter);
                        string.erase(startPos, sizeStartDelimiter);
                        endPos -= sizeEndDelimiter + sizeStartDelimiter;
                    }

                    // now operate on the string using the manipulation functor...
                    manipulator(string, startPos, endPos - startPos + 1);
                    ++endPos;

                    continue;
                }
            }

            break;
        }
    }

    /**
     * Function to find and replace substrings in the input string with those specified in this object's map of
     * search and replace strings (enabled if T = std::string)
     * @param string           the character sequence upon which the search and replace will be
     *                         performed
     * @param bWholeWordSearch enables/disables whole word search (false by default)
     */
    template<typename U = T, typename std::enable_if<std::is_same<U, std::string>::value, int>::type = 0>
    void searchAndReplace(std::string &string, bool bWholeWordSearch = false)
    {
        searchAndReplace(string, 0, string.length(), bWholeWordSearch);
    }

    /**
     * Function to find and replace substrings in the input string with those specified in this object's map of
     * search and replace strings (enabled if T = std::string)
     * @param string           the character sequence upon which search and replace will be performed
     * @param pos              the beginning position of the input substring that will be searched
     * @param length           the length of the input substring, starting from pos, that will be
     *                         searched
     * @param bWholeWordSearch enables/disables whole word search (false by default)
     */
    template<typename U = T, typename std::enable_if<std::is_same<U, std::string>::value, int>::type = 0>
    void searchAndReplace(std::string &string, std::string::size_type pos, std::string::size_type length,
                          bool bWholeWordSearch = false)
    {
        std::string::size_type end = pos + length;
        while (true)
        {
            // pick the first-occurring search substring...
            auto &&itSearchAndReplacePair = m_searchAndReplaceMap.end();
            auto &&itNextSearchAndReplacePair = m_searchAndReplaceMap.begin();
            std::string::size_type begin = end;
            for (; itNextSearchAndReplacePair != m_searchAndReplaceMap.cend(); ++itNextSearchAndReplacePair)
            {
                auto &&search = itNextSearchAndReplacePair->first;
                auto &&nextPos = string.find(search, pos);
                if (nextPos != std::string::npos && nextPos < begin)
                {
                    if (bWholeWordSearch)
                    {
                        auto &&searchEnd = nextPos + search.size();
                        if (searchEnd != end)
                        {
                            auto &&ch = string[searchEnd];
                            if (isalnum(ch) == 1 || ch == '_')
                                continue;
                        }

                        if (nextPos > 0)
                        {
                            auto &&ch = string[nextPos - 1];
                            if (isalnum(ch) == 1 || ch == '_')
                                continue;
                        }
                    }

                    begin = nextPos;
                    itSearchAndReplacePair = itNextSearchAndReplacePair;
                }
            }

            if (itSearchAndReplacePair != m_searchAndReplaceMap.cend())
            {
                auto &&search = itSearchAndReplacePair->first;
                auto &&replace = itSearchAndReplacePair->second;
                if (begin + search.size() <= end)
                {
                    string.replace(begin, search.length(), replace);
                    pos = begin + std::min(std::string::size_type(1), replace.size());

                    auto &&deltaSize = int(replace.size()) - int(search.size());
                    length += deltaSize;
                    end += deltaSize;

                    continue;
                }
            }

            break;
        }
    }

    /**
     * Function to find and replace substrings in the input string with those specified in this object's map of
     * search and replace strings
     * @param string  the character sequence upon which the search and replace will be performed
     * @param bRescan a flag that indicates whether or not the string will be re-scanned until all possible
     *                matches have been replaced (for example, a given search and replace may create another
     *                match that can only be replaced by re-scanning the string for matches; necessary for now
     *                since C++ regex currently does not support look-behind)
     */
    template<typename U = T, typename std::enable_if<std::is_same<U, std::regex>::value, int>::type = 0>
    void searchAndReplace(std::string &string, bool bRescan = false)
    {
        searchAndReplace(string, 0, string.size(), bRescan);
    }

    /**
     * Function to find and replace substrings in the input string with those specified in this object's map of
     * search and replace strings
     * @param string  the character sequence upon which the search and replace will be performed
     * @param pos     the beginning position of the input substring that will be searched
     * @param length  the length of the input substring, starting from pos, that will be searched
     * @param bRescan a flag that indicates whether or not the string will be re-scanned until all possible
     *                matches have been replaced (for example, a given search and replace may create another
     *                match that can only be replaced by re-scanning the string for matches; necessary for now
     *                since C++ regex currently does not support look-behind)
     */
    template<typename U = T, typename std::enable_if<std::is_same<U, std::regex>::value, int>::type = 0>
    void searchAndReplace(std::string &string, std::string::size_type pos, std::string::size_type length,
                          bool bRescan = false)
    {
        for (auto &&itSearchAndReplacePair : m_searchAndReplaceMap)
        {
            auto &&search = std::regex(itSearchAndReplacePair.first);
            auto &&replace = itSearchAndReplacePair.second;
            while (true)
            {
                auto &&begin = string.begin() + pos;
                auto &&end = begin + length;
                if (std::regex_search(begin, begin + length, search))
                {
                    std::string replaced;
                    auto sizeBefore = std::distance(begin, end);
                    std::regex_replace(std::back_inserter(replaced), begin, end, search, replace);
                    auto &&deltaSize = sizeBefore - int(replaced.size());
                    string.erase(pos, sizeBefore);
                    string.insert(pos, replaced);
                    length -= deltaSize;
                    if (bRescan)
                        continue;
                }

                break;
            }
        }
    }

private:

    /**
     * a map of search and replace string pairs
     */
    std::map<std::string, std::string> m_searchAndReplaceMap;
};

}

}

#endif
