#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include <algorithm>
#include <cctype>
#include <regex>
#include <string>

namespace utilities
{

namespace string
{

/**
 * This class contains string manipulation methods
 */
class StringUtilities final
{
public:

    /**
     * Constructor
     */
    StringUtilities(void)
    {

    }

    /**
     * Destructor
     */
    ~StringUtilities(void)
    {

    }

    /**
     * Case insensitive string comparator
     */
    inline static bool caseInsensitiveComparator(const std::string &left,
                                                 const std::string &right)
    {
        // define a case-insensitive string comparator
        auto &&comparator = [] (auto &lhs, auto &rhs) { return std::tolower(lhs) == std::tolower(rhs); };

        return left.size() == right.size() &&
               std::equal(left.cbegin(), left.cend(), right.cbegin(), comparator);
    }

    /**
     * Case insensitive lexicographical string comparator
     */
    inline static bool caseInsensitiveLexicographicalComparator(const std::string &left,
                                                                const std::string &right)
    {
        // define a case-insensitive string comparator
        auto &&comparator = [] (auto &lhs, auto &rhs) { return tolower (lhs) < tolower (rhs); };

        return std::lexicographical_compare(left.cbegin(), left.cend(), right.cbegin(), right.cend(),
                                            comparator);
    }

    /**
     * Concatenate an argument list of strings
     */
    template<typename Left, typename Right, typename ... Strings,
             typename std::enable_if<(sizeof ... (Strings) > 0), int>::type = 0>
    inline static auto concatenate(Left &&left,
                                   Right &&right,
                                   Strings && ... strings)
    {
        return StringUtilities::concatenate(StringUtilities::concatenate(std::forward<Left>(left),
                                                                         std::forward<Right>(right)),
                                            std::forward<Strings>(strings) ...);
    }

    /**
     * Concatenate two strings
     */
    template<typename Left, typename Right,
             typename std::enable_if<(std::is_assignable<std::string, typename std::decay<Left>::type>::value &&
                                      std::is_same<typename std::decay<Right>::type, std::string>::value) ||
                                     (std::is_same<typename std::decay<Left>::type, std::string>::value &&
                                      std::is_assignable<std::string, typename std::decay<Right>::type>::value) ||
                                     (std::is_assignable<std::wstring, typename std::decay<Left>::type>::value &&
                                      std::is_same<typename std::decay<Right>::type, std::wstring>::value) ||
                                     (std::is_same<typename std::decay<Left>::type, std::wstring>::value &&
                                      std::is_assignable<std::wstring, typename std::decay<Right>::type>::value),
                                     int>::type = 0>
    inline static auto concatenate(Left &&left,
                                   Right &&right)
    {
        return std::forward<Left>(left) + std::forward<Right>(right);
    }

    /**
     * Concatenate two strings
     */
    inline static auto concatenate(const char *pLeft,
                                   const char *pRight)
    {
        return std::string(pLeft) + std::string(pRight);
    }

    /**
     * Concatenate two strings
     */
    inline static auto concatenate(const wchar_t *pLeft,
                                   const wchar_t *pRight)
    {
        return std::wstring(pLeft) + std::wstring(pRight);
    }

    /**
     * Function to determine if a string is double-quoted
     */
    inline static bool isQuoted(std::string &string)
    {
        return isQuoted(string.begin(), string.end());
    }

    /**
     * Function to determine if a string is double-quoted
     */
    static bool isQuoted(std::string::iterator itBegin,
                         std::string::iterator itEnd)
    {
        if (std::regex_search(itBegin, itEnd, std::regex("\".*?\"")))
        {
            auto size = std::count_if(itBegin, itEnd, [] (auto &&ch) { return ch == '\"'; });
            if (size == 2)
                return true;
        }

        return false;
    }

    /**
     * Function to remove the outer quotes from a string (not including leading or trailing whitespace)
     */
    inline static std::string removeOuterQuotes(const std::string &string,
                                                const std::string &whitespace = " \t\r\n")
    {
        std::string unquotedString(string);
        removeOuterQuotes(&unquotedString);

        return unquotedString;
    }

    /**
     * Function to remove the outer quotes from a string (not including leading or trailing whitespace)
     */
    inline static bool removeOuterQuotes(std::string *pString,
                                         const std::string &whitespace = " \t\r\n")
    {
        bool bSuccess = (pString != nullptr && !pString->empty());
        if (bSuccess)
        {
            auto &&posQuote = pString->find_first_not_of(whitespace);
            if (posQuote != std::string::npos && (
                pString->at(posQuote) == '\"' || pString->at(posQuote) == '\''))
                pString->erase(pString->begin() + posQuote);

            posQuote = pString->find_last_not_of(whitespace);
            if (posQuote != std::string::npos && (
                pString->at(posQuote) == '\"' || pString->at(posQuote) == '\''))
                pString->erase(pString->begin() + posQuote);
        }

        return bSuccess;
    }

    /**
     * Function to convert words in a string to lower case
     */
    inline static std::string toLowerCase(const std::string &string)
    {
        std::string lowerCaseString(string);
        toLowerCase(&lowerCaseString);

        return lowerCaseString;
    }

    /**
     * Function to convert words in a string to lower case
     */
    inline static bool toLowerCase(std::string *pString)
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess)
            std::transform(pString->begin(), pString->end(), pString->begin(), ::tolower);

        return bSuccess;
    }

    /**
     * Function to convert words in a string to title case
     */
    inline static std::string toTitleCase(const std::string &string)
    {
        std::string titleCaseString(string);
        toTitleCase(&titleCaseString);

        return titleCaseString;
    }

    /**
     * Function to convert words in a string to title case
     */
    inline static bool toTitleCase(std::string *pString)
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess)
        {
            bool bCapitalizeNextLetter = true;
            for (std::size_t i = 0; i < pString->length(); ++i)
            {
                auto &ch = pString->at(i);
                if (bCapitalizeNextLetter && std::isalpha(ch))
                {
                    ch = ::toupper(ch);
                    bCapitalizeNextLetter = false;
                }
                else
                    bCapitalizeNextLetter = (std::isspace(ch) != 0 || std::ispunct(ch) != 0);
            }
        }

        return bSuccess;
    }

    /**
     * Function to convert words in a string to upper case
     */
    inline static std::string toUpperCase(const std::string &string)
    {
        std::string upperCaseString(string);
        toUpperCase(&upperCaseString);

        return upperCaseString;
    }

    /**
     * Function to convert words in a string to upper case
     */
    inline static bool toUpperCase(std::string *pString)
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess)
            std::transform(pString->begin(), pString->end(), pString->begin(), ::toupper);

        return bSuccess;
    }

    /**
     * Function to trim consecutive whitespace characters from a substring
     */
    inline static std::string trimConsecutiveWhitespace(const std::string &string)
    {
        std::string trimmed(string);
        trimConsecutiveWhitespace(&trimmed);

        return trimmed;
    }

    /**
     * Function to trim consecutive whitespace characters from a substring
     */
    inline static bool trimConsecutiveWhitespace(std::string *pString)
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess)
            trimConsecutiveWhitespace(pString, 0, pString->length());

        return bSuccess;
    }

    /**
     * Function to trim consecutive whitespace characters from a substring
     * @param pString a pointer to the full string
     * @param pos     the beginning position of the input substring
     * @param length  the size of the substring
     */
    inline static bool trimConsecutiveWhitespace(std::string *pString,
                                                 std::string::size_type pos,
                                                 std::string::size_type length)
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess)
        {
            size_t numConsecutiveWhitespace = 0;
            auto &&itString = pString->begin() + pos;
            auto &&itEnd = itString + length;
            while (itString != itEnd)
            {
                auto &ch = *itString;
                if (std::isspace(ch) != 0)
                {
                    if (numConsecutiveWhitespace++ > 0)
                    {
                        itString = pString->erase(itString);
                        --itEnd;
                        continue;
                    }
                }
                else
                    numConsecutiveWhitespace = 0;

                ++itString;
            }
        }

        return bSuccess;
    }

    /**
     * Function to trim leading whitespace from a string
     */
    inline static std::string trimLeadingWhitespace(const std::string &string,
                                                    const std::string &whitespace = " \t\r\n")
    {
        std::string trimmed(string);
        trimLeadingWhitespace(&trimmed, whitespace);

        return trimmed;
    }

    /**
     * Function to trim leading whitespace from a string
     */
    inline static bool trimLeadingWhitespace(std::string *pString, const std::string &whitespace = " \t\r\n")
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess && !pString->empty())
            pString->erase(0, pString->find_first_not_of(whitespace));

        return bSuccess;
    }

    /**
     * Function to trim leading and trailing whitespace from a string
     */
    inline static std::string trimLeadingAndTrailingWhitespace(const std::string &string,
                                                               const std::string &whitespace = " \t\r\n")
    {
        std::string trimmed(string);
        trimLeadingAndTrailingWhitespace(&trimmed, whitespace);

        return trimmed;
    }

    /**
     * Function to trim leading and trailing whitespace from a string
     */
    inline static bool trimLeadingAndTrailingWhitespace(std::string *pString,
                                                        const std::string &whitespace = " \t\r\n")
    {
        bool bSuccess = trimLeadingWhitespace(pString, whitespace);
        if (bSuccess)
            bSuccess = trimTrailingWhitespace(pString, whitespace);

        return bSuccess;
    }

    /**
     * Function to trim trailing whitespace from a string
     */
    inline static std::string trimTrailingWhitespace(const std::string &string,
                                                     const std::string &whitespace = " \t\r\n")
    {
        std::string trimmed(string);
        trimTrailingWhitespace(&trimmed, whitespace);

        return trimmed;
    }

    /**
     * Function to trim trailing whitespace from a string
     */
    inline static bool trimTrailingWhitespace(std::string *pString,
                                              const std::string &whitespace = " \t\r\n")
    {
        bool bSuccess = (pString != nullptr);
        if (bSuccess && !pString->empty())
        {
            auto &&end = pString->find_last_not_of(whitespace);
            if (end != std::string::npos)
            {
                if (1 + end < pString->length())
                    pString->erase(1 + end, std::string::npos);
            }
            else
                pString->clear(); // the whole string is composed of whitespace
        }

        return bSuccess;
    }
};

}

}

#endif
