#include "signal_slot.h"
#include "socket.h"
#include "static_message_dispatcher.h"
#include "URL.h"
#include <mutex>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <winsock.h>
#elif defined POSIX
#include <netdb.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#endif

#if defined POSIX
#define INVALID_SOCKET -1
#endif

// file-scoped variables
static constexpr char className[] = "Socket";

// using namespace declarations
using namespace attributes::concrete;
using namespace messaging;
using namespace utilities;

namespace networking
{

namespace sockets
{

// register static subscribers...
static StaticMessageRecipientRegistrar staticMessageRegistrar(
{ std::make_pair("StaticLoggable::setLoggingStreamMap<std::string, ToggleableStream>",
                 &Socket::setLoggingStreamMap<std::string, ToggleableStream>),
  std::make_pair("StaticSynchronizable::setMutexMap<std::string, std::mutex *>",
                 &Socket::setMutexMap<std::string, std::mutex *>)});

/**
 * Constructor
 * @param pURL a pointer to a URL object
 */
Socket::Socket(URL *pURL)
: m_bBlockingEnabled(true),
  m_bKeepAlive(false),
  m_connectRetryTimeout(100),
  m_pURL(pURL),
  m_receiveTimeout(1000),
  m_sendTimeout(1000),
  m_sockfd(int(INVALID_SOCKET))
{

}

/**
 * Destructor
 */
Socket::~Socket(void)
{
    // need to call disconnect() in sub-classes
}

/**
 * Query whether or not socket blocking mode is enabled
 */
bool Socket::blockingEnabled(void) const
{
    return m_bBlockingEnabled;
}

/**
 * Enable/disable socket blocking mode
 */
inline void Socket::blockingEnabled(bool bEnabled)
{
    if (initialized())
    {
#ifdef _WIN32
        // zero value specifies blocking mode enabled, one disables blocking...
        unsigned long flags = (bEnabled ? 0 : 1);
        int result = ioctlsocket(m_sockfd, FIONBIO, &flags);
#elif defined POSIX
        int result = fcntl(m_sockfd, F_GETFL, 0);
        if (result >= 0)
        {
            int flags = bEnabled ? result &~ O_NONBLOCK : result | O_NONBLOCK;
            result = fcntl(m_sockfd, F_SETFL, flags);
        }
#endif
        if (result < 0)
        {
            lock("std_out_mutex");
            perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
            unlock("std_out_mutex");
        }
        else
            m_bBlockingEnabled = bEnabled; // succeeded
    }
}

/**
 * create() function
 * @param pURL a pointer to a URL object
 */
Socket *Socket::create(URL *pURL)
{
    Socket *pSocket = nullptr;
    if (pURL != nullptr)
    {
        auto &&scheme = pURL->getScheme();
        if (scheme.empty())
            scheme = "http"; // assume http

        pSocket = FactoryConstructible<Socket>::create(scheme, pURL);
    }

    return pSocket;
}

/**
 * Get the name of this class
 */
std::string Socket::getClassName(void) const
{
    return className;
}

/**
 * Get connection retry (on failed attempt) timeout in milliseconds
 */
std::size_t Socket::getConnectRetryTimeout(void) const
{
    return m_connectRetryTimeout;
}

/**
 * Get the host name
 */
std::string Socket::getHost(void) const
{
    return m_pURL != nullptr ? m_pURL->getHost() : "";
}

/**
 * Get the port
 */
std::string Socket::getPort(void) const
{
    std::string port;
    if (m_pURL != nullptr)
    {
        port = m_pURL->getPort();
        if (port.empty())
        {
            auto &&scheme = m_pURL->getScheme();
            if (scheme.empty())
                scheme = "http"; // assume http

            auto *pServiceEntry = getservbyname(scheme.c_str(), nullptr);
            if (pServiceEntry != nullptr)
                port = std::to_string(ntohs(pServiceEntry->s_port));
        }
    }

    return port;
}

/**
 * Get socket receive timeout value in milliseconds
 */
std::size_t Socket::getServerReceiveTimeout(void) const
{
    return m_receiveTimeout;
}

/**
 * Get socket send timeout value in milliseconds
 */
std::size_t Socket::getServerSendTimeout(void) const
{
    return m_sendTimeout;
}

/**
 * Query whether or not socket keep-alive is enabled
 */
bool Socket::keepAlive(void) const
{
    return m_bKeepAlive;
}

/**
 * Enable/disable socket keep-alive
 */
void Socket::keepAlive(bool bEnabled)
{
    if (initialized())
    {
        // set the option to keep the socket alive
        int optval = (bEnabled ? 1 : 0);
        if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval)) >= 0)
            m_bKeepAlive = bEnabled;
        else
        {
            lock("std_out_mutex");
            perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
            unlock("std_out_mutex");
        }
    }
}

/**
 * Read a line of data (until CRLF)
 * @param pData a pointer to a buffer in which the received data will be stored
 */
bool Socket::readLine(std::string *pData)
{
    bool bSuccess = false;
    if (initialized())
    {
        long result = 0;
        while (true)
        {
            char byte;
            result = read(&byte, 1, 0);
            if (result <= 0)
                break;
            else
            {
                if (byte == '\r')
                {
                    result = read(&byte, 1, MSG_PEEK);
                    if (result > 0 && byte == '\n')
                    {
                        read(&byte, 1, 0);
                        bSuccess = true;
                        break;
                    }
                }

                if (pData != nullptr)
                    *pData += byte;
            }
        }
    }

    return bSuccess;
}

/**
 * Set connection retry (on failed attempt) timeout in milliseconds
 */
void Socket::setConnectRetryTimeout(std::size_t timeout)
{
    m_connectRetryTimeout = timeout;
}

/**
 * Set socket receive timeout value in milliseconds
 */
void Socket::setServerReceiveTimeout(std::size_t timeout)
{
    m_receiveTimeout = setTimeout(timeout, ConnectionTimeoutType::RECEIVE);
}

/**
 * Set socket send timeout value in milliseconds
 */
void Socket::setServerSendTimeout(std::size_t timeout)
{
    m_sendTimeout = setTimeout(timeout, ConnectionTimeoutType::SEND);
}

/**
 * Function to set the connection send/receive timeout in milliseconds
 */
long Socket::setTimeout(std::size_t timeout,
                        const ConnectionTimeoutType &type)
{
    bool bSuccess = initialized();
    if (bSuccess)
    {
        bSuccess = (m_bBlockingEnabled || timeout == 0);
        if (bSuccess)
        {

#ifdef _WIN32
            DWORD tv = static_cast<DWORD>(timeout);
#elif defined POSIX
            struct timeval tv;
            memset(&tv, 0, sizeof(timeval));
            tv.tv_sec = static_cast<int>(timeout / 1E3);
            tv.tv_usec = static_cast<int>((timeout - 1E3 * tv.tv_sec) * 1E3);
#endif
            int result = -1;
            switch (type)
            {
                case ConnectionTimeoutType::RECEIVE:
                result = setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
                break;

                case ConnectionTimeoutType::SEND:
                result = setsockopt(m_sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
                break;

                default:
                bSuccess = false;
                break;
            }

            if (bSuccess)
            {
                bSuccess = (result >= 0);
                if (!bSuccess)
                {
                    lock("std_out_mutex");
                    perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
                    unlock("std_out_mutex");
                }
            }
        }
        else
        {
            timeout = 0;
            lock("std_out_mutex");
            logMsg("debug", LoggingLevel::Enum::Debug,
                   "Reseting timeout to 0 in non-blocking mode.\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }
    }

    return timeout;
}

}

}
