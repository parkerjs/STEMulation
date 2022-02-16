#include "cookie.h"
#include "date.h"
#include "dictionary.h"
#include "downloader.h"
#include "httpHeaders.h"
#include "static_message_dispatcher.h"
#include "string_utilities.h"
#include "subscriber.h"
#include "tokenizer.h"
#include <iomanip>

// file-scoped variables
static constexpr char className[] = "Cookie";

// using namespace declarations
using namespace attributes::concrete;
using namespace messaging;
using namespace utilities;
using namespace utilities::calendar;
using namespace utilities::string;

/**
 * std::istream overload for struct tm
 */
std::istream &operator >> (std::istream &stream,
                           struct tm &time)
{
    stream >> std::get_time(&time, "%a, %d");
    stream.ignore(); // could be a dash or a whitespace character
    stream >> std::get_time(&time, "%b");
    stream.ignore(); // could be a dash or a whitespace character
    stream >> std::get_time(&time, "%Y %H:%M:%S GMT");

    return stream;
}

/**
 * std::ostream overload for struct tm
 */
std::ostream &operator << (std::ostream &stream,
                           const struct tm &time)
{
    stream << std::put_time(&time, "%a, %d %b %Y %H:%M:%S GMT");

    return stream;
}

namespace networking
{

namespace authentication
{

// register static recipients...
static StaticMessageRecipientRegistrar staticMessageRegistrar(
{ std::make_pair("StaticLoggable::setLoggingStreamMap<std::string, ToggleableStream>",
                 &Cookie::setLoggingStreamMap<std::string, ToggleableStream>),
  std::make_pair("StaticSynchronizable::setMutexMap<std::string, std::mutex *>",
                 &Cookie::setMutexMap<std::string, std::mutex *>)});

/**
 * std::istream overload for SameSiteEnum
 */
std::istream &operator >> (std::istream &stream,
                           Cookie::SameSiteEnum &value)
{
    std::string temp;
    stream >> temp;
    if (temp == "Lax")
        value = Cookie::SameSiteEnum::Lax;
    else if (temp == "Strict")
        value = Cookie::SameSiteEnum::Strict;
    else
        value = Cookie::SameSiteEnum::Unknown;

    return stream;
}

/**
 * std::ostream overload for SameSiteEnum
 */
std::ostream &operator << (std::ostream &stream,
                           const Cookie::SameSiteEnum &value)
{
    switch (value)
    {
        case Cookie::SameSiteEnum::Lax:
        stream << "Lax";
        break;

        case Cookie::SameSiteEnum::Strict:
        stream << "Strict";
        break;

        default:
        stream << "Unknown";
        break;
    }

    return stream;
}

/**
 * Constructor
 * @param pDownloader a pointer to a Downloader object which shall be used to download/refresh the cookie
 */
Cookie::Cookie(std::shared_ptr<Downloader> pDownloader)
: VariableMapRegistrable([] (auto &&left, auto &&right)
                         { return utilities::string::StringUtilities::
                                  caseInsensitiveLexicographicalComparator(left, right); }),
  m_bHttpOnly(false),
  m_bSecure(false),
  m_expiration({ 0 }),
  m_maxAge(0),
  m_pDownloader(pDownloader),
  m_sameSite(SameSiteEnum::Unknown)
{
    auto &&currentTime = time(0);
    Date::getMutex(0).lock();

    // convert time_t to tm struct for local timezone
    m_expiration = *localtime(&currentTime);
    Date::getMutex(0).unlock();
}

/**
 * Copy constructor
 */
Cookie::Cookie(const Cookie &cookie)
: m_pDownloader(nullptr)
{
    operator = (cookie);
}

/**
 * Move constructor
 */
Cookie::Cookie(Cookie &&cookie)
: m_pDownloader(nullptr)
{
    operator = (std::move(cookie));
}

/**
 * Destructor
 */
Cookie::~Cookie(void)
{
//  if (m_pDownloader != nullptr)
//      delete m_pDownloader;
}

/**
 * Copy assignment operator
 */
Cookie &Cookie::operator = (const Cookie &cookie)
{
    if (&cookie != this)
    {
        m_bHttpOnly = cookie.m_bHttpOnly;
        m_bSecure = cookie.m_bSecure;
        m_domain = cookie.m_domain;
        m_expiration = cookie.m_expiration;
        m_maxAge = cookie.m_maxAge;
        m_nameValuePairs = cookie.m_nameValuePairs;
        m_path = cookie.m_path;
        m_pDownloader = cookie.m_pDownloader;

//        if (m_pDownloader != nullptr)
//        {
//            delete m_pDownloader;
//            m_pDownloader = nullptr;
//        }
//
//        if (cookie.m_pDownloader != nullptr)
//            m_pDownloader = cookie.m_pDownloader->clone();

        m_sameSite = cookie.m_sameSite;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Cookie &Cookie::operator = (Cookie &&cookie)
{
    if (&cookie != this)
    {
        m_bHttpOnly = std::move(cookie.m_bHttpOnly);
        m_bSecure = std::move(cookie.m_bSecure);
        m_domain = std::move(cookie.m_domain);
        m_expiration = std::move(cookie.m_expiration);
        m_maxAge = std::move(cookie.m_maxAge);
        m_nameValuePairs = std::move(cookie.m_nameValuePairs);
        m_path = std::move(cookie.m_path);

//        if (m_pDownloader != nullptr)
//        {
//            delete m_pDownloader;
//            m_pDownloader = nullptr;
//        }

        m_pDownloader = std::move(cookie.m_pDownloader);
        cookie.m_pDownloader = nullptr;

        m_sameSite = std::move(cookie.m_sameSite);
    }

    return *this;
}

/**
 * Add a name-value pair to this cookie; returns true upon success
 */
bool Cookie::addNameValuePair(const std::string &name,
                              const std::string &value)
{
    bool bSuccess = (!name.empty() && !value.empty() && !m_registry.contains(name));
    if (bSuccess)
        m_nameValuePairs[name] = value;
    else
    {
        logMsg("error", LoggingLevel::Enum::Error,
               "Name-value pair must be non-empty and name must be unique from cookie field name!\n\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Configure function (using a variable token map)
 */
bool Cookie::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}

/**
 * create() function
 * @param pDownloader a pointer to a Downloader object which shall be used to download/refresh the cookie
 */
Cookie *Cookie::create(std::shared_ptr<Downloader> pDownloader)
{
    auto *pCookie = new Cookie(pDownloader);
    pCookie->setup();

    return pCookie;
}

/**
 * Get the name of this class
 */
std::string Cookie::getClassName(void) const
{
    return className;
}

/**
 * Get the value associated with the specified name; return a non-empty string upon success
 */
std::string Cookie::getValue(const std::string &name) const
{
    auto &&itNameValuePair = m_nameValuePairs.find(name);
    if (itNameValuePair != m_nameValuePairs.cend())
        return itNameValuePair->second;

    return "";
}

/**
 * Read cookie information from HTTP header token map; returns true if cookie was successfully read or false if
 * an error occurred or no cookie information was present
 * @param pHttpResponseHeaders a pointer to an HttpHeaders container, which upon success will be populated with
 *                             the HTTP response headers
 */
bool Cookie::read(HttpHeaders *pHttpResponseHeaders)
{
    bool bSuccess = (pHttpResponseHeaders != nullptr);
    if (bSuccess)
    {
        auto *pValues = pHttpResponseHeaders->get("set-cookie");
        bSuccess = (pValues != nullptr);
        if (bSuccess)
        {
            for (auto &&value : *pValues)
            {
                if (bSuccess)
                    bSuccess = read(value);
            }
        }
    }

    return bSuccess;
}

/**
 * Read cookie information from the specified string input; returns true if cookie was successfully read or
 * false if an error occurred or no cookie information was present
 * @param header the cookie directives from an HTTP response header
 */
bool Cookie::read(const std::string &header)
{
    bool bSuccess = true;
    std::size_t posEndField, posBeginField = 0;
    do
    {
        posEndField = header.find(";", posBeginField);
        if (posEndField == std::string::npos)
            posEndField = header.size();

        std::string value;
        auto &&posEqualSign = header.find("=", posBeginField);
        if (posEqualSign == std::string::npos)
            posEqualSign = posEndField;

        auto &&name = header.substr(posBeginField, posEqualSign - posBeginField);
        StringUtilities::trimLeadingAndTrailingWhitespace(&name);
        if (posEqualSign == posEndField)
            value = "true";
        else
        {
            value = header.substr(posEqualSign + 1, posEndField - posEqualSign - 1);
            StringUtilities::trimLeadingAndTrailingWhitespace(&value);
        }

        if (m_registry.contains(name))
        {
            // it's a field other than cookie name-value pair
            bSuccess = m_registry.assign(name, value);
        }
        else if (!name.empty())
        {
            // it's a cookie name-value pair
            m_nameValuePairs[name] = value;
        }
        else
        {
            bSuccess = false;
        }

        posBeginField = posEndField + 1;
    }
    while (bSuccess && posBeginField < header.size());

    if (!bSuccess)
    {
        logMsg("error", LoggingLevel::Enum::Error,
               "Error encountered while attempting to read cookie from header!\n\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Request a cookie refresh from the server
 */
bool Cookie::refresh(void)
{
    bool bSuccess = (m_pDownloader != nullptr);
    if (bSuccess)
    {
        auto *pHttpResponseHeaders = m_pDownloader->getHttpResponseHeaders();
        bSuccess = read(pHttpResponseHeaders);
    }
    else
    {
        logMsg("error", LoggingLevel::Enum::Error,
               "Downloader object is null, but it must be set in order to refresh cookie!\n\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Set pointer to downloader object (object is moved into a shared_ptr)
 */
void Cookie::setDownloader(Downloader *&&pDownloader)
{
    m_pDownloader.reset(std::move(pDownloader));

    pDownloader = nullptr;
}

/**
 * Setup function
 */
bool Cookie::setup(void)
{
    // setup token map-configurable variables...
    m_registry["Domain"] = m_domain;
    m_registry["Expires"] = m_expiration;
    m_registry["HttpOnly"] = m_bHttpOnly;
    m_registry["Max-Age"] = m_maxAge;
    m_registry["Path"] = m_path;
    m_registry["SameSite"] = m_sameSite;
    m_registry["Secure"] = m_bSecure;

    return true;
}

/**
 * Set the value associated with the specified name; returns true if the value is non-empty and the name for
 * which the value will be associated exists in this object's map of name-value pairs
 */
bool Cookie::setValue(const std::string &name,
                      const std::string &value)
{
    auto &&itNameValuePair = m_nameValuePairs.find(name);
    bool bSuccess = (itNameValuePair != m_nameValuePairs.end() && !value.empty());
    if (bSuccess)
        itNameValuePair->second = value;

    return bSuccess;
}

/**
 * Write (append) cookie to the specified header string
 */
bool Cookie::write(std::string &header) const
{
    bool bSuccess = (!m_nameValuePairs.empty());
    if (bSuccess)
    {
        auto &&itNameValuePair = m_nameValuePairs.cbegin();
        do
        {
            if (itNameValuePair != m_nameValuePairs.cbegin())
                header += "; ";

            header += itNameValuePair->first + "=" + itNameValuePair->second;
        }
        while (++itNameValuePair != m_nameValuePairs.cend());

        struct tm time = { 0 };
        if (memcmp(&m_expiration, &time, sizeof(tm)) != 0)
        {
            std::ostringstream stream;
            stream << m_expiration;
            header += "; Expires=" + stream.str();
        }

        if (m_maxAge > 0)
            header += "; Max-Age=" + std::to_string(m_maxAge);

        if (!m_domain.empty())
            header += "; Domain=" + m_domain;

        if (!m_path.empty())
            header += "; Path=" + m_path;

        if (m_bSecure)
            header += "; Secure";

        if (m_bHttpOnly)
            header += "; HttpOnly";

        if (m_sameSite != SameSiteEnum::Unknown)
        {
            header += "; SameSite=";
            std::ostringstream oss;
            oss << m_sameSite;
            header += oss.str();
        }
    }
    else
    {
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Name-value pairs map is empty, cannot write cookie to header!\n\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

}

}
