#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "is_pair_associative_container.h"
#include "string_utilities.h"
#include "tokenizer.h"
#include "variable_registry.h"
#include <fstream>
#include <regex>
#include <set>

namespace utilities
{

/**
 * This class can be used to create a variable dictionary from an input stream or string
 */
class Dictionary final
{
public:

    /**
     * Type alias declarations
     */
    using StringUtilities = utilities::string::StringUtilities;

    /**
     * Typedef declarations
     */
    typedef Tokenizer::tStringPreprocessor tStringPreprocessor;
    typedef Tokenizer::tStringPreprocessorMap tStringPreprocessorMap;
    typedef std::pair<std::string, std::string> tTokenPair;
    typedef std::vector<tTokenPair> tTokenPairs;

    /**
     * Constructor
     */
    Dictionary(void)
    : m_bIgnoreUndefinedVariables(true),
      m_pRegistry(nullptr),
      m_variableValueRegex("^\\s*(.+?)\\s*=\\s*(.+?)\\s*$")
    {

    }

    /**
     * Constructor
     */
    Dictionary(VariableRegistry *pRegistry)
    : m_bIgnoreUndefinedVariables(true),
      m_pRegistry(pRegistry),
      m_variableValueRegex("^\\s*(.+?)\\s*=\\s*(.+?)\\s*$")
    {

    }

    /**
     * Copy constructor
     */
    Dictionary(const Dictionary &dictionary)
    : m_pRegistry(nullptr)
    {
        operator = (dictionary);
    }

    /**
     * Move constructor
     */
    Dictionary(Dictionary &&dictionary)
    : m_pRegistry(nullptr)
    {
        operator = (std::move(dictionary));
    }

    /**
     * Destructor
     */
    virtual ~Dictionary(void)
    {

    }

    /**
     * Copy assignment operator
     */
    Dictionary &operator = (const Dictionary &dictionary)
    {
        if (&dictionary != this)
        {
            m_bIgnoreUndefinedVariables = dictionary.m_bIgnoreUndefinedVariables;
            m_lineTokenizer = dictionary.m_lineTokenizer;
            m_pRegistry = dictionary.m_pRegistry;
            m_variableValueRegex = dictionary.m_variableValueRegex;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Dictionary &operator = (Dictionary &&dictionary)
    {
        if (&dictionary != this)
        {
            m_bIgnoreUndefinedVariables = std::move(dictionary.m_bIgnoreUndefinedVariables);
            m_lineTokenizer = std::move(dictionary.m_lineTokenizer);
            m_pRegistry = std::move(dictionary.m_pRegistry);
            m_variableValueRegex = dictionary.m_variableValueRegex;

            dictionary.m_pRegistry = nullptr;
        }

        return *this;
    }

    /**
     * Add a variable to the registry associated with this dictionary
     */
    template<typename T>
    inline bool add(const std::string &name, T &variable)
    {
        bool bSuccess = (m_pRegistry != nullptr);
        if (bSuccess)
            m_pRegistry->add(name, variable);

        return bSuccess;
    }

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param criteria  a regular expression that will be matched to variable declarations within the input
     *                  string
     * @param container an stl-style container
     * @param input     the input string
     */
    template<typename Container>
    inline bool add(const std::regex &criteria,
                    Container &&container,
                    const std::string &input)
    {
        std::istringstream stream(input);

        return add(criteria, container, stream);
    }

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param criteria  a regular expression that will be matched to variable declarations within the input
     *                  stream
     * @param container an stl-style container
     * @param stream    an r-value reference to an std::ifstream object
     */
    template<typename Container>
    bool add(const std::regex &criteria,
             Container &&container,
             std::ifstream &&stream)
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            bSuccess = add(criteria, container, stream);
            stream.close();
        }

        return bSuccess;
    }

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param criteria  a regular expression that will be matched to variable declarations within the input
     *                  stream
     * @param container an stl-style container
     * @param stream    a reference to an std::istream object
     */
    template<typename Container>
    bool add(const std::regex &criteria,
             Container &&container,
             std::istream &stream)
    {
        // record the current stream position and flags
        auto &&flags = stream.flags();
        auto &&streamPos = stream.tellg();

        // get variable names from the input stream
        std::set<std::string> names;
        getVariableNamesFromInput(stream, names);

        // first determine how many variables match the regular expression
        std::vector<std::smatch> matches;
        for (auto &&name : names)
        {
            std::smatch match;
            if (std::regex_match(name, match, criteria))
                matches.push_back(match);
        }

        bool bSuccess = add(matches, std::forward<Container>(container));

        stream.seekg(streamPos);
        stream.setf(flags);

        return bSuccess;
    }

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param criteria   a regular expression that will be matched to variable declarations within the lines
     *                   tokens
     * @param container  an stl-style container
     * @param lineTokens a vector of tokenized strings containing variable-value expressions
     */
    template<typename Container>
    bool add(const std::regex &criteria,
             Container &&container,
             const std::vector<std::string> &lineTokens) const
    {
        // get variable names from the input stream
        std::set<std::string> names;
        getVariableNamesFromInput(lineTokens, names);

        // first determine how many variables match the regular expression
        std::vector<std::smatch> matches;
        for (auto &&name : names)
        {
            std::smatch match;
            if (std::regex_match(name, match, criteria))
                matches.push_back(match);
        }

        return add(matches, std::forward<Container>(container));
    }

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param criteria  a regular expression that will be matched to variable declarations within the token
     *                  map
     * @param container an stl-style container
     * @param tokenMap  a map of variable-value token pairs
     */
    template<typename Container>
    bool add(const std::regex &criteria,
             Container &&container,
             const VariableRegistry::tTokenMap &tokenMap)
    {
        // first determine how many variables match the regular expression
        std::vector<std::smatch> matches;
        for (auto &&itTokenPair : tokenMap)
        {
            auto &&name = itTokenPair.first;
            std::smatch match;
            if (std::regex_match(name, match, criteria))
                matches.push_back(match);
        }

        return add(matches, std::forward<Container>(container));
    }

    /**
     * Add a set of variable(s) to the registry associated with this dictionary
     * @param names     a set of variable names
     * @param container an stl-style pair associative container
     */
    template<typename Container, typename U = typename std::decay<Container>::type,
             typename std::enable_if<traits::tests::is_pair_associative_container<U>::value, int>::type = 0>
    bool add(const std::set<std::string> &names,
             Container &container)
    {
        typedef typename U::mapped_type mapped_type;
        for (auto &&name : names)
        {
            auto &&itNameValuePair = container.emplace(name, mapped_type());
            add(name, itNameValuePair.second);
        }

        return true;
    }

    /**
     * Add a set of variable(s) to the registry associated with this dictionary
     * @param names     a set of variable names
     * @param container an stl-style resizeable sequence container
     */
    template<typename Container, typename U = typename std::decay<Container>::type,
             typename std::enable_if<traits::tests::is_resizeable_sequence_container<U>::value &&
                                     !std::is_same<U, std::string>::value, int>::type = 0>
    bool add(const std::set<std::string> &names,
             Container &container)
    {
        container.resize(names.size());
        auto &&itContainer = container.cbegin();
        for (auto &&name : names)
            add(name, *itContainer++);

        return true;
    }

private:

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param matches   a vector regex_match objects
     * @param container an stl-style pair associative container
     */
    template<typename Container, typename U = typename std::decay<Container>::type,
             typename std::enable_if<traits::tests::is_pair_associative_container<U>::value, int>::type = 0>
    bool add(const std::vector<std::smatch> &matches,
             Container &container)
    {
        typedef typename U::key_type key_type;
        typedef typename U::mapped_type mapped_type;

        std::istringstream stream;
        bool bSuccess = false;
        for (auto &match : matches)
        {
            if (match.size() == 2)
            {
                auto &&name = match[0].str();
                key_type key(match[1].str());
                mapped_type value;
                auto &&itNameValuePair = container.emplace(key, value);
                bSuccess = add(name, itNameValuePair.first->second);
            }
        }

        return bSuccess;
    }

    /**
     * Add variable(s) defined via regular expression to the registry associated with this dictionary
     * @param matches   a vector regex_match objects
     * @param container an stl-style resizeable sequence container
     */
    template<typename Container, typename U = typename std::decay<Container>::type,
             typename std::enable_if<traits::tests::is_resizeable_sequence_container<U>::value &&
                                     !std::is_same<U, std::string>::value, int>::type = 0>
    bool add(const std::vector<std::smatch> &matches,
             Container &container)
    {
        bool bSuccess = true;
        std::vector<int> indices;

        // loop through the matches and find the subscript index
        int index = 0, size = 0;
        for (auto &&match : matches)
        {
            std::ssub_match subscript = match[1];
            std::istringstream stream(subscript.str());
            bSuccess &= !(stream >> index).fail();
            if (bSuccess)
            {
                indices.push_back(index);
                size = std::max(size, index);
            }
        }

        // if no errors occurred, resize the container and store the variables in the registry
        if (bSuccess && !matches.empty())
        {
            auto &&itIndex = indices.begin();
            container.resize(size + 1);
            for (auto &&match : matches)
            {
                auto &&name = match[0].str();
                auto &&value = *std::next(container.begin(), *itIndex++);
                add(name, value);
            }
        }

        return bSuccess;
    }

private:

    /**
     * Function to add a variable name to the input container (specialized for std::vector containers)
     */
    template<typename Container>
    inline static typename std::enable_if<std::is_same<typename std::decay<Container>::type,
                                          std::vector<std::string>>::value, void>::type
    addVariableNameToContainer(const std::string &variable,
                               Container &container)
    {
        if (std::find(container.cbegin(), container.cend(), variable) == container.cend())
            container.push_back(variable);
    }

    /**
     * Function to add a variable name to the input container (specialized for std::set containers)
     */
    template<typename Container>
    inline static typename std::enable_if<std::is_same<typename std::decay<Container>::type,
                                          std::set<std::string>>::value, void>::type
    addVariableNameToContainer(const std::string &variable,
                               Container &container)
    {
        container.insert(variable);
    }

public:

    /**
     * Function to create token pairs from the registry associated with this dictionary
     * @param input the input string
     */
    virtual VariableRegistry::tTokenMap createTokenPairs(void) const final
    {
        VariableRegistry::tTokenMap tokenMap;
        if (m_pRegistry != nullptr)
            tokenMap = m_pRegistry->createTokenPairs();

        return tokenMap;
    }

    /**
     * Function to create token pairs from the input string
     * @param input the input string
     */
    template<typename Container>
    inline Container createTokenPairs(const std::string &input) const
    {
        std::istringstream stream(input);

        return createTokenPairs<Container>(stream, { "\n", "\r" });
    }

    /**
     * Function to create token pairs from the input string
     * @param input          the input string
     * @param lineDelimiters a vector of strings that is used to split the input string into individual
     *                       lines
     */
    template<typename Container>
    inline Container createTokenPairs(const std::string &input,
                                      const std::vector<std::string> &lineDelimiters) const
    {
        std::istringstream stream(input);

        return createTokenPairs<Container>(stream, lineDelimiters);
    }

    /**
     * Function to create token pairs from the input stream
     * @param stream an r-value reference to an std::ifstream object
     */
    template<typename Container>
    Container createTokenPairs(std::ifstream &&stream) const
    {
        VariableRegistry::tTokenMap tokenMap;
        if (stream)
        {
            tokenMap = std::move(createTokenPairs<Container>(stream));
            stream.close();
        }

        return tokenMap;
    }

    /**
     * Function to create token pairs from the input stream
     * @param stream a reference to an std::istream object
     */
    template<typename Container>
    inline Container createTokenPairs(std::istream &stream) const
    {
        return createTokenPairs<Container>(stream, { "\n", "\r" });
    }

    /**
     * Function to create token pairs from the input stream
     * @param stream         a reference to an std::istream object
     * @param lineDelimiters a vector of strings that is used to split the input stream into individual
     *                       lines
     */
    template<typename Container>
    inline Container createTokenPairs(std::istream &stream,
                                      const std::vector<std::string> &lineDelimiters) const
    {
        Container container;

        createTokenPairs(stream, lineDelimiters, container);

        return container;
    }

private:

    /**
     * Function to create token pairs from the input stream
     * @param stream         a reference to an std::istream object
     * @param lineDelimiters a vector of strings that is used to split the input stream into individual
     *                       lines
     * @param tokenMap       a map of variable-value token pairs
     */
    inline virtual void createTokenPairs(std::istream &stream,
                                         const std::vector<std::string> &lineDelimiters,
                                         VariableRegistry::tTokenMap &tokenMap) const final
    {
        auto is_space = [] (const std::string &str) { return std::all_of(str.cbegin(), str.cend(), isspace); };
        m_lineTokenizer.setDelimiters(lineDelimiters);
        auto &&lineTokens = m_lineTokenizer.parse<std::string>(stream, is_space);
        for (auto &&line : lineTokens)
        {
            std::smatch match;
            if (std::regex_search(line, match, std::regex(m_variableValueRegex)) && match.size() == 3)
            {
                auto &&variable = match[1].str();
                auto &&value = match[2].str();

                tokenMap.emplace(std::move(variable), std::move(value));
            }
        }
    }

    /**
     * Function to create token pairs from the input stream
     * @param stream         a reference to an std::istream object
     * @param lineDelimiters a vector of strings that is used to split the input stream into individual
     *                       lines
     * @param tokenPairs     a vector of token pairs
     */
    virtual void createTokenPairs(std::istream &stream,
                                  const std::vector<std::string> &lineDelimiters,
                                  tTokenPairs &tokenPairs) const final
    {
        auto is_space = [] (const std::string &str) { return std::all_of(str.cbegin(), str.cend(), isspace); };
        m_lineTokenizer.setDelimiters(lineDelimiters);
        auto &&lineTokens = m_lineTokenizer.parse<std::string>(stream, is_space);
        for (auto &&line : lineTokens)
        {
            std::smatch match;
            if (std::regex_search(line, match, std::regex(m_variableValueRegex)) && match.size() == 3)
            {
                auto &&variable = match[1].str();
                auto &&value = match[2].str();

                tokenPairs.emplace_back(std::make_pair(std::move(variable), std::move(value)));
            }
        }
    }

public:

    /**
     * Get a reference to this object's line tokenizer
     */
    inline virtual Tokenizer &getLineTokenizer(void) final
    {
        return m_lineTokenizer;
    }

    /**
     * Get a list of variables from the input string
     * @param input     the input string
     * @param variables an stl-style container to hold the list of variables
     */
    template<typename Container, typename regex>
    inline typename std::enable_if<std::is_same<typename std::decay<regex>::type, std::regex>::value, bool>::type
    getVariableNamesFromInput(const std::string &input,
                              Container &variables) const
    {
        std::istringstream stream(input);

        return getVariableNamesFromInput(stream, variables, m_variableValueRegex);
    }

    /**
     * Get a list of variables from the stream
     * @param stream    an r-value reference to an std::ifstream object
     * @param variables an stl-style container to hold the list of variables
     */
    template<typename Container>
    bool getVariableNamesFromInput(std::ifstream &&stream,
                                   Container &variables) const
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            bSuccess = getVariableNamesFromInput(stream, variables, m_variableValueRegex);
            stream.close();
        }

        return bSuccess;
    }

    /**
     * Get a list of variables from the stream
     * @param stream    a reference to an std::istream object
     * @param variables an stl-style container to hold the list of variables
     */
    template<typename Container>
    inline void getVariableNamesFromInput(std::istream &stream,
                                          Container &variables) const
    {
        getVariableNamesFromInput(stream, variables, { "\n", "\r" });
    }

    /**
     * Get a list of variables from the input string
     * @param input          the input string
     * @param variables      an stl-style container to hold the list of variables
     * @param lineDelimiters a vector of strings that is used to split the input string into individual
     *                       lines
     */
    template<typename Container>
    inline bool getVariableNamesFromInput(const std::string &input,
                                          Container &variables,
                                          const std::vector<std::string> &lineDelimiters) const
    {
        std::istringstream stream(input);

        return getVariableNamesFromInput(stream, variables, lineDelimiters, m_variableValueRegex);
    }

    /**
     * Get a list of variables from the stream
     * @param stream         a reference to an std::istream object
     * @param variables      an stl-style container to hold the list of variables
     * @param lineDelimiters a vector of strings that is used to split the input stream into individual
     *                       lines
     */
    template<typename Container>
    void getVariableNamesFromInput(std::istream &stream,
                                   Container &variables,
                                   const std::vector<std::string> &lineDelimiters) const
    {
        auto is_space = [] (const std::string &str) { return std::all_of(str.cbegin(), str.cend(), isspace); };
        m_lineTokenizer.setDelimiters(lineDelimiters);
        auto &&lineTokens = m_lineTokenizer.parse<std::string>(stream, is_space);

        getVariableNamesFromInput(lineTokens, variables, m_variableValueRegex);
    }

    /**
     * Get a list of variables from the vector of tokenized lines
     * @param lineTokens         a vector of tokenized strings containing variable-value expressions
     * @param variables          an stl-style container to hold the list of variables
     * @param criteria           a regular expression object that will attempt to match the desired variable
     *                           declarations; if unspecified, all variables names will be returned
     * @param variableValueRegex an optional regular expression containing two capture expressions, one of
     *                           which defines how to extract the variable name and the other defines how to
     *                           extract its associated value
     */
    template<typename Container>
    static void getVariableNamesFromInput(const std::vector<std::string> &lineTokens,
                                          Container &variables,
                                          const std::string &variableValueRegex = "^\\s*(.+?)\\s*=\\s*(.+?)\\s*$")
    {
        for (auto &&line : lineTokens)
        {
            std::smatch match;
            if (std::regex_search(line, match, std::regex(variableValueRegex))  && match.size() == 3)
            {
                auto &&variable = match[1].str();

                addVariableNameToContainer(variable, variables);
            }
        }
    }

    /**
     * Get variable registry associated with this dictionary
     */
    inline virtual VariableRegistry *getVariableRegistry(void) const final
    {
        return m_pRegistry;
    }

    /**
     * Get this object's regular expression containing two capture expressions, one of which defines how to
     * extract the variable name and the other defines how to extract its associated value
     */
    inline virtual std::string getVariableValueRegex(void) const final
    {
        return m_variableValueRegex;
    }

    /**
     * Function to get the flag that indicates that variables referenced within the stream will be ignored when
     * populating the dictionary if they are not defined within the current object's variable registry
     */
    inline virtual bool ignoreUndefinedVariables(void) const final
    {
        return m_bIgnoreUndefinedVariables;
    }

    /**
     * Function to set the flag that indicates that variables referenced within the stream will be ignored when
     * populating the dictionary if they are not defined within the current object's variable registry
     */
    inline virtual void ignoreUndefinedVariables(bool bIgnoreUndefinedVariables) final
    {
        m_bIgnoreUndefinedVariables = bIgnoreUndefinedVariables;
    }

    /**
     * Assign values to variables in the dictionary from the input string
     */
    inline virtual bool populate(const std::string &input) final
    {
        std::istringstream stream(input);

        return populate(stream);
    }

    /**
     * Assign values to variables in the dictionary from the input stream
     */
    virtual bool populate(std::ifstream &&stream) final
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            bSuccess = populate(stream);
            stream.close();
        }

        return bSuccess;
    }

    /**
     * Assign values to variables in the dictionary from the input stream
     */
    inline virtual bool populate(std::istream &stream) final
    {
        return populate(stream, { "\n", "\r" });
    }

    /**
     * Assign values to variables in the dictionary from the input string
     * @param input          the input string
     * @param lineDelimiters a vector of strings that is used to split the input string into individual
     *                       lines
     */
    inline virtual bool populate(const std::string &input,
                                 const std::vector<std::string> &lineDelimiters) final
    {
        std::istringstream stream(input);

        return populate(stream, lineDelimiters);
    }

    /**
     * Assign values to variables in the dictionary from the input stream
     * @param stream         an r-value reference to an std::ifstream object
     * @param lineDelimiters a vector of strings that is used to split the input stream into individual
     *                       lines
     */
    virtual bool populate(std::ifstream &&stream,
                          const std::vector<std::string> &lineDelimiters) final
    {
        bool bSuccess = (bool)stream;
        if (bSuccess)
        {
            bSuccess = populate(stream, lineDelimiters);
            stream.close();
        }

        return bSuccess;
    }

    /**
     * Assign values to variables in the dictionary from the input stream
     * @param stream         a reference to an std::istream object
     * @param lineDelimiters a vector of strings that is used to split the input stream into individual
     *                       lines
     */
    virtual bool populate(std::istream &stream,
                          const std::vector<std::string> &lineDelimiters) final
    {
        auto is_space = [] (const std::string &str) { return std::all_of(str.cbegin(), str.cend(), isspace); };
        m_lineTokenizer.setDelimiters(lineDelimiters);
        auto &&lineTokens = m_lineTokenizer.parse<std::string>(stream, is_space);

        return populate(lineTokens);
    }

    /**
     * Assign values to variables in the dictionary from the line tokens
     * @param lineTokens a vector of tokenized strings containing variable-value expressions
     */
    virtual bool populate(const std::vector<std::string> &lineTokens) const final
    {
        // search the input vector of line tokens for variables defined in the registry
        bool bSuccess = (m_pRegistry != nullptr);
        if (bSuccess)
        {
            for (auto &&line : lineTokens)
            {
                std::smatch match;
                if (std::regex_search(line, match, std::regex(m_variableValueRegex)) && match.size() == 3)
                {
                    auto &&variable = match[1].str();
                    auto &&value = match[2].str();

                    auto &&itRegistryEntry = m_pRegistry->findByName(variable);
                    if (itRegistryEntry != m_pRegistry->cend())
                    {
                        if (!value.empty())
                            bSuccess &= itRegistryEntry->second.assign(value);
                    }
                    else if (!m_bIgnoreUndefinedVariables)
                        return false;
                }
            }
        }

        return bSuccess;
    }

    /**
     * Assign values to variables in the dictionary from the token map
     * @param tokenMap a map of variable-value token pairs
     */
    virtual bool populate(const VariableRegistry::tTokenMap &tokenMap) final
    {
        // search the input token map for variables defined in the registry
        bool bSuccess = (m_pRegistry != nullptr);
        if (bSuccess)
        {
            for (auto &&itTokenPair : tokenMap)
            {
                std::string name(itTokenPair.first);
                std::string value(itTokenPair.second);

                StringUtilities::trimLeadingAndTrailingWhitespace(&name);
                StringUtilities::trimLeadingAndTrailingWhitespace(&value);

                auto &&itRegistryEntry = m_pRegistry->findByName(name);
                if (itRegistryEntry != m_pRegistry->cend())
                {
                    if (!value.empty())
                        bSuccess &= itRegistryEntry->second.assign(value);
                }
                else if (!m_bIgnoreUndefinedVariables)
                    return false;
            }
        }

        return bSuccess;
    }

    /**
     * Set variable registry associated with this dictionary
     */
    inline virtual void setVariableRegistry(VariableRegistry *pRegistry) final
    {
        m_pRegistry = pRegistry;
    }

    /**
     * Set this object's regular expression containing two capture expressions, one of which defines how to
     * extract the variable name and the other defines how to extract its associated value
     */
    inline virtual void setVariableValueRegex(const std::string &variableValueRegex) final
    {
        m_variableValueRegex = variableValueRegex;
    }

private:

    /**
     * flag to indicate that undefined variables will be ignored when populating the variable registry
     */
    bool m_bIgnoreUndefinedVariables;

    /**
     * tokenizer object
     */
    mutable Tokenizer m_lineTokenizer;

    /**
     * pointer to variable registry
     */
    VariableRegistry *m_pRegistry;

    /**
     * a regular expression containing two capture expressions, one of which defines how to extract the
     * variable name and the other defines how to extract its associated value
     */
    std::string m_variableValueRegex;
};

}

#endif
