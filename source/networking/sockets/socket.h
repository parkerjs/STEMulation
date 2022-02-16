#ifndef SOCKET_H
#define SOCKET_H

#include "factory_constructible.h"
#include "initializable.h"
#include "reflective.h"
#include "static_loggable.h"
#include "static_logging_streamable.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include "toggleable_stream.h"
#include <string>

namespace networking
{

// forward declarations
class URL;

namespace sockets
{

/**
 * This class implements a C++ abstract base wrapper class for network sockets
 */
class Socket
: public attributes::abstract::FactoryConstructible<Socket>,
  public attributes::interfaces::Initializable,
  virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticLoggable<Socket>,
  public attributes::concrete::StaticLoggingStreamable<Socket, std::string,
                                                       utilities::ToggleableStream>,
  public attributes::concrete::StaticMutexMappable<Socket, std::string, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<Socket>
{
private:

    /**
     * Enumerations
     */
    enum class ConnectionTimeoutType { RECEIVE, SEND };

protected:

    /**
     * Constructor
     * @param pURL a pointer to a URL object
     */
    EXPORT_STEM Socket(URL *pURL);

    /**
     * Copy constructor
     */
    EXPORT_STEM Socket(const Socket &socket) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM Socket(Socket &&socket) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Socket(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Socket &operator = (const Socket &socket) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM Socket &operator = (Socket &&socket) = delete;

public:

    /**
     * Query whether or not socket blocking mode is enabled
     */
    EXPORT_STEM virtual bool blockingEnabled(void) const final;

    /**
     * Enable/disable socket blocking mode
     */
    EXPORT_STEM virtual void blockingEnabled(bool bEnabled) final;

    /**
     * Function to perform a DNS request and establish a connection to the host
     */
    EXPORT_STEM virtual bool connect(void) = 0;

    /**
     * create() function
     * @param pURL a pointer to a URL object
     */
    static EXPORT_STEM Socket *create(URL *pURL);

    /**
     * Function to disconnect from the host and destroy the socket
     */
    EXPORT_STEM virtual bool disconnect(void) = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get connection retry (on failed attempt) timeout in milliseconds
     */
    EXPORT_STEM virtual std::size_t getConnectRetryTimeout(void) const final;

    /**
     * Get the host name
     */
    EXPORT_STEM virtual std::string getHost(void) const final;

    /**
     * Determine the last error
     * @param result the integer return value of the associated preceding function call which prompted the
     *               error
     */
    EXPORT_STEM virtual int getLastError(int result = 0) const = 0;

    /**
     * Get the port
     */
    EXPORT_STEM virtual std::string getPort(void) const final;

    /**
     * Get socket receive timeout value in milliseconds
     */
    EXPORT_STEM virtual std::size_t getServerReceiveTimeout(void) const final;

    /**
     * Get socket send timeout value in milliseconds
     */
    EXPORT_STEM virtual std::size_t getServerSendTimeout(void) const final;

    /**
     * Get a pointer to this object's URL
     */
    inline virtual URL *getURL(void)
    {
        return m_pURL;
    }

    /**
     * Query whether or not socket keep-alive is enabled
     */
    EXPORT_STEM virtual bool keepAlive(void) const final;

    /**
     * Enable/disable socket keep-alive
     */
    EXPORT_STEM virtual void keepAlive(bool bEnabled) final;

    /**
     * Function to query whether this socket has been initialized
     */
    EXPORT_STEM virtual bool initialized(void) const = 0;

    /**
     * Test this socket for open connection
     */
    EXPORT_STEM virtual bool isConnected(void) = 0;

    /**
     * Function to peek at the incoming data stream and store it in the buffer without removing sequence from
     * the input queue
     * @param pBuffer a pointer to a buffer in which the data will be received
     * @param length  the size of the buffer
     */
    EXPORT_STEM virtual long peek(char *pBuffer,
                                  std::size_t length) = 0;

    /**
     * Function to read data from the socket
     * @param pBuffer a pointer to a buffer in which the data will be received
     * @param length  the size of the buffer
     * @param flags   an integer composed of logically or'd bit flags which dictates to the underlying
     *                receiving function the type of message reception to be performed
     */
    EXPORT_STEM virtual long read(char *pBuffer,
                                  size_t length,
                                  int flags = 0) = 0;

    /**
     * Read a line of data (until CRLF)
     * @param pData a pointer to a buffer in which the received data will be stored
     */
    EXPORT_STEM virtual bool readLine(std::string *pData = nullptr);

    /**
     * Set connection retry (on failed attempt) timeout in milliseconds
     */
    EXPORT_STEM virtual void setConnectRetryTimeout(std::size_t timeout) final;

    /**
     * Set socket receive timeout value in milliseconds
     */
    EXPORT_STEM virtual void setServerReceiveTimeout(std::size_t timeout) final;

    /**
     * Set socket send timeout value in milliseconds
     */
    EXPORT_STEM virtual void setServerSendTimeout(std::size_t timeout) final;

private:

    /**
     * Function to set the connection send/receive timeout in milliseconds
     */
    EXPORT_STEM virtual long setTimeout(std::size_t timeout,
                                        const ConnectionTimeoutType &type) final;

public:

    /**
     * Function to write data to the socket
     * @param data a buffer containing the character sequence to be written to the socket
     */
    EXPORT_STEM virtual bool write(const std::string &data) = 0;

protected:

    /**
     * flag to indicate blocking mode enabled/disabled
     */
    bool m_bBlockingEnabled;

    /**
     * flag to indicate keep-alive enabled/disabled
     */
    bool m_bKeepAlive;

    /**
     * connection retry timeout in milliseconds
     */
    std::size_t m_connectRetryTimeout;

    /**
     * pointer to a URL object
     */
    URL *m_pURL;

    /**
     * receive timeout (milliseconds)
     */
    std::size_t m_receiveTimeout;

    /**
     * send timeout (milliseconds)
     */
    std::size_t m_sendTimeout;

    /**
     * socket file descriptor
     */
    int m_sockfd;
};

}

}

#endif
