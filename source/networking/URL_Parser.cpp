#include "URL_Parser.h"
#include <regex>

namespace networking
{

/**
 * Constructor
 */
URL_Parser::URL_Parser(void)
{

}

/**
 * Constructor
 * @param url the web address to be parsed
 */
URL_Parser::URL_Parser(const std::string &url)
{
    parse(url);
}

/**
 * Copy constructor
 */
URL_Parser::URL_Parser(const URL_Parser &parser)
{
    operator = (parser);
}

/**
 * Move constructor
 */
URL_Parser::URL_Parser(URL_Parser &&parser)
{
    operator = (std::move(parser));
}

/**
 * Destructor
 */
URL_Parser::~URL_Parser(void)
{

}

/**
 * Copy assignment operator
 */
URL_Parser &URL_Parser::operator = (const URL_Parser &parser)
{
    if (&parser != this)
    {
        URL::operator = (parser);
    }

    return *this;
}

/**
 * Move assignment operator
 */
URL_Parser &URL_Parser::operator = (URL_Parser &&parser)
{
    if (&parser != this)
    {
        URL::operator = (std::move(parser));
    }

    return *this;
}

/**
 * clone() function
 */
URL_Parser *URL_Parser::clone(void) const
{
    return new URL_Parser(*this);
}

/**
 * Function to parse the specified URL; sets this object's current url to the input argument
 * @param url the web address to be parsed
 */
bool URL_Parser::parse(const std::string &url)
{
    bool bSuccess = initialize();
    if (bSuccess)
    {
        std::size_t pos = 0;
        parseScheme(url, pos);
        parseUserInformation(url, pos);
        parseHost(url, pos);
        parsePort(url, pos);
        parsePath(url, pos);
        parseQuery(url, pos);
        parseFragment(url, pos);
    }

    return bSuccess;
}

/**
 * Parse the fragment identifier
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parseFragment(const std::string &url, std::size_t &pos)
{
    if (pos != std::string::npos && url[pos] == '#')
    {
        // parse fragment
        m_fragment = url.substr(++pos, std::string::npos);
    }
}

/**
 * Parse the host name
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parseHost(const std::string &url, std::size_t &pos)
{
    if (pos != std::string::npos)
    {
        // parse host address
        auto endHostPos = url.find_first_of(":/?#", pos);
        if ((endHostPos == std::string::npos ||
             endHostPos != std::string::npos) && url.find('.', pos) < endHostPos &&
                                                 url.find("./", pos) >= endHostPos &&
                                                 url.find("../", pos) >= endHostPos)
        {
            m_host = url.substr(pos, endHostPos - pos);

            // update position in the string
            pos = endHostPos;
        }
    }
}

/**
 * Parse the path component
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parsePath(const std::string &url, std::size_t &pos)
{
    // parse path
    if (pos != std::string::npos)
    {
        auto &&endPathPos = url.find_first_of("?#", pos);
        m_path = url.substr(pos, endPathPos - pos);

        // update position in string
        pos = endPathPos;
    }
}

/**
 * Parse the port number
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parsePort(const std::string &url, std::size_t &pos)
{
    if (pos != std::string::npos && url[pos] == ':')
    {
        // parse port number
        auto &&endPortPos = url.find_first_of("/?#", ++pos);
        if (endPortPos != std::string::npos)
        {
            m_port = url.substr(pos, endPortPos - pos);

            // update position in the string
            pos = endPortPos;
        }
    }
}

/**
 * Parse the query string
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parseQuery(const std::string &url, std::size_t &pos)
{
    if (pos != std::string::npos && url[pos] == '?')
    {
        // parse query
        auto &&endQueryPos = url.find_first_of("#", ++pos);
        auto &&query = url.substr(pos, endQueryPos - pos);

        parseQueryParameters(query);

        // update position in string
        pos = endQueryPos;
    }
}

/**
 * Parse query parameter from query string
 * @param query the query string
 */
void URL_Parser::parseQueryParameters(const std::string &query)
{
    m_queryParameterPairs.clear();
    std::size_t position = 0;
    std::regex criteria("([^=]+)(?:\\=([^&]+))*&*");
    std::sregex_iterator itRegex(query.cbegin() + position, query.cend(), criteria), itRegexEnd;
    while (itRegex != itRegexEnd)
    {
        position += itRegex->position();
        std::smatch match = *itRegex;
        std::string parameter, value;
        if (match.size() >= 2)
        {
            parameter = match[1];
            if (match.size() == 3)
                value = match[2];
        }

        if (!parameter.empty())
            m_queryParameterPairs.push_back(std::make_pair(parameter, value));

        ++itRegex;
    }
}

/**
 * Parse the protocol scheme
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parseScheme(const std::string &url, std::size_t &pos)
{
    if (pos != std::string::npos)
    {
        // parse protocol scheme
        auto &&endSchemePos = url.find("://", pos);
        if (endSchemePos != std::string::npos)
        {
            m_scheme = url.substr(pos, endSchemePos - pos);

            // update position in the string
            pos = endSchemePos + 3;
        }
    }
}

/**
 * Parse the user information
 * @param url the web address to be parsed
 * @param pos the position within the URL string currently under examination
 */
void URL_Parser::parseUserInformation(const std::string &url, std::size_t &pos)
{
    if (pos != std::string::npos)
    {
        // parse user name and password
        auto endUserInformation = url.find_first_of("@", pos);
        if (endUserInformation != std::string::npos)
        {
            auto userInformation = url.substr(pos, endUserInformation - pos);
            std::smatch match;
            std::regex criteria("(.+?)(?::(.+))*");
            if (std::regex_match(userInformation, match, criteria))
            {
                m_userName = match[1];
                if (match[2].matched)
                    m_userPassword = match[2];

                // update position in the string
                pos = endUserInformation + 1;
            }
        }
    }
}

}
