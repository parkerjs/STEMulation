#include "httpHeaders.h"
#include "socket.h"
#include "string_utilities.h"

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <winsock.h>
#elif defined POSIX
#include <sys/socket.h>
#endif

// using namespace declarations
using namespace networking::sockets;
using namespace utilities::string;

namespace networking
{

/**
 * Constructor
 */
HttpHeaders::HttpHeaders(void)
{

}

/**
 * Copy constructor
 */
HttpHeaders::HttpHeaders(const HttpHeaders &httpHeaders)
{
    operator = (httpHeaders);
}

/**
 * Move constructor
 */
HttpHeaders::HttpHeaders(HttpHeaders &&httpHeaders)
{
    operator = (std::move(httpHeaders));
}

/**
 * Destructor
 */
HttpHeaders::~HttpHeaders(void)
{

}

/**
 * Copy assignment operator
 */
HttpHeaders &HttpHeaders::operator = (const HttpHeaders &httpHeaders)
{
    if (&httpHeaders != this)
    {
        m_entries = httpHeaders.m_entries;
    }

    return *this;
}

/**
 * Move assignment operator
 */
HttpHeaders &HttpHeaders::operator = (HttpHeaders &&httpHeaders)
{
    if (&httpHeaders != this)
    {
        m_entries = std::move(httpHeaders.m_entries);
    }

    return *this;
}

/**
 * Add entries to this object's map of HTTP headers; returns successful if a duplicate value doesn't already
 * exist for the given field
 */
bool HttpHeaders::addEntries(const tTokenMap &entries)
{
    bool bSuccess = false;
    for (auto &&entry : entries)
    {
        bSuccess = addEntry(entry.first, entry.second);
        if (!bSuccess)
            break;
    }

    return bSuccess;
}

/**
 * Add entries to this object's map of HTTP headers; returns successful if a duplicate value doesn't already
 * exist for the given field
 */
bool HttpHeaders::addEntries(const tTokenVectorMap &entries)
{
    bool bSuccess = false;
    for (auto &&entry : entries)
    {
        bSuccess = addEntry(entry.first, entry.second);
        if (!bSuccess)
            break;
    }

    return bSuccess;
}

/**
 * Add an entry to this object's map of HTTP headers; returns successful if a duplicate value doesn't already
 * exist for the given field
 * @param field the name of the header field to be added
 * @param value the value associated with the specified header field
 */
bool HttpHeaders::addEntry(const std::string &field,
                           const std::string &value)
{
    bool bSuccess = (!field.empty() && !value.empty());
    if (bSuccess)
    {
        auto &&values = m_entries[field];
        auto &&itValue = std::find(values.cbegin(), values.cend(), value);
        if (itValue == values.cend())
            values.push_back(value);
    }

    return bSuccess;
}

/**
 * Add an entry to this object's map of HTTP headers; returns successful if a duplicate value doesn't already
 * exist for the given field
 * @param field  the name of the header field to be added
 * @param values a vector of tokens associated with the specified header field
 */
bool HttpHeaders::addEntry(const std::string &field,
                           const tTokenVector &values)
{
    bool bSuccess = false;
    for (auto &&value : values)
    {
        bSuccess = addEntry(field, value);
        if (!bSuccess)
            break;
    }

    return bSuccess;
}

/**
 * clone() function
 */
HttpHeaders *HttpHeaders::clone(void) const
{
    return new HttpHeaders(*this);
}

/**
 * Test whether or not an entry within this object's map of HTTP headers is associated with the specified field
 */
bool HttpHeaders::containsEntry(const std::string &field) const
{
    return m_entries.find(field) != m_entries.cend();
}

/**
 * Input stream extraction function
 * @param stream a reference to an std::istream object
 */
std::istream &HttpHeaders::extract(std::istream &stream)
{
    clearEntries();

    std::string data;
    while (std::getline(stream, data, '\n') && !data.empty())
    {
        auto &&posColon = data.find(":");
        if (posColon != std::string::npos)
        {
            std::string &&field = data.substr(0, posColon);
            std::string &&value = data.substr(posColon + 1, data.size() - posColon - 1);

            // trim leading and trailing whitespace
            StringUtilities::trimLeadingAndTrailingWhitespace(&field);
            StringUtilities::trimLeadingAndTrailingWhitespace(&value);

            m_entries[field].push_back(value);
        }

        data.clear();
    }

    return stream;
}

/**
 * Retrieve the value(s) associated with the specified field from this object's map of HTTP headers; returns a
 * non-null pointer to a vector of values upon successfully finding an existing entry
 */
const HttpHeaders::tTokenVector *HttpHeaders::get(const std::string &field) const
{
    const tTokenVector *pValues = nullptr;
    auto &&itField = m_entries.find(field);
    if (itField != m_entries.cend())
        pValues = &itField->second;

    return pValues;
}

/**
 * Retrieve the value(s) associated with the specified field from this object's map of HTTP headers; returns
 * true upon successfully finding an existing entry
 */
bool HttpHeaders::get(const std::string &field,
                      tTokenVector &values) const
{
    auto &&itField = m_entries.find(field);
    bool bSuccess = (itField != m_entries.cend());
    if (bSuccess)
        values = itField->second;

    return bSuccess;
}

/**
 * Get the content length from this object's map of HTTP headers; returns -1 if the field is not present
 */
long HttpHeaders::getContentLength(void) const
{
    auto &&itEntry = m_entries.find("Content-Length");
    if (itEntry != m_entries.cend())
    {
        auto &&values = itEntry->second;
        if (!values.empty())
            return std::atoi(values.front().c_str());
    }

    return -1;
}

/**
 * Get the transfer encoding used, if any, from this object's map of HTTP headers
 */
std::string HttpHeaders::getTransferEncoding(void) const
{
    auto &&itEntry = m_entries.find("Transfer-Encoding");
    if (itEntry != m_entries.cend())
    {
        auto &&values = itEntry->second;
        if (!values.empty())
            return values.front();
    }

    return "";
}

/**
 * Get a flag from this object's map of HTTP headers indicating whether or not keep-alive is supported
 */
bool HttpHeaders::keepAliveSupported(void) const
{
    auto &&itEntry = m_entries.find("Connection");
    if (itEntry != m_entries.cend())
    {
        auto &&values = itEntry->second;
        for (auto &&value : values)
        {
            std::string connection(value);
            std::transform(connection.begin(), connection.end(), connection.begin(), ::tolower);
            if (connection.find("keep-alive") != std::string::npos)
                return true;
        }
    }

    return false;
}

/**
 * Output stream print function
 * @param stream a reference to an std::ostream object
 */
std::ostream &HttpHeaders::print(std::ostream &stream) const
{
    if (stream)
    {
        for (auto &&entry : m_entries)
        {
            auto &&field = entry.first;
            auto &&values = entry.second;
            for (auto &&value : values)
                stream << field << ": " << value << "\r\n";
        }
    }

    return stream;
}

/**
 * Read the HTTP response headers from the specified socket
 * @param pSocket a pointer to a socket objects
 */
bool HttpHeaders::readHeaders(Socket *pSocket)
{
    bool bSuccess = (pSocket != nullptr);
    if (bSuccess)
    {
        char buffer[5] = { 0 };
        auto &&result = pSocket->read(buffer, 4, MSG_PEEK);
        if (result > 0 && strcmp(buffer, "HTTP") == 0)
        {
            std::string data;
            for (std::size_t size = 0; pSocket->readLine(&data) && size != data.size(); size = data.size())
                data += '\n'; // append a newline to each line

            std::istringstream iss(data);
            extract(iss);
        }
    }

    return bSuccess;
}

/**
 * Remove entries from this object's map of HTTP headers
 * @param fields a list of names for which associated entries shall be removed
 */
void HttpHeaders::removeEntries(const tTokenVector &fields)
{
    for (auto &&field : fields)
    {
        auto &&itEntry = m_entries.find(field);
        if (itEntry != m_entries.end())
            m_entries.erase(itEntry);
    }
}

}
