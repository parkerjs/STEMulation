#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "socket.h"

// forward declarations
struct addrinfo;

namespace networking
{

namespace sockets
{

/**
 * This class implements a C++ wrapper for TCP/IP sockets
 */
class TCP_Socket
: virtual private attributes::abstract::Reflective,
  public Socket
{
protected:

    /**
     * Constructor
     * @param pURL a pointer to a URL object
     */
    EXPORT_STEM TCP_Socket(URL *pURL);

    /**
     * Copy constructor
     */
    EXPORT_STEM TCP_Socket(const TCP_Socket &socket) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM TCP_Socket(TCP_Socket &&socket) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~TCP_Socket(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM TCP_Socket &operator = (const TCP_Socket &socket) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM TCP_Socket &operator = (TCP_Socket &&socket) = delete;

public:

    /**
     * Function to perform a DNS request and establish a connection to the host
     */
    EXPORT_STEM virtual bool connect(void) override;

    /**
     * create() function
     * @param pURL a pointer to a URL object
     */
    static EXPORT_STEM TCP_Socket *create(URL *pURL);

    /**
     * Function to disconnect from the host and destroy the socket
     */
    EXPORT_STEM virtual bool disconnect(void) override;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "TCP_Socket";
    }

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

    /**
     * Determine the last error
     * @param result the integer return value of the associated preceding function call which prompted the
     *               error
     */
    EXPORT_STEM virtual int getLastError(int /* not used */ = 0) const override;

    /**
     * Get socket file descriptor
     */
    inline virtual long getSocketFileDescriptor(void) const final
    {
        return m_sockfd;
    }

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Function to query whether this socket has been initialized
     */
    EXPORT_STEM virtual bool initialized(void) const override;

    /**
     * Test this socket for open connection
     */
    EXPORT_STEM virtual bool isConnected(void) override;

    /**
     * Function to peek at the incoming data stream and store it in the buffer without removing sequence from
     * the input queue
     * @param pBuffer a pointer to a buffer in which the data will be received
     * @param length  the size of the buffer
     */
    EXPORT_STEM virtual long peek(char *pBuffer,
                                  size_t length) override;

    /**
     * Function to read data from the socket
     * @param pBuffer a pointer to a buffer in which the data will be received
     * @param length  the size of the buffer
     * @param flags   an integer composed of logically or'd bit flags which dictates to the underlying
     *                receiving function the type of message reception to be performed
     */
    EXPORT_STEM virtual long read(char *pBuffer,
                                  size_t length,
                                  int flags = 0) override;

    /**
     * Function to write data to the socket
     * @param data a buffer containing the character sequence to be written to the socket
     */
    EXPORT_STEM virtual bool write(const std::string &data) override;

protected:

    /**
     * contains current host name
     */
    std::string m_host;

    /**
     * pointer to addrinfo structure
     */
    addrinfo *m_pAddrInfo;
};

}

}

#endif
