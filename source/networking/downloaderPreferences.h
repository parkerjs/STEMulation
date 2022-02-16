#ifndef DOWNLOADER_PREFERENCES_H
#define DOWNLOADER_PREFERENCES_H

#include "dependency_injectable.h"
#include "loggable.h"
#include "priority_subscriber.h"
#include "reflective.h"
#include "setupable.h"
#include "toggleable_stream.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"

// forward declarations
namespace messaging { class PriorityPublisher; }

namespace networking
{

/**
 * This class manages socket downloader preferences
 */
class DownloaderPreferences
: public attributes::concrete::DependencyInjectable<messaging::PriorityPublisher *>,
  public attributes::concrete::Loggable<std::string, utilities::ToggleableStream>,
  public messaging::PrioritySubscriber<attributes::concrete::Loggable<std::string,
                                                                      utilities::ToggleableStream>,
                                       attributes::interfaces::Setupable,
                                       attributes::interfaces::TokenMapConfigurable>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::concrete::VariableMapRegistrable
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    EXPORT_STEM DownloaderPreferences(const tDependencies &dependencies);

private:

    /**
     * Copy constructor
     */
    EXPORT_STEM DownloaderPreferences(const DownloaderPreferences &preferences) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM DownloaderPreferences(DownloaderPreferences &&preferences) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~DownloaderPreferences(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM DownloaderPreferences &operator = (const DownloaderPreferences &preferences) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM DownloaderPreferences &operator = (DownloaderPreferences &&preferences) = delete;

public:

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     * @param dependencies a tuple of this object's required injection dependencies
     */
    static EXPORT_STEM DownloaderPreferences *create(const tDependencies &dependencies);

    /**
     * Get the name of this object
     */
    inline virtual std::string getClassName(void) const override
    {
        return "DownloaderPreferences";
    }

    /**
     * Get connection retry (on failed attempt) timeout in milliseconds
     */
    inline virtual std::size_t getConnectRetryTimeout(void) const final
    {
        return m_connectRetryTimeout;
    }

    /**
     * Set the HTTP version to be used
     */
    inline virtual std::string getHttpVersion(void) const final
    {
        return m_httpVersion;
    }

    /**
     * Get maximum number of connect retry attempts
     */
    inline virtual std::size_t getMaximumConnectRetryAttempts(void) const final
    {
        return m_maxConnectRetryAttempts;
    }

    /**
     * Get maximum number of receive retry attempts
     */
    inline virtual std::size_t getMaximumReceiveRetryAttempts(void) const final
    {
        return m_maxRecvRetryAttempts;
    }

    /**
     * Get maximum number of threads to use in the download
     */
    inline virtual long getMaximumThreads(void) const final
    {
        return m_maxThreads;
    }

    /**
     * Get receive buffer size
     */
    inline virtual std::size_t getReceiveBufferSize(void) const final
    {
        return m_receiveBufferSize;
    }

    /**
     * Get receive retry (on failed attempt) timeout in milliseconds
     */
    inline virtual std::size_t getReceiveRetryTimeout(void) const final
    {
        return m_receiveRetryTimeout;
    }

    /**
     * Get socket receive timeout value in milliseconds
     */
    inline virtual std::size_t getServerReceiveTimeout(void) const final
    {
        return m_receiveTimeout;
    }

    /**
     * Get socket send timeout value in milliseconds
     */
    inline virtual std::size_t getServerSendTimeout(void) const final
    {
        return m_sendTimeout;
    }

    /**
     * Query whether or not socket keep-alive is enabled
     */
    inline virtual bool keepSocketConnectionAlive(void) const final
    {
        return m_bKeepSocketConnectionAlive;
    }

    /**
     * Enable/disable socket keep-alive
     */
    inline virtual void keepSocketConnectionAlive(bool option) final
    {
        m_bKeepSocketConnectionAlive = option;
    }

    /**
     * Set connection retry (on failed attempt) timeout in milliseconds
     */
    inline virtual void setConnectRetryTimeout(std::size_t timeout) final
    {
        m_connectRetryTimeout = timeout;
    }

    /**
     * Set the HTTP version to be used
     */
    inline virtual void setHttpVersion(const std::string &httpVersion) final
    {
        m_httpVersion = httpVersion;
    }

    /**
     * Set maximum number of connect retry attempts
     */
    inline virtual void setMaximumConnectRetryAttempts(std::size_t maxConnectRetryAttempts) final
    {
        m_maxConnectRetryAttempts = maxConnectRetryAttempts;
    }

    /**
     * Set maximum number of receive retry attempts
     */
    inline virtual void setMaximumReceiveRetryAttempts(std::size_t maxReceiveRetryAttempts) final
    {
        m_maxRecvRetryAttempts = maxReceiveRetryAttempts;
    }

    /**
     * Set maximum number of threads to use in the download
     */
    inline virtual void setMaximumThreads(long maxThreads) final
    {
        m_maxThreads = maxThreads;
    }

    /**
     * Set receive buffer size
     */
    inline virtual void setReceiveBufferSize(std::size_t size) final
    {
        m_receiveBufferSize = size;
    }

    /**
     * Set receive retry (on failed attempt) timeout in milliseconds
     */
    inline virtual void setReceiveRetryTimeout(std::size_t timeout) final
    {
        m_receiveRetryTimeout = timeout;
    }

    /**
     * Set socket receive timeout value in milliseconds
     */
    inline virtual void setServerReceiveTimeout(std::size_t timeout) final
    {
        m_receiveTimeout = timeout;
    }

    /**
     * Set socket send timeout value in milliseconds
     */
    inline virtual void setServerSendTimeout(std::size_t timeout) final
    {
        m_sendTimeout = timeout;
    }

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Query whether or not socket blocking mode is enabled
     */
    inline virtual bool socketBlockingEnabled(void) const final
    {
        return m_bBlockingEnabled;
    }

    /**
     * Enable/disable socket blocking mode
     */
    inline virtual void socketBlockingEnabled(bool bEnabled) final
    {
        m_bBlockingEnabled = bEnabled;
    }

private:

    /**
     * flag to indicate socket blocking mode
     */
    bool m_bBlockingEnabled;

    /**
     * socket keep-alive option
     */
    bool m_bKeepSocketConnectionAlive;

    /**
     * connection retry timeout in milliseconds
     */
    std::size_t m_connectRetryTimeout;

    /**
     * specifies the HTTP version to be used
     */
    std::string m_httpVersion;

    /**
     * maximum number of attempts to reconnect
     */
    std::size_t m_maxConnectRetryAttempts;

    /**
     * maximum number of attempts to receive
     */
    std::size_t m_maxRecvRetryAttempts;

    /**
     * number of threads to use in the download
     */
    long m_maxThreads;

    /**
     * receive buffer size
     */
    std::size_t m_receiveBufferSize;

    /**
     * receive retry timeout in milliseconds
     */
    std::size_t m_receiveRetryTimeout;

    /**
     * receive timeout (milliseconds)
     */
    std::size_t m_receiveTimeout;

    /**
     * send timeout (milliseconds)
     */
    std::size_t m_sendTimeout;
};

}

#endif
