#ifndef CSV_TOKENIZER_H
#define CSV_TOKENIZER_H

#include "iterable.h"
#include "string_utilities.h"
#include "token_iterator.h"
#include <fstream>
#include <sstream>
#include <vector>

namespace utilities
{

/**
 * This class implements an iterable csv-formatted string tokenizer
 */
class CsvTokenizer
: public attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::token_iterator_tag>
{
private:

    /**
     * Enumerations
     */
    enum class Status
    {
        UnquotedField, // , ... ,    (token with no quotation marks)
        QuotedField, // , ".,.,." ,  (token enclosed by quotation marks, possibly including commas
        QuotedQuote // , "...""...", (token containing consecutive quoted strings
    };

public:

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::token_iterator_tag>
                      ::const_iterator const_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::token_iterator_tag>
                      ::iterator iterator;

    /**
     * Constructor
     * @param bRemoveEmptyTokens      flag indicating whether or not empty tokens will be removed
     * @param bTrimLeadingWhitespace  flag indicating whether or not to remove leading whitespace from tokens
     * @param bTrimTrailingWhitespace flag indicating removal or not to remove trailing whitespace from tokens
     */
    CsvTokenizer(bool bRemoveEmptyTokens = true,
                 bool bTrimLeadingWhitespace = false,
                 bool bTrimTrailingWhitespace = false)
    : m_bRemoveEmptyTokens(bRemoveEmptyTokens),
      m_bTrimLeadingWhitespace(bTrimLeadingWhitespace),
      m_bTrimTrailingWhitespace(bTrimTrailingWhitespace)
    {

    }

    /**
     * Copy constructor
     */
    CsvTokenizer(const CsvTokenizer &tokenizer)
    {
        operator = (tokenizer);
    }

    /**
     * Move constructor
     */
    CsvTokenizer(CsvTokenizer &&tokenizer)
    {
        operator = (std::move(tokenizer));
    }

    /**
     * Destructor
     */
    virtual ~CsvTokenizer(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    CsvTokenizer &operator = (const CsvTokenizer &tokenizer)
    {
        if (&tokenizer != this)
        {
            m_bRemoveEmptyTokens = tokenizer.m_bRemoveEmptyTokens;
            m_bTrimLeadingWhitespace = tokenizer.m_bTrimLeadingWhitespace;
            m_bTrimTrailingWhitespace = tokenizer.m_bTrimTrailingWhitespace;
            m_string = tokenizer.m_string;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    CsvTokenizer &operator = (CsvTokenizer &&tokenizer)
    {
        if (&tokenizer != this)
        {
            m_bRemoveEmptyTokens = std::move(tokenizer.m_bRemoveEmptyTokens);
            m_bTrimLeadingWhitespace = std::move(tokenizer.m_bTrimLeadingWhitespace);
            m_bTrimTrailingWhitespace = std::move(tokenizer.m_bTrimTrailingWhitespace);
            m_string = std::move(tokenizer.m_string);
        }

        return *this;
    }

    /**
     * begin() override
     */
    inline virtual iterator begin(void) override
    {
        iterator it(m_string, std::bind(&CsvTokenizer::findNextToken, this, std::placeholders::_1,
                                        std::placeholders::_2));

        return ++it;
    }

    /**
     * cbegin() override
     */
    inline virtual const_iterator cbegin(void) const override
    {
        return const_cast<CsvTokenizer *>(this)->begin();
    }

    /**
     * cend() override
     */
    inline virtual const_iterator cend(void) const override
    {
        return const_cast<CsvTokenizer *>(this)->end();
    }

    /**
     * end() override
     */
    inline virtual iterator end(void) override
    {
        return iterator(m_string);
    }

private:

    /**
     * Function to get the next token from the input; returns zero if a token was successfully extracted
     */
    virtual int findNextToken(const char *&pBuffer, std::string &token) const final
    {
        int result = -1;
        Status status = Status::UnquotedField;
        if (pBuffer != nullptr && *pBuffer != 0)
        {
            token.clear();
            do
            {
                auto &&ch = *pBuffer;
                switch (status)
                {
                    case Status::UnquotedField: // end of field
                    switch (ch)
                    {
                        case '\n':
                        case '\0':
                        if (!token.empty())
                            result = 0;

                        break;

                        case ',': // end of field
                        if (!token.empty() || !m_bRemoveEmptyTokens)
                            result = 0;

                        break;

                        case '"':
                        status = Status::QuotedField;
                        break;

                        default:
                        token.push_back(ch);
                        break;
                    }

                    break;

                    case Status::QuotedField:
                    switch (ch)
                    {
                        case '"':
                        status = Status::QuotedQuote;
                        break;

                        default:
                        token.push_back(ch);
                        break;
                    }

                    break;

                    case Status::QuotedQuote:
                    switch (ch)
                    {
                        case '\0':
                        case '\n':
                        if (!token.empty())
                            result = 0;

                        break;

                        case ',': // , after closing quote
                        status = Status::UnquotedField;
                        if (!token.empty() || !m_bRemoveEmptyTokens)
                            result = 0;

                        break;

                        case '"': // "" -> "
                        token.push_back('"');
                        status = Status::QuotedField;
                        break;

                        default: // end of quote
                        status = Status::UnquotedField;
                        break;
                    }

                    break;
                }

                if (ch == '\0')
                    break;

                ++pBuffer;
            }
            while (result != 0);
        }

        if (result == 0)
        {
            if (m_bTrimLeadingWhitespace)
                string::StringUtilities::trimLeadingWhitespace(&token);

            if (m_bTrimTrailingWhitespace)
                string::StringUtilities::trimTrailingWhitespace(&token);
        }

        return result;
    }

public:

    /**
     * Initialization function
     * @param stream an r-value reference to an std::ifstream object
     */
    inline virtual bool initialize(std::ifstream &&stream) final
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            bSuccess = initialize(stream);
            stream.close();
        }

        return bSuccess;
    }

    /**
     * Initialization function
     * @param stream a reference to an std::istream object
     */
    inline virtual bool initialize(std::istream &stream) final
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            auto &&string = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

            bSuccess = initialize(std::move(string));
        }

        return bSuccess;
    }

    /**
     * Initialization function
     * @param string an std::string object
     */
    inline virtual bool initialize(const std::string &string) final
    {
        return initialize(std::string(string));
    }

    /**
     * Initialization function
     * @param string an r-value std::string object
     */
    inline virtual bool initialize(std::string &&string) final
    {
        if (&string != &m_string)
            m_string = std::move(string);

        return true;
    }

    /**
     * Overload of the parseLine() method which converts a stream into a string and then calls the split()
     * method to tokenize a line from the resultant string
     * @param stream an r-value reference to an std::ifstream object
     */
    inline virtual std::vector<std::string> parseLine(std::ifstream &&stream) final
    {
        std::vector<std::string> tokens;
        if (stream)
        {
            tokens = parseLine(stream);
            stream.close();
        }

        return tokens;
    }

    /**
     * Overload of the parseLine() method which converts a stream into a string and then calls the split()
     * method to tokenize a line from the resultant string
     * @param stream a reference to an std::istream object
     */
    inline virtual std::vector<std::string> parseLine(std::istream &stream) final
    {
        std::string buffer;
        std::getline(stream, buffer, '\n');

        return parseLine(std::move(buffer));
    }

    /**
     * Overload of the parseLine() method which calls the split() method to tokenize a line from the input
     * string
     * @param string an std::string object
     */
    inline virtual std::vector<std::string> parseLine(const std::string &string) final
    {
        return split(std::string(string));
    }

    /**
     * Overload of the parseLine() method which calls the split() method to tokenize a line from the input
     * string
     * @param string an r-value std::string object
     */
    inline virtual std::vector<std::string> parseLine(std::string &&string) final
    {
        return split(std::move(string));
    }

    /**
     * Overload of the parseTable() method which converts a stream into a string and then calls the split()
     * method to tokenize the resultant string into vectors of row tokens
     * @param stream an r-value reference to an std::ifstream object
     */
    inline virtual std::vector<std::vector<std::string>> parseTable(std::ifstream &&stream) final
    {
        std::vector<std::vector<std::string>> tokensTable;
        if (stream)
        {
            tokensTable = parseTable(stream);
            stream.close();
        }

        return tokensTable;
    }

    /**
     * Overload of the parseTable() method which converts a stream into a string and then calls the split()
     * method to tokenize the resultant string into vectors of row tokens
     * @param stream a reference to an std::istream object
     */
    inline virtual std::vector<std::vector<std::string>> parseTable(std::istream &stream) final
    {
        std::string buffer;
        std::vector<std::vector<std::string>> tokensTable;
        while (std::getline(stream, buffer, '\n'))
        {
            auto &&tokens = split(std::move(buffer));
            if (!tokens.empty())
                tokensTable.emplace_back(std::move(tokens));
        }

        return tokensTable;
    }

    /**
     * Overload of the parseTable() method which calls the split() method to tokenize the input string into
     * vectors of row tokens
     * @param string an std::string object
     */
    inline virtual std::vector<std::vector<std::string>> parseTable(const std::string &string) final
    {
        return parseTable(std::string(string));
    }

    /**
     * Overload of the parseTable() method which calls the split() method to tokenize the input string into
     * vectors of row tokens
     * @param string an r-value std::string object
     */
    inline virtual std::vector<std::vector<std::string>> parseTable(std::string &&string) final
    {
        std::istringstream stream(std::move(string));

        return parseTable(stream);
    }

    /**
     * Query whether or not empty tokens will be removed
     */
    inline virtual bool removeEmptyTokens(void) const final
    {
        return m_bRemoveEmptyTokens;
    }

    /**
     * Set flag to enable/disable the removal of empty tokens
     */
    inline virtual void removeEmptyTokens(bool bEnabled) final
    {
        m_bRemoveEmptyTokens = bEnabled;
    }

private:

    /**
     * This is the primary tokenization method that converts string input into tokens.
     * @param string an r-value std::string object
     */
    inline virtual std::vector<std::string> split(std::string &&string) final
    {
        initialize(std::move(string));

        std::vector<std::string> tokens;
        for (auto &&itToken = begin(); itToken != end(); ++itToken)
        {
            if (!itToken->empty() || !m_bRemoveEmptyTokens)
                tokens.push_back(*itToken);
        }

        return tokens;
    }

public:

    /**
     * Query whether or not both leading and trailing whitespace will be trimmed from tokens
     */
    inline virtual bool trimLeadingAndTrailingWhitespaceFromTokens(void) const final
    {
        return m_bTrimLeadingWhitespace && m_bTrimTrailingWhitespace;
    }

    /**
     * Set flag to enable/disable the removal of both leading and trailing whitespace from tokens
     */
    inline virtual void trimLeadingAndTrailingWhitespaceFromTokens(bool bEnabled) final
    {
        m_bTrimLeadingWhitespace = bEnabled;
        m_bTrimTrailingWhitespace = bEnabled;
    }

    /**
     * Query whether or not leading whitespace will be trimmed from tokens
     */
    inline virtual bool trimLeadingWhitespaceFromTokens(void) const final
    {
        return m_bTrimLeadingWhitespace;
    }

    /**
     * Set flag to enable/disable the removal of leading whitespace from tokens
     */
    inline virtual void trimLeadingWhitespaceFromTokens(bool bEnabled) final
    {
        m_bTrimLeadingWhitespace = bEnabled;
    }

    /**
     * Query whether or not trailing whitespace will be trimmed from tokens
     */
    inline virtual bool trimTrailingWhitespaceFromTokens(void) const final
    {
        return m_bTrimTrailingWhitespace;
    }

    /**
     * Set flag to enable/disable the removal of trailing whitespace from tokens
     */
    inline virtual void trimTrailingWhitespaceFromTokens(bool bEnabled) final
    {
        m_bTrimTrailingWhitespace = bEnabled;
    }

private:

    /**
     * flag to indicate whether or not empty tokens will be removed
     */
    bool m_bRemoveEmptyTokens;

    /**
     * flag to indicate removal of leading whitespace from tokens
     */
    bool m_bTrimLeadingWhitespace;

    /**
     * flag to indicate removal of trailing whitespace from tokens
     */
    bool m_bTrimTrailingWhitespace;

    /**
     * the string to be tokenized
     */
    std::string m_string;
};

}

#endif
