#include "URL.h"
#include "URL_Parser.h"
#include <algorithm>
#include <sstream>

namespace networking
{

/**
 * Constructor
 */
URL::URL(void)
: m_bEncodeURL(false)
{

}

/**
 * Constructor
 * @param url the web address to be parsed into components and stored within this object
 */
URL::URL(const std::string &url)
: m_bEncodeURL(false)
{
    setURL(url);
}

/**
 * Copy constructor
 */
URL::URL(const URL &url)
{
    operator = (url);
}

/**
 * Move constructor
 */
URL::URL(URL &&url)
{
    operator = (std::move(url));
}

/**
 * Destructor
 */
URL::~URL(void)
{

}

/**
 * Copy assignment operator
 */
URL &URL::operator = (const URL &url)
{
    if (&url != this)
    {
        m_bEncodeURL = url.m_bEncodeURL;
        m_fragment = url.m_fragment;
        m_host = url.m_host;
        m_path = url.m_path;
        m_port = url.m_port;
        m_queryParameterPairs = url.m_queryParameterPairs;
        m_scheme = url.m_scheme;
        m_userName = url.m_userName;
        m_userPassword = url.m_userPassword;
    }

    return *this;
}

/**
 * Move assignment operator
 */
URL &URL::operator = (URL &&url)
{
    if (&url != this)
    {
        m_bEncodeURL = std::move(url.m_bEncodeURL);
        m_fragment = std::move(url.m_fragment);
        m_host = std::move(url.m_host);
        m_path = std::move(url.m_path);
        m_port = std::move(url.m_port);
        m_queryParameterPairs = std::move(url.m_queryParameterPairs);
        m_scheme = std::move(url.m_scheme);
        m_userName = std::move(url.m_userName);
        m_userPassword = std::move(url.m_userPassword);
    }

    return *this;
}

/**
 * Function to convert ascii to hexadecimal character
 */
char URL::asciiToHexadecimal(char ascii)
{
    return ascii + (ascii > 9 ? ('A' - 10) : '0');
}

/**
 * Clear all or part of the fields that comprise the URL represented by this object
 * @param fields a bit-field mask that specifies the desired fields to be cleared; the individual bits are
 *               defined in the URL_Field enumeration
 */
void URL::clear(short fields)
{
    if (fields & URL_Field::Scheme)
        m_scheme.clear();

    if (fields & (URL_Field::UserName | URL_Field::UserNameAndUserPassword))
        m_userName.clear();

    if (fields & URL_Field::UserNameAndUserPassword)
        m_userPassword.clear();

    if (fields & URL_Field::Host)
        m_host.clear();

    if (fields & URL_Field::Port)
        m_port.clear();

    if (fields & URL_Field::Path)
        m_path.clear();

    if (fields & URL_Field::Query)
        m_queryParameterPairs.clear();

    if (fields & URL_Field::Fragment)
        m_fragment.clear();
}

/**
 * clone() function
 */
URL *URL::clone(void) const
{
    return new URL(*this);
}

/**
 * Remove URL-encoding from the specified URL
 */
std::string URL::decode(const std::string &url)
{
    std::string result;
    for (std::string::size_type i = 0; i < url.size(); ++i)
    {
        if (url[i] == '+')
            result += ' ';
        else if (url[i] == '%' && url.size() > i + 2)
        {
            auto &&ch1 = hexadecimalToAscii(url[i + 1]);
            auto &&ch2 = hexadecimalToAscii(url[i + 2]);
            auto &&ch = (ch1 << 4) | ch2;
            result += ch;
            i += 2;
        }
        else
            result += url[i];
    }

    return result;
}

/**
 * Test for empty object
 */
bool URL::empty(void) const
{
    return m_fragment.empty() &&
           m_host.empty() &&
           m_path.empty() &&
           m_port.empty() &&
           m_queryParameterPairs.empty() &&
           m_scheme.empty() &&
           m_userName.empty() &&
           m_userPassword.empty();
}

/**
 * Apply URL-encoding to the specified URL
 */
std::string URL::encode(const std::string &url)
{
    std::ostringstream stream;
    for (auto &&ch : url)
    {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
            stream << ch;
        else if (ch == ' ')
            stream << '+';
        else
            stream << '%' << asciiToHexadecimal(ch >> 4)
                          << asciiToHexadecimal(ch % 16);
    }

    return stream.str();
}

/**
 * Input stream extraction function
 * @param stream a reference to an std::istream object
 */
std::istream &URL::extract(std::istream &stream)
{
    std::string url;
    stream >> url;

    setURL(url);

    return stream;
}

/**
 * Get the authority string
 */
std::string URL::getAuthority(void) const
{
    std::string authority;
    if (!m_userName.empty() && !m_userPassword.empty())
        authority = m_userName + ":" + m_userPassword + "@";

    if (!m_host.empty())
    {
        authority += m_host;
        if (!m_port.empty())
            authority += ":" + m_port;
    }

    return authority;
}

/**
 * Get the query string
 */
std::string URL::getQuery(void) const
{
    std::string query;
    for (auto &&queryParameterPair : m_queryParameterPairs)
    {
        if (!query.empty())
            query += "&";

        query += queryParameterPair.first;
        if (!queryParameterPair.second.empty())
            query += "=" + queryParameterPair.second;
    }

    return query;
}

/**
 * Get a query parameter
 */
std::string URL::getQueryParameter(const std::string &parameter) const
{
    auto &&itQueryParameterPair = std::find_if(m_queryParameterPairs.cbegin(), m_queryParameterPairs.cend(),
                                               [&parameter] (auto &&pair) { return pair.first == parameter; });
    if (itQueryParameterPair != m_queryParameterPairs.cend())
        return itQueryParameterPair->second;

    return "";
}

/**
 * Get this object's request URI
 */
std::string URL::getRequestURI(void) const
{
    std::string uri;
    if (!m_path.empty())
    {
        if (m_path.find_first_of('/') != 0)
            uri += "/";

        uri += m_path;
    }

    auto &&query = getQuery();
    if (!query.empty())
    {
        if (query.find_first_of('?') != 0)
            uri += "?";

        uri += query;
    }

    if (!m_fragment.empty())
    {
        if (m_fragment.find_first_of('#') != 0)
            uri += "#";

        uri += m_fragment;
    }

    return m_bEncodeURL ? encode(uri) : uri;
}

/**
 * Get all or part of this object's URL
 * @param fields a bit-field mask that specifies the desired fields to be included in the URL returned by this
 *               function; the individual bits are defined in the URL_Field enumeration
 */
std::string URL::getURL(short fields) const
{
    std::string url;
    if (fields & URL_Field::Scheme && !m_scheme.empty())
        url += m_scheme + "://";

    if (!m_userName.empty())
    {
        if (fields & URL_Field::UserNameAndUserPassword && !m_userPassword.empty())
            url += m_userName + ":" + m_userPassword + "@";
        else if (fields & URL_Field::UserName)
            url += m_userName + "@";
    }

    if (fields & URL_Field::Host && !m_host.empty())
        url += m_host;

    if (fields & URL_Field::Port && !m_port.empty())
        url += ":" + m_port;

    if (fields & URL_Field::Path && !m_path.empty())
    {
        if (m_path.find_first_of('/') != 0)
            url += "/";

        url += m_path;
    }

    auto &&query = getQuery();
    if (fields & URL_Field::Query && !query.empty())
    {
        if (query.find_first_of('?') != 0)
            url += "?";

        url += query;
    }

    if (fields & URL_Field::Fragment && !m_fragment.empty())
    {
        if (m_fragment.find_first_of('#') != 0)
            url += "#";

        url += m_fragment;
    }

    return m_bEncodeURL ? encode(url) : url;
}

/**
 * Get the user info string
 */
std::string URL::getUserInfo(void) const
{
    std::string userInfo;
    if (!m_userName.empty())
    {
        userInfo = m_userName;
        if (!m_userPassword.empty())
            userInfo += ":" + m_userPassword;
    }

    return userInfo;
}

/**
 * Function to convert a hexadecimal character to ascii
 */
char URL::hexadecimalToAscii(char hex)
{
    if (hex <= '9' && hex >= '0')
        return hex - '0';
    else if (hex <= 'f' && hex >= 'a')
        return hex - 'a' - 10;
    else if (hex <= 'F' && hex >= 'A')
        return hex - 'A' - 10;
    else
        return 0;
}

/**
 * Initialization function
 */
bool URL::initialize(void)
{
    m_fragment.clear();
    m_host.clear();
    m_path.clear();
    m_port.clear();
    m_queryParameterPairs.clear();
    m_scheme.clear();
    m_userName.clear();
    m_userPassword.clear();

    return true;
}

/**
 * Output stream print function
 * @param stream a reference to an std::ostream object
 */
std::ostream &URL::print(std::ostream &stream) const
{
    if (stream)
    {
        stream << getURL();
    }

    return stream;
}

/**
 * Remove a query parameter
 */
bool URL::removeQueryParameter(const std::string &parameter)
{
    auto &&itQueryParameterPair = std::find_if(m_queryParameterPairs.begin(), m_queryParameterPairs.end(),
                                               [&parameter] (auto &&pair) { return pair.first == parameter; });
    bool bSuccess = (itQueryParameterPair != m_queryParameterPairs.cend());
    if (bSuccess)
        m_queryParameterPairs.erase(itQueryParameterPair);

    return bSuccess;
}

/**
 * Set the query string
 */
void URL::setQuery(const std::string &query)
{
    URL_Parser urlParser;
    urlParser.parseQueryParameters(query);

    m_queryParameterPairs = urlParser.getQueryParameterPairs();
}

/**
 * Set a query parameter; returns true upon success
 */
bool URL::setQueryParameter(const std::string &parameter,
                            const std::string &value)
{
    bool bSuccess = !parameter.empty();
    if (bSuccess)
    {
        auto &&itQueryParameterPair = std::find_if(m_queryParameterPairs.begin(), m_queryParameterPairs.end(),
                                                   [&parameter] (auto &&pair)
                                                   {
                                                       return pair.first == parameter;
                                                   });
        if (itQueryParameterPair != m_queryParameterPairs.end())
            itQueryParameterPair->second = value;
        else
            m_queryParameterPairs.push_back(std::make_pair(parameter, value));
    }

    return bSuccess;
}

/**
 * Set this object's URL
 * @param url the web address to be parsed into components and stored within this object
 */
void URL::setURL(const std::string &url)
{
    URL_Parser urlParser(url);

    m_fragment = urlParser.getFragment();
    m_host = urlParser.getHost();
    m_path = urlParser.getPath();
    m_port = urlParser.getPort();
    m_queryParameterPairs = urlParser.getQueryParameterPairs();
    m_scheme = urlParser.getScheme();
    m_userName = urlParser.getUserName();
    m_userPassword = urlParser.getUserPassword();
}

}
