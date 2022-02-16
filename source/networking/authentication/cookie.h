#ifndef COOKIE_H
#define COOKIE_H

#include "cloneable.h"
#include "reflective.h"
#include "setupable.h"
#include "static_loggable.h"
#include "static_logging_streamable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "toggleable_stream.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#include <memory>
#include <time.h>

namespace networking
{

// forward declarations
class Downloader;
class DownloaderPreferences;
class HttpHeaders;

namespace authentication
{

/**
 * This class represents an HTTP cookie, and contains methods and members to manage
 */
class Cookie
: public attributes::interfaces::Cloneable<Cookie>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::concrete::StaticLoggable<Cookie>,
  public attributes::concrete::StaticLoggingStreamable<Cookie, std::string, utilities::ToggleableStream>,
  public attributes::concrete::StaticMutexMappable<Cookie, std::string, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Cookie>,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::concrete::VariableMapRegistrable
{
public:

    /**
     * Enumerations
     */
    enum class SameSiteEnum { Lax, Strict, Unknown };

private:

    /**
     * Constructor
     * @param pDownloader a pointer to a Downloader object which shall be used to download/refresh the cookie
     */
    EXPORT_STEM Cookie(std::shared_ptr<Downloader> pDownloader = nullptr);

    /**
     * Copy constructor
     */
    EXPORT_STEM Cookie(const Cookie &cookie);

    /**
     * Move constructor
     */
    EXPORT_STEM Cookie(Cookie &&cookie);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Cookie(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Cookie &operator = (const Cookie &cookie);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Cookie &operator = (Cookie &&cookie);

public:

    /**
     * Add a name-value pair to this cookie; returns true upon success
     */
    EXPORT_STEM virtual bool addNameValuePair(const std::string &name,
                                              const std::string &value) final;

    /**
     * clone() function
     */
    inline virtual Cookie *clone(void) const override
    {
        return new Cookie(*this);
    }

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     * @param pDownloader a pointer to a Downloader object which shall be used to download/refresh the cookie
     */
    static EXPORT_STEM Cookie *create(std::shared_ptr<Downloader> pDownloader = nullptr);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get domain name
     */
    inline virtual std::string getDomain(void) const final
    {
        return m_domain;
    }

    /**
     * Get pointer to downloader object
     */
    inline virtual std::shared_ptr<Downloader> getDownloader(void) const final
    {
        return m_pDownloader;
    }

    /**
     * Get expiration date and time
     */
    inline virtual struct tm &getExpiration(void) final
    {
        return m_expiration;
    }

    /**
     * Get max-age
     */
    inline virtual long getMaxAge(void) const final
    {
        return m_maxAge;
    }

    /**
     * Get cookie name-value pairs
     */
    inline virtual std::map<std::string, std::string> &getNameValuePairs(void) final
    {
        return m_nameValuePairs;
    }

    /**
     * Get path directive
     */
    inline virtual std::string getPath(void) const final
    {
        return m_path;
    }

    /**
     * Get SameSite directive
     */
    inline virtual SameSiteEnum getSameSite(void) const final
    {
        return m_sameSite;
    }

    /**
     * Get the value associated with the specified name; return a non-empty string upon success
     */
    EXPORT_STEM virtual std::string getValue(const std::string &name) const final;

    /**
     * Query httponly directive
     */
    inline virtual bool httpOnly(void) const final
    {
        return m_bHttpOnly;
    }

    /**
     * Enable/disable httponly directive
     */
    inline virtual void httpOnly(bool bEnabled) final
    {
        m_bHttpOnly = bEnabled;
    }

    /**
     * Read cookie information from HTTP header token map; returns true if cookie was successfully read or
     * false if an error occurred or no cookie information was present
     * @param pHttpResponseHeaders a pointer to an HttpHeaders container, which upon success will be populated
     *                             with the HTTP response headers
     */
    EXPORT_STEM virtual bool read(HttpHeaders *pHttpResponseHeaders);

    /**
     * Read cookie information from the specified string input; returns true if cookie was successfully read or
     * false if an error occurred or no cookie information was present
     * @param header the cookie directives from an HTTP response header
     */
    EXPORT_STEM virtual bool read(const std::string &header);

    /**
     * Request a cookie refresh from the server
     */
    EXPORT_STEM virtual bool refresh(void);

    /**
     * Query secure directive
     */
    inline virtual bool secure(void) const final
    {
        return m_bSecure;
    }

    /**
     * Enable/disable secure directive
     */
    inline virtual void secure(bool bEnabled) final
    {
        m_bSecure = bEnabled;
    }

    /**
     * Set domain
     */
    inline virtual void setDomain(const std::string &domain)
    {
        m_domain = domain;
    }

    /**
     * Set pointer to downloader object (object is cloned)
     */
    inline virtual void setDownloader(std::shared_ptr<Downloader> pDownloader) final
    {
        m_pDownloader = pDownloader;
    }

    /**
     * Set pointer to downloader object (object is moved into a shared_ptr)
     */
    EXPORT_STEM virtual void setDownloader(Downloader *&&pDownloader) final;

    /**
     * Set expiration date and time
     */
    inline virtual void setExpiration(struct tm &expiration) final
    {
        m_expiration = expiration;
    }

    /**
     * Set max-age
     */
    inline virtual void setMaxAge(long maxAge) final
    {
        m_maxAge = maxAge;
    }

    /**
     * Set cookie name
     */
    inline virtual void setNameValuePairs(const std::map<std::string, std::string> &nameValuePairs) final
    {
        m_nameValuePairs = nameValuePairs;
    }

    /**
     * Set path directive
     */
    inline virtual void setPath(const std::string &path) final
    {
        m_path = path;
    }

    /**
     * Set SameSite directive
     */
    inline virtual void setSameSite(const SameSiteEnum &value) final
    {
        m_sameSite = value;
    }

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Set the value associated with the specified name; returns true if the value is non-empty and the name
     * for which the value will be associated exists in this object's map of name-value pairs
     */
    EXPORT_STEM virtual bool setValue(const std::string &name,
                                      const std::string &value) final;

    /**
     * Write (append) cookie to the specified header string
     */
    EXPORT_STEM virtual bool write(std::string &header) const final;

private:

    /**
     * httponly directive
     */
    bool m_bHttpOnly;

    /**
     * secure directive
     */
    bool m_bSecure;

    /**
     * domain name directive
     */
    std::string m_domain;

    /**
     * expiration time directive
     */
    struct tm m_expiration;

    /**
     * max-age directive
     */
    long m_maxAge;

    /**
     * cookie name-value pairs
     */
    std::map<std::string, std::string> m_nameValuePairs;

    /**
     * path directive
     */
    std::string m_path;

    /**
     * pointer to a Downloader object
     */
    std::shared_ptr<Downloader> m_pDownloader;

    /**
     * same site directive
     */
    SameSiteEnum m_sameSite;
};

}

}

#endif
