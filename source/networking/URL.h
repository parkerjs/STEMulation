#ifndef URL_H
#define URL_H

#include "cloneable.h"
#include "initializable.h"
#include "streamable.h"
#include "URL_Field.h"
#include <string>
#include <vector>

namespace networking
{

/**
 * This class facilitates the storage and manipulation of the various components of URL web addresses
 */
class URL
: public attributes::interfaces::Cloneable<URL>,
  public attributes::interfaces::Initializable,
  public attributes::concrete::Streamable<URL>
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM URL(void);

    /**
     * Constructor
     * @param url the web address to be parsed into components and stored within this object
     */
    EXPORT_STEM URL(const std::string &url);

    /**
     * Copy constructor
     */
    EXPORT_STEM URL(const URL &url);

    /**
     * Move constructor
     */
    EXPORT_STEM URL(URL &&url);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~URL(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM URL &operator = (const URL &url);

    /**
     * Move assignment operator
     */
    EXPORT_STEM URL &operator = (URL &&url);

private:

    /**
     * Function to convert ASCII to hexadecimal character
     */
    static EXPORT_STEM char asciiToHexadecimal(char ascii);

public:

    /**
     * Clear all or part of the fields that comprise the URL represented by this object
     * @param fields a bit-field mask that specifies the desired fields to be cleared; the individual bits are
     *               defined in the URL_Field enumeration
     */
    EXPORT_STEM virtual void clear(short fields = URL_Field::Fragment |
                                                  URL_Field::Host |
                                                  URL_Field::Path |
                                                  URL_Field::Port |
                                                  URL_Field::Query |
                                                  URL_Field::Scheme |
                                                  URL_Field::UserNameAndUserPassword) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual URL *clone(void) const override;

    /**
     * Remove URL-encoding from the specified URL
     */
    static EXPORT_STEM std::string decode(const std::string &url);

    /**
     * Test for empty object
     */
    EXPORT_STEM virtual bool empty(void) const final;

    /**
     * Apply URL-encoding to the specified URL
     */
    static EXPORT_STEM std::string encode(const std::string &url);

    /**
     * Query whether or not URL encoding is enabled/disabled for the URL represented by this object
     */
    inline virtual bool encode(void) const final
    {
        return m_bEncodeURL;
    }

    /**
     * Function to enable/disable URL encoding for the URL represented by this object
     */
    inline virtual void encode(bool bEnabled) final
    {
        m_bEncodeURL = bEnabled;
    }

    /**
     * Input stream extraction function
     * @param stream a reference to an std::istream object
     */
    EXPORT_STEM virtual std::istream &extract(std::istream &stream) override;

    /**
     * Get the authority string
     */
    EXPORT_STEM virtual std::string getAuthority(void) const final;

    /**
     * Get the fragment identifier
     */
    inline virtual std::string getFragment(void) const final
    {
        return m_fragment;
    }

    /**
     * Get the host name
     */
    inline virtual std::string getHost(void) const final
    {
        return m_host;
    }

    /**
     * Get the path
     */
    inline virtual std::string getPath(void) const final
    {
        return m_path;
    }

    /**
     * Get the port number
     */
    inline virtual std::string getPort(void) const final
    {
        return m_port;
    }

    /**
     * Get the query string
     */
    EXPORT_STEM virtual std::string getQuery(void) const final;

    /**
     * Get a query parameter
     */
    EXPORT_STEM virtual std::string getQueryParameter(const std::string &parameter) const final;

    /**
     * Get the query parameter pairs vector
     */
    inline virtual std::vector<std::pair<std::string, std::string>> &getQueryParameterPairs(void) final
    {
        return m_queryParameterPairs;
    }

    /**
     * Get this object's request URI
     */
    EXPORT_STEM virtual std::string getRequestURI(void) const final;

    /**
     * Get the scheme
     */
    inline virtual std::string getScheme(void) const final
    {
        return m_scheme;
    }

    /**
     * Get all or part of the URL represented by this object
     * @param fields a bit-field mask that specifies the desired fields to be included in the URL returned by
     *               this function; the individual bits are defined in the URL_Field enumeration
     */
    EXPORT_STEM virtual std::string getURL(short fields = URL_Field::Fragment |
                                                          URL_Field::Host |
                                                          URL_Field::Path |
                                                          URL_Field::Port |
                                                          URL_Field::Query |
                                                          URL_Field::Scheme |
                                                          URL_Field::UserNameAndUserPassword) const final;

    /**
     * Get the user info string
     */
    EXPORT_STEM virtual std::string getUserInfo(void) const final;

    /**
     * Get the user name
     */
    inline virtual std::string getUserName(void) const final
    {
        return m_userName;
    }

    /**
     * Get the user password
     */
    inline virtual std::string getUserPassword(void) const final
    {
        return m_userPassword;
    }

private:

    /**
     * Function to convert a hexadecimal character to ascii
     */
    static EXPORT_STEM char hexadecimalToAscii(char hex);

public:

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Output stream print function
     * @param stream a reference to an std::ostream object
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;

    /**
     * Remove a query parameter
     */
    EXPORT_STEM virtual bool removeQueryParameter(const std::string &parameter) final;

    /**
     * Set the fragment identifier
     */
    inline virtual void setFragment(const std::string &fragment) final
    {
        m_fragment = fragment;
    }

    /**
     * Set the host name
     */
    inline virtual void setHost(const std::string &host) final
    {
        m_host = host;
    }

    /**
     * Set the path
     */
    inline virtual void setPath(const std::string &path) final
    {
        m_path = path;
    }

    /**
     * Set the port number
     */
    inline virtual void setPort(const std::string &port) final
    {
        m_port = port;
    }

    /**
     * Set the query string
     */
    EXPORT_STEM virtual void setQuery(const std::string &query) final;

    /**
     * Set a query parameter; returns true upon success
     */
    EXPORT_STEM virtual bool setQueryParameter(const std::string &parameter,
                                               const std::string &value = "") final;

    /**
     * Set the query parameter pairs vector
     */
    inline virtual void
    setQueryParameterPairs(const std::vector<std::pair<std::string, std::string>> &queryParameterPairs) final
    {
        m_queryParameterPairs = queryParameterPairs;
    }

    /**
     * Set the scheme
     */
    inline virtual void setScheme(const std::string &scheme) final
    {
        m_scheme = scheme;
    }

    /**
     * Set this object's URL
     * @param url the web address to be parsed into components and stored within this object
     */
    EXPORT_STEM virtual void setURL(const std::string &url) final;

    /**
     * Set the user name
     */
    inline virtual void setUserName(const std::string &userName) final
    {
        m_userName = userName;
    }

    /**
     * Set the user password
     */
    inline virtual void setUserPassword(const std::string &userPassword) final
    {
        m_userPassword = userPassword;
    }

protected:

    /**
     * flag to indicate that URL will be encoded
     */
    bool m_bEncodeURL;

    /**
     * fragment identifier
     */
    std::string m_fragment;

    /**
     * host name
     */
    std::string m_host;

    /**
     * path component
     */
    std::string m_path;

    /**
     * port number
     */
    std::string m_port;

    /**
     * map of query parameter pairs
     */
    std::vector<std::pair<std::string, std::string>> m_queryParameterPairs;

    /**
     * protocol scheme
     */
    std::string m_scheme;

    /**
     * string containing user name
     */
    std::string m_userName;

    /**
     * string containing user password
     */
    std::string m_userPassword;
};

}

#endif
