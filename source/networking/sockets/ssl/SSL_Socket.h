#ifndef SSL_SOCKET_H
#define SSL_SOCKET_H

#include "TCP_Socket.h"
#include <deque>
#include <openssl/ssl.h>

namespace networking
{

namespace sockets
{

namespace ssl
{

/**
 * This class implements a C++ wrapper for Open-SSL sockets
 */
class SSL_Socket
: virtual private attributes::abstract::Reflective,
  public TCP_Socket
{
private:

    /**
     * Constructor
     * @param pURL a pointer to a URL object
     */
    EXPORT_STEM SSL_Socket(URL *pURL);

    /**
     * Copy constructor
     */
    EXPORT_STEM SSL_Socket(const SSL_Socket &socket) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM SSL_Socket(SSL_Socket &&socket) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~SSL_Socket(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM SSL_Socket &operator = (const SSL_Socket &socket) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM SSL_Socket &operator = (SSL_Socket &&socket) = delete;

public:

    /**
     * Function to perform a DNS request and establish a connection to the host
     */
    EXPORT_STEM virtual bool connect(void) override;

    /**
     * create() function
     * @param pURL a pointer to a URL object
     */
    static EXPORT_STEM SSL_Socket *create(URL *pURL);

    /**
     * Function to disconnect from the host and destroy the socket
     */
    EXPORT_STEM virtual bool disconnect(void) override;

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "SSL_Socket";
    }

private:

    /**
     * Get a pointer to SSL context
     */
    inline virtual SSL_CTX *getContext(void) const final
    {
        return m_pContext;
    }

public:

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get a pointer to SSL handle
     */
    inline virtual SSL *getHandle(void) const final
    {
        return m_pHandle;
    }

    /**
     * Determine the last error
     * @param result the integer return value of the associated preceding function call which prompted the
     *               error
     */
    EXPORT_STEM virtual int getLastError(int result) const override;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Function to query whether this socket has been initialized
     */
    EXPORT_STEM virtual bool initialized(void) const override;

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
     *                receive() function the type of message reception to be performed
     */
    EXPORT_STEM virtual long read(char *pBuffer,
                                  size_t length,
                                  int flags = 0) final;

    /**
     * Function to write data to the socket
     * @param data a buffer containing the character sequence to be written to the socket
     */
    EXPORT_STEM virtual bool write(const std::string &data) override;

private:

    /**
     * OpenSSL library initialization handler
     */
    struct openssl_init_handler
    {
        /**
         * Constructor
         */
        EXPORT_STEM openssl_init_handler(void);

        /**
         * Destructor
         */
        EXPORT_STEM ~openssl_init_handler(void);
    };

    /**
     * OpenSSL initialization handler
     */
    static openssl_init_handler m_initializationHandler;

    /**
     * pointer to SSL context
     */
    SSL_CTX *m_pContext;

    /**
     * contains data that has been 'sniffed', but not consumed from the stream
     */
    std::deque<char> m_peekBuffer;

    /**
     * pointer to SSL handle
     */
    SSL *m_pHandle;
};

}

}

}

#endif
