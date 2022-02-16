#ifndef HTTP_HEADERS_H
#define HTTP_HEADERS_H

#include "cloneable.h"
#include "streamable.h"
#include <map>
#include <string>
#include <vector>

namespace networking
{

// forward declarations
namespace sockets { class Socket; }

/**
 * This class represents HTTP response and request headers
 */
class HttpHeaders
: public attributes::interfaces::Cloneable<HttpHeaders>,
  public attributes::concrete::Streamable<HttpHeaders>
{
public:

    /**
     * Typedef declarations
     */
    typedef std::map<std::string, std::string> tTokenMap;
    typedef std::vector<std::string> tTokenVector;
    typedef std::map<std::string, tTokenVector> tTokenVectorMap;

    /**
     * Constructor
     */
    EXPORT_STEM HttpHeaders(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM HttpHeaders(const HttpHeaders &httpHeaders);

    /**
     * Move constructor
     */
    EXPORT_STEM HttpHeaders(HttpHeaders &&httpHeaders);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~HttpHeaders(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM HttpHeaders &operator = (const HttpHeaders &httpHeaders);

    /**
     * Move assignment operator
     */
    EXPORT_STEM HttpHeaders &operator = (HttpHeaders &&httpHeaders);

    /**
     * Subscript operator
     */
    inline virtual tTokenVector &operator [] (const std::string &field) final
    {
        return m_entries[field];
    }

    /**
     * Add entries to this object's map of HTTP headers; returns successful if a duplicate value doesn't
     * already exist for the given field
     */
    EXPORT_STEM virtual bool addEntries(const tTokenMap &entries) final;

    /**
     * Add entries to this object's map of HTTP headers; returns successful if a duplicate value doesn't
     * already exist for the given field
     */
    EXPORT_STEM virtual bool addEntries(const tTokenVectorMap &entries) final;

    /**
     * Add an entry to this object's map of HTTP headers; returns successful if a duplicate value doesn't
     * already exist for the given field
     * @param field the name of the header field to be added
     * @param value the value associated with the specified header field
     */
    EXPORT_STEM virtual bool addEntry(const std::string &field,
                                      const std::string &value) final;

    /**
     * Add an entry to this object's map of HTTP headers; returns successful if a duplicate value doesn't
     * already exist for the given field
     * @param field  the name of the header field to be added
     * @param values a vector of tokens associated with the specified header field
     */
    EXPORT_STEM virtual bool addEntry(const std::string &field,
                                      const tTokenVector &values) final;

    /**
     * Clear all entries from this object's map of HTTP headers
     */
    inline virtual void clearEntries(void) final
    {
        m_entries.clear();
    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual HttpHeaders *clone(void) const override;

    /**
     * Test whether or not an entry within this object's map of HTTP headers is associated with the specified
     * field
     */
    EXPORT_STEM virtual bool containsEntry(const std::string &field) const final;

    /**
     * Input stream extraction function
     * @param stream a reference to an std::istream object
     */
    EXPORT_STEM virtual std::istream &extract(std::istream &stream) override;

    /**
     * Retrieve the value(s) associated with the specified field from this object's map of HTTP headers;
     * returns a non-null pointer to a vector of values upon successfully finding an existing entry
     */
    EXPORT_STEM virtual const tTokenVector *get(const std::string &field) const final;

    /**
     * Retrieve the value(s) associated with the specified field from this object's map of HTTP headers;
     * returns true upon successfully finding an existing entry
     */
    EXPORT_STEM virtual bool get(const std::string &field,
                                 tTokenVector &values) const final;

    /**
     * Get the content length from this object's map of HTTP headers; returns -1 if the field is not present
     */
    EXPORT_STEM virtual long getContentLength(void) const final;

    /**
     * Get this object's map of HTTP header entries
     */
    inline virtual tTokenVectorMap &getEntries(void) final
    {
        return m_entries;
    }

    /**
     * Get the transfer encoding used, if any, from this object's map of HTTP headers
     */
    EXPORT_STEM virtual std::string getTransferEncoding(void) const final;

    /**
     * Get a flag from this object's map of HTTP headers indicating whether or not keep-alive is supported
     */
    EXPORT_STEM virtual bool keepAliveSupported(void) const final;

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * Read the HTTP response headers from the specified socket
     * @param pSocket a pointer to a socket objects
     */
    EXPORT_STEM virtual bool readHeaders(sockets::Socket *pSocket) final;

    /**
     * Remove entries from this object's map of HTTP headers
     * @param fields a variadic argument list which identifies the associated entries to be removed
     */
    template<typename ... Fields>
    inline auto removeEntries(Fields && ... fields) -> typename
    std::enable_if<!std::is_same<typename std::decay<Fields ...>::type, tTokenVector>::value, void>::type
    {
        removeEntries(tTokenVector{ fields ... });
    }

    /**
     * Remove entries from this object's map of HTTP headers
     * @param fields a list of names for which associated entries shall be removed
     */
    EXPORT_STEM virtual void removeEntries(const tTokenVector &fields) final;

    /**
     * Remove an entry from this object's map of HTTP headers
     */
    inline virtual void removeEntry(const std::string &field) final
    {
        removeEntries(field);
    }

    /**
     * Set this object's map of HTTP header entries
     */
    inline virtual void setEntries(const tTokenVectorMap &entries) final
    {
        m_entries = entries;
    }

private:

    /**
     * a map of HTTP header entries
     */
    tTokenVectorMap m_entries;
};

}

#endif
