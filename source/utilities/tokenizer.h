#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "iterable.h"
#include "token_iterator.h"
#include <algorithm>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <vector>

namespace utilities
{

/**
 * This class implements an iterable string tokenizer. Delimiters can be freely added or removed. The default
 * delimiter is a newline character. Valid delimiters include single characters or strings of characters.
 */
class Tokenizer final
: public attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::token_iterator_tag>
{
public:

    /**
     * Typedef declarations
     */
    typedef attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::token_iterator_tag>
                      ::const_iterator const_iterator;
    typedef attributes::abstract::Iterable<iterators::Iterator, std::string, iterators::token_iterator_tag>
                      ::iterator iterator;
    typedef std::function<bool (std::string &)> tStringPreprocessor;
    typedef std::vector<std::pair<std::string, tStringPreprocessor>> tStringPreprocessorMap;

    /**
     * Constructor
     */
    Tokenizer(void)
    : Tokenizer("\n")
    {

    }

    /**
     * Constructor
     */
    template<class ... Delimiters>
    Tokenizer(const std::string &delimiter, Delimiters ... delimiters)
    : Tokenizer(std::vector<std::string>({std::string(delimiter), static_cast<std::string>(delimiters) ...}))
    {

    }

    /**
     * Constructor which accepts a vector of delimiter strings
     */
    Tokenizer(const std::vector<std::string> &delimiters)
    : m_bEnableStringPreprocessing(true)
    {
        setDelimiters(delimiters);

        auto *pDefaultStringPreprocessorsMap = getDefaultStringPreprocessorsMap();
        if (pDefaultStringPreprocessorsMap != nullptr)
        {
            for (auto &&itNameStringPreprocessor : *pDefaultStringPreprocessorsMap)
                addStringPreprocessor(itNameStringPreprocessor.first, itNameStringPreprocessor.second);
        }

        // setup the default token functor
        m_tokenProcessor = [] (std::string &) { return 0; };
    }

    /**
     * Copy constructor
     */
    Tokenizer(const Tokenizer &tokenizer)
    {
        operator = (tokenizer);
    }

    /**
     * Move constructor
     */
    Tokenizer(Tokenizer &&tokenizer)
    : m_bEnableStringPreprocessing(false)
    {
        operator = (std::move(tokenizer));
    }

    /**
     * Copy assignment operator
     */
    Tokenizer &operator = (const Tokenizer &tokenizer)
    {
        if (&tokenizer != this)
        {
            m_bEnableStringPreprocessing = tokenizer.m_bEnableStringPreprocessing;
            m_delimiters = tokenizer.m_delimiters;
            m_string = tokenizer.m_string;
            m_stringPreprocessors = tokenizer.m_stringPreprocessors;
            m_tokenProcessor = tokenizer.m_tokenProcessor;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Tokenizer &operator = (Tokenizer &&tokenizer)
    {
        if (&tokenizer != this)
        {
            m_bEnableStringPreprocessing = std::move(tokenizer.m_bEnableStringPreprocessing);
            m_delimiters = std::move(tokenizer.m_delimiters);
            m_string = std::move(tokenizer.m_string);
            m_stringPreprocessors = std::move(tokenizer.m_stringPreprocessors);
            m_tokenProcessor = std::move(tokenizer.m_tokenProcessor);
        }

        return *this;
    }

    /**
     * Destructor
     */
    virtual ~Tokenizer(void) override
    {

    }

    /**
     * Add delimiters
     */
    template<class ... Delimiters>
    inline void addDelimiters(Delimiters ... delimiters)
    {
        addDelimiters({ std::string(delimiters) ...});
    }

    /**
     * Add delimiters
     */
    inline virtual void addDelimiters(const std::vector<std::string> &delimiters) final
    {
        // insert delimiters in order from longest to shortest
        for (auto &&delimiter : delimiters)
        {
            auto &&function = [&delimiter] (auto &&delim) { return delimiter.size() > delim.size(); };
            auto &&itDelim = std::find_if(m_delimiters.begin(), m_delimiters.end(), function);
            m_delimiters.insert(itDelim, delimiter);
        }
    }

    /**
     * Function to add a new string preprocessor; returns false if a string preprocessor with the given name
     * already exists
     */
    inline virtual bool addStringPreprocessor(const std::string &name,
                                              const tStringPreprocessor &stringPreprocessor) final
    {
        auto &&itNameStringPreprocessorPair = std::find_if(m_stringPreprocessors.cbegin(),
                                                           m_stringPreprocessors.cend(),
                                                           [&name] (auto &&pair)
                                                           { return pair.first == name; });
        bool bSuccess = (itNameStringPreprocessorPair == m_stringPreprocessors.end());
        if (bSuccess)
            setStringPreprocessor(name, stringPreprocessor);

        return bSuccess;
    }

    /**
     * begin() override
     */
    inline virtual iterator begin(void) override
    {
        iterator it(m_string, std::bind(&Tokenizer::findNextToken, this, std::placeholders::_1,
                                        std::placeholders::_2));

        return ++it;
    }

    /**
     * cbegin() override
     */
    inline virtual const_iterator cbegin(void) const override
    {
        return const_cast<Tokenizer *>(this)->begin();
    }

    /**
     * cend() override
     */
    inline virtual const_iterator cend(void) const override
    {
        return const_cast<Tokenizer *>(this)->end();
    }

    /**
     * Tests whether or not the object contains any delimiters
     */
    inline virtual bool empty(void) const final
    {
        return m_delimiters.empty();
    }

    /**
     * Function to enable/disable string pre-processing
     */
    inline virtual void enableStringPreprocessing(bool bEnabled) final
    {
        m_bEnableStringPreprocessing = bEnabled;
    }

    /**
     * Function to query status of string pre-processing enabled/disabled
     */
    inline virtual bool enableStringPreprocessing(void) const final
    {
        return m_bEnableStringPreprocessing;
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
     * Function to get the next token. Returns a value of 0 for valid tokens, a value greater than 0 when a
     * token should be ignored, or a value less than zero if parsing should terminate immediately.
     */
    virtual int findNextToken(const char *&pBuffer, std::string &token) const final
    {
        bool bConsecutiveDelimiter;
        int result = -1; // assume failure or end of string
        do
        {
            bConsecutiveDelimiter = false;
            size_t length = std::strlen(pBuffer);
            if (length > 0)
            {
                // find the first occurrence of a delimiter...
                const char *pEndOfToken = pBuffer + length, *pEndOfBuffer = pEndOfToken;
                size_t delimiterSize = 0;
                for (auto &&delimiter : m_delimiters)
                {
                    size_t size = delimiter.size();
                    if (size > 0)
                    {
                        auto *pDelimiter = delimiter.c_str();
                        pDelimiter = std::search(pBuffer, pBuffer + length, pDelimiter, pDelimiter + size);
                        if (pDelimiter == pEndOfToken && pEndOfToken != pEndOfBuffer)
                            delimiterSize = std::max(delimiterSize, size);
                        else if (pDelimiter < pEndOfToken)
                        {
                            pEndOfToken = pDelimiter;
                            delimiterSize = size;
                        }
                    }
                }

                std::string string(pBuffer, pEndOfToken);
                if (!string.empty())
                {
                    pBuffer = pEndOfToken;
                    result = m_tokenProcessor(string);
                    if (result == 0)
                        token = std::move(string);
                }
                else if (pEndOfToken)
                    bConsecutiveDelimiter = true;

                if (pEndOfToken)
                    pBuffer += delimiterSize;
            }
        }
        while (bConsecutiveDelimiter);

        return result;
    }

public:

    /**
     * Function to get the default global string preprocessors map
     */
    inline static tStringPreprocessorMap *&getDefaultStringPreprocessorsMap(void)
    {
        static tStringPreprocessorMap *pDefaultStringPreprocessors = nullptr;

        return pDefaultStringPreprocessors;
    }

    /**
     * Return a vector of delimiter strings
     */
    inline virtual std::set<std::string> &getDelimiters(void) final
    {
        return m_delimiters;
    }

    /**
     * Function to retrieve a string preprocessor by name
     */
    inline virtual bool getStringPreprocessor(const std::string &name,
                                              tStringPreprocessor &stringPreprocessor) final
    {
        auto &&itNameStringPreprocessorPair = std::find_if(m_stringPreprocessors.cbegin(),
                                                           m_stringPreprocessors.cend(),
                                                           [&name] (auto &&pair)
                                                           { return pair.first == name; });
        bool bSuccess = (itNameStringPreprocessorPair != m_stringPreprocessors.cend());
        if (bSuccess)
            stringPreprocessor = itNameStringPreprocessorPair->second;

        return bSuccess;
    }

    /**
     * Function to get this object's map of string preprocessors
     */
    inline virtual tStringPreprocessorMap &getStringPreprocessors(void) final
    {
        return m_stringPreprocessors;
    }

    /**
     * Initialization function
     * @param input an std::string or std::istream object
     */
    template<typename Input>
    inline auto initialize(Input &&input)
    {
        return initialize(std::forward<Input>(input), [] (std::string &) { return 0; });
    }

    /**
     * Initialization function
     * @param stream         an r-value reference to an std::ifstream object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename Functor>
    inline bool initialize(std::ifstream &&stream, Functor &&tokenProcessor)
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            bSuccess = initialize(stream, std::forward<Functor>(tokenProcessor));
            stream.close();
        }

        return bSuccess;
    }

    /**
     * Initialization function
     * @param stream         a reference to an std::istream object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename Functor>
    inline bool initialize(std::istream &stream, Functor &&tokenProcessor)
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            auto &&string = std::string((std::istreambuf_iterator<char>(stream)),
                                         std::istreambuf_iterator<char>());

            bSuccess = initialize(std::move(string), std::forward<Functor>(tokenProcessor));
        }

        return bSuccess;
    }

    /**
     * Initialization function
     * @param string         an std::string object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename Functor>
    inline bool initialize(const std::string &string, Functor &&tokenProcessor)
    {
        return initialize(std::string(string), std::forward<Functor>(tokenProcessor));
    }

    /**
     * Initialization function
     * @param string         an r-value std::string object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename Functor>
    inline bool initialize(std::string &&string, Functor &&tokenProcessor)
    {
        m_tokenProcessor = std::forward<Functor>(tokenProcessor);

        if (&string != &m_string)
            m_string = std::move(string);

        return true;
    }

    /**
     * Overload of the parse() method which converts a stream into a string and then calls the split() method
     * to tokenize the string.
     * @param stream an r-value reference to an std::ifstream object
     */
    template<typename T>
    inline std::vector<T> parse(std::ifstream &&stream)
    {
        std::vector<T> tokens;
        if (stream)
        {
            tokens = std::move(parse<T>(stream));
            stream.close();
        }

        return tokens;
    }

    /**
     * Overload of the parse() method which converts a stream into a string and then calls the split() method
     * to tokenize the string.
     * @param stream a reference to an std::istream object
     */
    template<typename T>
    inline std::vector<T> parse(std::istream &stream)
    {
        std::string string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

        return split<T>(std::move(string));
    }

    /**
     * Calls the split() method with the input string
     * @param string an std::string object
     */
    template<typename T>
    inline std::vector<T> parse(const std::string &string)
    {
        return split<T>(std::move(std::string(string)));
    }

    /**
     * Calls the split() method with the input string
     * @param string an r-value std::string object
     */
    template<typename T>
    inline std::vector<T> parse(std::string &&string)
    {
        return split<T>(std::move(string));
    }

    /**
     * This function converts the input stream into a string and then calls the split() method to tokenize the
     * string. The function also accepts a function object that can be used to compare or condition each token
     * using some specified criteria. The functor should return a value of 0 for valid tokens, a value greater
     * than 0 when a token should be ignored, or a value less than zero if parsing should terminate immediately
     * @param stream         an r-value reference to an std::ifstream object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename T, typename Functor>
    inline std::vector<T> parse(std::ifstream &&stream, Functor &&tokenProcessor)
    {
        std::vector<T> tokens;
        if (stream)
        {
            tokens = std::move(parse<T, Functor>(stream, std::forward<Functor>(tokenProcessor)));
            stream.close();
        }

        return tokens;
    }

    /**
     * This function converts the input stream into a string and then calls the split() method to tokenize the
     * string. The function also accepts a function object that can be used to compare or condition each token
     * using some specified criteria. The functor should return a value of 0 for valid tokens, a value greater
     * than 0 when a token should be ignored, or a value less than zero if parsing should terminate immediately
     * @param stream         a reference to an std::istream object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename T, typename Functor>
    inline std::vector<T> parse(std::istream &stream, Functor &&tokenProcessor)
    {
        std::string string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

        return split<T>(std::move(string), std::forward<Functor>(tokenProcessor));
    }

    /**
     * Calls the split() method with the input string. The function also accepts a function object that can be
     * used to compare or condition each token using some specified criteria. The functor should return a value
     * of 0 for valid tokens, a value greater than 0 when a token should be ignored, or a value less than zero
     * if parsing should terminate immediately
     * @param string         an std::string object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename T, typename Functor>
    inline std::vector<T> parse(const std::string &string, Functor &&tokenProcessor)
    {
        return split<T>(std::string(string), std::forward<Functor>(tokenProcessor));
    }

    /**
     * Calls the split() method with the input string. The function also accepts a function object that can be
     * used to compare or condition each token using some specified criteria. The functor should return a value
     * of 0 for valid tokens, a value greater than 0 when a token should be ignored, or a value less than zero
     * if parsing should terminate immediately.
     * @param string         an r-value std::string object
     * @param tokenProcessor a function object that is used to process each individual token (see the split
     *                       function for description)
     */
    template<typename T, typename Functor>
    inline std::vector<T> parse(std::string &&string, Functor &&tokenProcessor)
    {
        return split<T>(std::move(string), std::forward<Functor>(tokenProcessor));
    }

    /**
     * Forwarding function that accepts a vector of delimiters and forwards the remaining variadic argument
     * list to one of the other parse() function overloads
     */
    template<typename T, typename ... Args>
    inline static auto parse(const std::vector<std::string> &delimiters, Args && ... args)
    {
        Tokenizer tokenizer(delimiters);

        return tokenizer.parse<T>(std::forward<Args>(args) ...);
    }

    /**
     * Remove delimiters
     */
    template<class ... Delimiters>
    void removeDelimiters(Delimiters ... delimiters)
    {
        std::vector<std::string> array = { static_cast<std::string>(delimiters) ... };
        if (!array.empty())
        {
            for (auto &&delimiter : array)
            {
                auto &&itDelimiter = std::find(m_delimiters.begin(), m_delimiters.end(), delimiter);
                if (itDelimiter != m_delimiters.end())
                    m_delimiters.erase(itDelimiter);
            }
        }
        else
            m_delimiters.clear();
    }

    /**
     * Function to remove a string preprocessor by name; returns true upon success
     */
    inline virtual bool removeStringPreprocessor(const std::string &name) final
    {
        auto &&itNameStringPreprocessorPair = std::find_if(m_stringPreprocessors.begin(),
                                                           m_stringPreprocessors.end(),
                                                           [&name] (auto &&pair)
                                                           { return pair.first == name; });
        bool bSuccess = (itNameStringPreprocessorPair != m_stringPreprocessors.end());
        if (bSuccess)
            m_stringPreprocessors.erase(itNameStringPreprocessorPair);

        return bSuccess;
    }

    /**
     * Function to remove this object's string preprocessors
     */
    inline virtual void removeStringPreprocessors(void) final
    {
        m_stringPreprocessors.clear();
    }

    /**
     * Function to set the default global string preprocessors map
     */
    inline static bool setDefaultStringPreprocessorsMap(tStringPreprocessorMap *pStringPreprocessorsMap)
    {
        bool bSuccess = (pStringPreprocessorsMap != nullptr);
        if (bSuccess)
        {
            auto *&pDefaultStringPreprocessors = getDefaultStringPreprocessorsMap();

            pDefaultStringPreprocessors = pStringPreprocessorsMap;
        }

        return bSuccess;
    }

    /**
     * Function to set this object's delimiters
     */
    template<class ... Delimiters>
    inline void setDelimiters(const std::string &delimiter, Delimiters ... delimiters)
    {
        setDelimiters(std::vector<std::string>({ std::string(delimiter),
                                                 static_cast<std::string>(delimiters) ... }));
    }

    /**
     * Function to set this object's delimiters
     */
    inline virtual void setDelimiters(const std::vector<std::string> &delimiters) final
    {
        m_delimiters.clear();

        addDelimiters(delimiters);
    }

    /**
     * Function to set a string preprocessor; overwrites an existing string preprocessor if an entry with the
     * given name already exists
     */
    virtual void setStringPreprocessor(const std::string &name,
                                       const tStringPreprocessor &stringPreprocessor) final
    {
        auto &&itNameStringPreprocessorPair = std::find_if(m_stringPreprocessors.begin(),
                                                           m_stringPreprocessors.end(),
                                                           [&name] (auto &&pair)
                                                           { return pair.first == name; });
        if (itNameStringPreprocessorPair == m_stringPreprocessors.end())
            m_stringPreprocessors.emplace_back(std::make_pair(name, stringPreprocessor));
        else
            itNameStringPreprocessorPair->second = stringPreprocessor;
    }

    /**
     * Function to set this object's map of string preprocessors
     */
    inline virtual void setStringPreprocessors(const tStringPreprocessorMap &stringPreprocessors) final
    {
        m_stringPreprocessors = stringPreprocessors;
    }

    /**
     * Return the number of delimiters in the object
     */
    inline virtual size_t size(void) const final
    {
        return m_delimiters.size();
    }

private:

    /**
     * This is the primary tokenization method that converts string input into tokens. The function tokenizes
     * the input string in the order in which delimiters are encountered and returns the tokens in a vector. If
     * more than one delimiter is encountered at the same position in the input string, the largest delimiter
     * is used to tokenize the string.
     * @param string an r-value std::string object
     */
    template<typename T>
    inline std::vector<T> split(std::string &&string)
    {
        return split<T>(std::move(string), [] (std::string &) { return 0; });
    }

    /**
     * This is the primary tokenization method that converts string input into tokens. The function tokenizes
     * the input string in the order in which delimiters are encountered and returns the tokens in a vector. If
     * more than one delimiter is encountered at the same position in the input string, the largest delimiter
     * is used to tokenize the string. Only tokens of type T are stored. The function also accepts a function
     * object that can be used to compare or condition each token using some specified criteria. The functor
     * should return a value of 0 for valid tokens, a value greater than 0 when a token should be ignored, or a
     * value less than zero if parsing should terminate immediately.
     * @param string         an r-value std::string object
     * @param tokenProcessor a function object that is used to process each individual token
     */
    template<typename T, typename Functor>
    std::vector<T> split(std::string &&string, Functor &&tokenProcessor)
    {
        initialize(std::move(string), std::forward<Functor>(tokenProcessor));

        // preprocess the string prior to tokenization
        if (m_bEnableStringPreprocessing)
        {
            auto &&stringPreprocessors = getStringPreprocessors();
            for (auto &&itNameStringPreprocessorPair : stringPreprocessors)
            {
                auto &&stringPreprocessor = itNameStringPreprocessorPair.second;
                if (stringPreprocessor)
                    stringPreprocessor(m_string);
            }
        }

        std::vector<T> tokens;
        split(tokens);

        return tokens;
    }

    /**
     * This is the primary tokenization method that converts string input into tokens. The function tokenizes
     * the input string in the order in which delimiters are encountered and returns the tokens in a vector. If
     * more than one delimiter is encountered at the same position in the input string, the largest delimiter
     * is used to tokenize the string.
     * @param tokens upon success, will be populated with the tokens
     */
    template<typename T>
    typename std::enable_if<std::is_assignable<T, std::string>::value, void>::type
    split(std::vector<T> &tokens)
    {
        std::istringstream stream;
        for (auto &&itToken = begin(); itToken != end(); ++itToken)
        {
            if (!itToken->empty())
            {
                tokens.push_back(*itToken);
                stream.clear();
            }
        }
    }

    /**
     * This is the primary tokenization method that converts string input into tokens. The function tokenizes
     * the input string in the order in which delimiters are encountered and returns the tokens in a vector. If
     * more than one delimiter is encountered at the same position in the input string, the largest delimiter
     * is used to tokenize the string.
     * @param tokens upon success, will be populated with tokens converted to type T
     */
    template<typename T>
    typename std::enable_if<!std::is_assignable<T, std::string>::value, void>::type
    split(std::vector<T> &tokens)
    {
        T value;
        std::istringstream stream;
        for (auto &&itToken = begin(); itToken != end(); ++itToken)
        {
            if (!itToken->empty())
            {
                stream.str(*itToken);
                stream >> value;
                if (!stream.fail())
                    tokens.push_back(value);

                stream.clear();
            }
        }
    }

    /**
     * flag to enable/disable string pre-processing
     */
    bool m_bEnableStringPreprocessing;

    /**
     * vector of delimiter tokens
     */
    std::set<std::string> m_delimiters;

    /**
     * the string to be tokenized
     */
    std::string m_string;

    /**
     * map of string preprocessors
     */
    tStringPreprocessorMap m_stringPreprocessors;

    /**
     * function object that defines tokenization criteria
     */
    std::function<int (std::string &)> m_tokenProcessor;
};

}

#endif
