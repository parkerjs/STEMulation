#include "SSL_Socket.h"
#include <algorithm>
#include <mutex>
#include <openssl/err.h>
#include <thread>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <winsock.h>
#elif defined POSIX
#include <netdb.h>
#endif

#if defined POSIX
#define INVALID_SOCKET -1
#endif

// file-scoped variables
static constexpr char factoryName[] = "https";

// using namespace declarations
using namespace attributes::abstract;
using namespace utilities;

namespace networking
{

namespace sockets
{

namespace ssl
{

// register factories...
static FactoryRegistrar<Socket> ssl_socket_factory(factoryName, &SSL_Socket::create);

// static member initializations
SSL_Socket::openssl_init_handler SSL_Socket::m_initializationHandler;

/**
 * Constructor
 */
SSL_Socket::openssl_init_handler::openssl_init_handler(void)
{
    SSL_library_init();
    SSL_load_error_strings();
    FIPS_mode_set(1);
}

/**
 * Destructor
 */
SSL_Socket::openssl_init_handler::~openssl_init_handler(void)
{
    FIPS_mode_set(0);
    CRYPTO_set_locking_callback(nullptr);
    CRYPTO_set_id_callback(nullptr);
    ERR_free_strings();
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
}

/**
 * Constructor
 * @param pURL a pointer to a URL object
 */
SSL_Socket::SSL_Socket(URL *pURL)
: TCP_Socket(pURL),
  m_pContext(nullptr),
  m_pHandle(nullptr)
{

}

/**
 * Destructor
 */
SSL_Socket::~SSL_Socket(void)
{
    disconnect();
}

/**
 * Function to perform a DNS request and establish a connection to the host
 */
bool SSL_Socket::connect(void)
{
    bool bSuccess = TCP_Socket::connect();
    if (bSuccess)
    {
        // finally do the SSL handshake
        for (int result = SSL_connect(m_pHandle); result != 1; result = SSL_connect(m_pHandle))
        {
            switch (getLastError(result))
            {
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                std::this_thread::sleep_for(std::chrono::milliseconds(m_connectRetryTimeout));
                break;

                default:
                SSL_free(m_pHandle);
                SSL_CTX_free(m_pContext);
                m_pHandle = nullptr;
                m_pContext = nullptr;
                break;
            }
        }
    }

    return bSuccess;
}

/**
 * create() function
 * @param pURL a pointer to a URL object
 */
SSL_Socket *SSL_Socket::create(URL *pURL)
{
    return new SSL_Socket(pURL);
}

/**
 * Function to disconnect from the host and destroy the socket
 */
bool SSL_Socket::disconnect(void)
{
    if (m_pHandle != nullptr)
    {
        SSL_shutdown(m_pHandle);
        SSL_free(m_pHandle);
        m_pHandle = nullptr;
    }

    if (m_pContext != nullptr)
    {
        SSL_CTX_free(m_pContext);
        m_pContext = nullptr;
    }

    return TCP_Socket::disconnect();
}

/**
 * Get the factory name of this constructible
 */
std::string SSL_Socket::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Determine the last error
 * @param result the integer return value of the associated preceding function call which prompted the error
 */
int SSL_Socket::getLastError(int result) const
{
    auto &&get_ssl_error = [] { return std::string(ERR_error_string(0, nullptr)); };
    auto &&error = SSL_get_error(m_pHandle, result);
    switch (error)
    {
        case SSL_ERROR_NONE:
        // do nothing
        break;

        case SSL_ERROR_WANT_ACCEPT:
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Error connecting socket: " + get_ssl_error() + "\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        case SSL_ERROR_WANT_CONNECT:
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Error in SSL handshake: " + get_ssl_error() + "\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        case SSL_ERROR_WANT_READ:
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Error reading from socket: " + get_ssl_error() + "\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        case SSL_ERROR_WANT_WRITE:
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Error writing to socket: " + get_ssl_error() + "\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        case SSL_ERROR_ZERO_RETURN: // the socket has been closed on the other end
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "The socket disconnected: " + get_ssl_error() + "\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        default:
        // TODO: handle other cases...
        break;
    }

    return error;
}

/**
 * Initialization function
 */
bool SSL_Socket::initialize(void)
{
    bool bSuccess = TCP_Socket::initialize();
    if (bSuccess)
    {
        auto &&get_ssl_error = [] { return std::string(ERR_error_string(0, nullptr)); };
        if (m_pContext == nullptr)
            m_pContext = SSL_CTX_new(TLSv1_client_method());

        if (m_pContext == nullptr)
        {
            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "Unable to create SSL context: " + get_ssl_error() + "\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }

        // create an SSL handle that we will use for reading and writing
        if (m_pHandle == nullptr)
            m_pHandle = SSL_new(m_pContext);

        if (m_pHandle == nullptr)
        {
            SSL_CTX_free(m_pContext);
            m_pContext = nullptr;

            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "Unable to create SSL handle: " + get_ssl_error() + "\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }

        // pair the SSL handle with the plain socket
        if (SSL_set_fd(m_pHandle, m_sockfd) == 0)
        {
            SSL_free(m_pHandle);
            SSL_CTX_free(m_pContext);
            m_pHandle = nullptr;
            m_pContext = nullptr;

            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "Unable to associate SSL and plain socket: " + get_ssl_error() + "\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }

        m_peekBuffer.clear();
    }

    return bSuccess;
}

/**
 * Function to query whether this socket has been initialized
 */
bool SSL_Socket::initialized(void) const
{
    bool bSuccess = TCP_Socket::initialized();
    if (bSuccess)
        bSuccess = (m_pContext != nullptr && m_pHandle != nullptr);

    return bSuccess;
}

/**
 * Function to peek at the incoming data stream and store it in the buffer without removing sequence from the
 * input queue
 * @param pBuffer a pointer to a buffer in which the data will be received
 * @param length  the size of the buffer
 */
long SSL_Socket::peek(char *pBuffer,
                      size_t length)
{
    return read(pBuffer, length, MSG_PEEK);
}

/**
 * Function to read data from the socket
 * @param pBuffer a pointer to a buffer in which the data will be received
 * @param length  the size of the buffer
 * @param flags   an integer composed of logically or'd bit flags which dictates to the underlying receiving
 *                function the type of message reception to be performed
 */
long SSL_Socket::read(char *pBuffer,
                      size_t length,
                      int flags)
{
    long result = 0;
    if (pBuffer != nullptr && initialized())
    {
        // read data from the peek buffer, if there is any
        auto &&size = std::min(length, m_peekBuffer.size());
        for (size_t i = 0; i < size; ++i, ++pBuffer)
        {
            if (flags & MSG_PEEK)
            {
                // we're requesting a peek, so do not consume what is currently in the buffer
                *pBuffer = m_peekBuffer[i];
            }
            else
            {
                // we're not requesting a peek, so consume what is currently in the buffer
                *pBuffer = m_peekBuffer.front();
                m_peekBuffer.pop_front();
            }
        }

        result += size;
        length -= size;
        if (length > 0)
        {
            result = SSL_read(m_pHandle, pBuffer, length);
            if (result > 0)
            {
                if (flags & MSG_PEEK)
                {
                    for (int i = 0; i < result; ++i)
                        m_peekBuffer.push_back(*pBuffer++);
                }

                result += size;
            }
            else if ((flags & MSG_PEEK) == 0)
            {
                switch (getLastError(result))
                {
                    case SSL_ERROR_ZERO_RETURN: // the socket has been closed on the other end
                    disconnect();
                    result = 0;
                    break;

                    case SSL_ERROR_WANT_READ:
                    case SSL_ERROR_WANT_WRITE:
                    result = 0; // read nothing
                    break;

                    default:
                    // do nothing
                    break;
                }

                if (size > 0)
                {
                    // if data was read from the peek buffer, set this as the return value
                    result = size;
                }
            }
        }
    }

    return result;
}

/**
 * Function to write data to the socket
 * @param data a buffer containing the character sequence to be written to the socket
 */
bool SSL_Socket::write(const std::string &data)
{
    bool bSuccess = false;
    if (initialized())
    {
        auto &&result = SSL_write(m_pHandle, data.c_str(), int(data.length()));
        bSuccess = (result > 0);
        if (!bSuccess)
        {
            switch (getLastError(result))
            {
                case SSL_ERROR_ZERO_RETURN: // the socket has been closed on the other end
                disconnect();
                break;

                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                std::this_thread::sleep_for(std::chrono::milliseconds(m_sendTimeout));
                break;

                default:
                // do nothing
                break;
            }
        }
    }

    return bSuccess;
}

}

}

}
