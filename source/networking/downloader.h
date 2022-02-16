#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "cloneable.h"
#include "dependency_injectable.h"
#include "httpRequestMethod.h"
#include "reflective.h"
#include "setupable.h"
#include "static_loggable.h"
#include "static_logging_streamable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "toggleable_stream.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#include <cstdlib>
#include <string>

namespace networking
{

// forward declarations
class DownloaderPreferences;
class HttpHeaders;
namespace sockets { class Socket; }
class URL;

/**
 * This class serves as an abstract base for classes that download data. The default implementation of the
 * download() function makes use of sockets to retrieve the data.
 */
class Downloader
: public attributes::interfaces::Cloneable<Downloader>,
  virtual public attributes::concrete::DependencyInjectable<DownloaderPreferences *>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::concrete::StaticLoggable<Downloader>,
  public attributes::concrete::StaticLoggingStreamable<Downloader, std::string,
                                                       utilities::ToggleableStream>,
  public attributes::concrete::StaticMutexMappable<Downloader, std::string, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Downloader>,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::concrete::VariableMapRegistrable
{
protected:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     * @param url          the web address from which the resource(s) will be downloaded
     */
    EXPORT_STEM Downloader(const tDependencies &dependencies,
                           const std::string &url = "");

    /**
     * Copy constructor
     */
    EXPORT_STEM Downloader(const Downloader &downloader);

    /**
     * Move constructor
     */
    EXPORT_STEM Downloader(Downloader &&downloader);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Downloader(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Downloader &operator = (const Downloader &downloader);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Downloader &operator = (Downloader &&downloader);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual Downloader *clone(void) const override;

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     * @param dependencies a tuple of this object's required injection dependencies
     * @param url          the web address from which the resource(s) will be downloaded
     */
    static EXPORT_STEM Downloader *create(const tDependencies &dependencies,
                                          const std::string &url = "");

protected:

    /**
     * Function to create a socket
     */
    EXPORT_STEM virtual sockets::Socket *createSocket(void) final;

public:

    /**
     * Default function implementation to download data from a server. Function returns true if data was
     * successfully read from the server
     * @param data an std::string that will be populated with bytes read from the server.
     */
    EXPORT_STEM virtual bool download(std::string &data);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the full HTTP request string
     */
    EXPORT_STEM virtual std::string getHttpRequest(void) const final;

    /**
     * Get this object's HTTP request headers
     */
    EXPORT_STEM virtual HttpHeaders *getHttpRequesteHeaders(void) final;

    /**
     * Get this object's HTTP response headers
     */
    EXPORT_STEM virtual HttpHeaders *getHttpResponseHeaders(void);

    /**
     * Get the HTTP request method
     */
    inline HttpRequestMethod getHttpRequestMethod(void) const
    {
        return m_httpRequestMethod;
    }

    /**
     * Get the port
     */
    EXPORT_STEM virtual std::string getPort(void) const final;

    /**
     * Get socket file descriptor
     */
    inline virtual sockets::Socket *getSocket(void) const final
    {
        return m_pSocket;
    }

    /**
     * Get a pointer to this object's URL
     */
    inline virtual URL *getURL(void)
    {
        return m_pURL;
    }

protected:

    /**
     * Function to send request to the server, if connected. Upon success, returns true.
     */
    EXPORT_STEM virtual bool request(void);

public:

    /**
     * Set the HTTP request method
     */
    inline void setHttpRequestMethod(const HttpRequestMethod &httpRequestMethod)
    {
        m_httpRequestMethod = httpRequestMethod;
    }

    /**
     * Set the port
     */
    EXPORT_STEM virtual void setPort(const std::string &port);

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

protected:

    /**
     * specifies the HTTP request method (GET vs POST
     */
    HttpRequestMethod m_httpRequestMethod;

    /**
     * pointer to an object containing HTTP request headers
     */
    HttpHeaders *m_pHttpRequestHeaders;

    /**
     * pointer to an object containing HTTP response headers
     */
    HttpHeaders *m_pHttpResponseHeaders;

    /**
     * pointer to a socket object
     */
    sockets::Socket *m_pSocket;

    /**
     * pointer to a URL object
     */
    URL *m_pURL;

};

}

#endif
