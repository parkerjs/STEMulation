#include "TCP_Socket.h"
#include "URL.h"
#include <mutex>
#include <thread>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <WS2tcpip.h>
#include <winsock.h>
#elif defined POSIX
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define _errno WSAGetLastError()
#define CONNRESET WSAECONNRESET
#define NOTCONN WSAENOTCONN
#define TIMEDOUT WSAETIMEDOUT
#define WOULDBLOCK WSAEWOULDBLOCK
#elif defined POSIX
#define closesocket close
#define CONNRESET ECONNRESET
#define _errno errno
#define INVALID_SOCKET -1
#define NOTCONN ENOTCONN
#define SOCKET_ERROR -1
#define TIMEDOUT ETIMEDOUT
#define WOULDBLOCK EWOULDBLOCK
#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

// file-scoped variables
static constexpr char factoryName[] = "http";

// using namespace declarations
using namespace attributes::abstract;
using namespace utilities;

namespace networking
{

namespace sockets
{

// register factories...
static FactoryRegistrar<Socket> tcp_socket_factory(factoryName, &TCP_Socket::create);

/**
 * Constructor
 * @param pURL a pointer to a URL object
 */
TCP_Socket::TCP_Socket(URL *pURL)
: Socket(pURL),
  m_pAddrInfo(nullptr)
{

}

/**
 * Destructor
 */
TCP_Socket::~TCP_Socket(void)
{
    disconnect();
}

/**
 * Function to perform a DNS request and establish a connection to the host
 */
bool TCP_Socket::connect(void)
{
    bool bSuccess = initialized();
    if (!bSuccess)
        bSuccess = initialize();

    if (bSuccess)
    {
        // socket was successfully created, now connect to the server
        bSuccess = (::connect(m_sockfd, m_pAddrInfo->ai_addr, int(m_pAddrInfo->ai_addrlen)) >= 0);
        if (!bSuccess)
        {
            lock("std_out_mutex");
            perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
            unlock("std_out_mutex");
        }
    }

    return bSuccess;
}

/**
 * create() function
 * @param pURL a pointer to a URL object
 */
TCP_Socket *TCP_Socket::create(URL *pURL)
{
    return new TCP_Socket(pURL);
}

/**
 * Function to disconnect from the host and destroy the socket
 */
bool TCP_Socket::disconnect(void)
{
    bool bSuccess = TCP_Socket::initialized();
    if (bSuccess)
    {
        closesocket(m_sockfd);
#ifdef _WIN32
        int result = WSACleanup();
        bSuccess = (result == NO_ERROR);
        if (!bSuccess)
        {
            lock("std_out_mutex");
            perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
            unlock("std_out_mutex");
        }
#endif
        m_sockfd = int(INVALID_SOCKET);
    }

    if (m_pAddrInfo != nullptr)
    {
        freeaddrinfo(m_pAddrInfo);
        m_pAddrInfo = nullptr;
    }

    return bSuccess;
}

/**
 * Get the factory name of this constructible
 */
std::string TCP_Socket::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Determine the last error
 * @param result the integer return value of the associated preceding function call which prompted the error
 */
int TCP_Socket::getLastError(int /* not used */) const
{
    switch (_errno)
    {
        default:
        // TODO: handle other cases...
        break;

        case NOTCONN:
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Socket is not connected.\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        case TIMEDOUT:
        lock("std_out_mutex");
        logMsg("warning", LoggingLevel::Enum::Warning,
               "Connection timed out.\n",
               getQualifiedMethodName(__func__));
        unlock("std_out_mutex");
        break;

        case WOULDBLOCK:
        if (blockingEnabled())
        {
            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "Receive request has timed-out.\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }
        else
        {
            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "Socket would block, but blocking is disabled.\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }
    }

    return _errno;
}

/**
 * Initialization function
 */
bool TCP_Socket::initialize(void)
{
    bool bSuccess = (m_pURL != nullptr); // assume success
    if (bSuccess)
    {
        disconnect();

#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        bSuccess = (result == NO_ERROR);
        if (!bSuccess)
        {
            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "WSAStartup failed with error code: " + std::to_string(result) + "\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
        }
        else
#endif
        {
            if (m_pAddrInfo != nullptr)
            {
                freeaddrinfo(m_pAddrInfo);
                m_pAddrInfo = nullptr;
            }

            struct addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            m_host = m_pURL->getHost();
            auto &&port = getPort();

            // use getaddrinfo to translate host name to an internet address
            bSuccess = (getaddrinfo(m_host.c_str(), port.c_str(), &hints, &m_pAddrInfo) == 0);
            if (!bSuccess)
            {
                lock("std_out_mutex");
                perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
                unlock("std_out_mutex");
            }
            else
            {
                // internet address was successfully determined, now create a socket for connecting to the server
                m_sockfd = int(socket(m_pAddrInfo->ai_family, m_pAddrInfo->ai_socktype, m_pAddrInfo->ai_protocol));
                bSuccess = (m_sockfd >= 0);
                if (!bSuccess)
                {
                    lock("std_out_mutex");
                    perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());
                    unlock("std_out_mutex");
                }
            }
        }
    }

    return bSuccess;
}

/**
 * Function to query whether this socket has been initialized
 */
bool TCP_Socket::initialized(void) const
{
    return (m_sockfd != static_cast<int>(INVALID_SOCKET) && m_pURL != nullptr && m_pURL->getHost() == m_host);
}

/**
 * Test this socket for open connection
 */
bool TCP_Socket::isConnected(void)
{
    bool bSuccess = initialized();
    if (bSuccess)
    {
        int flags = MSG_PEEK;
#ifdef _WIN32
        // unlike POSIX-based platforms, Windows does not support MSG_DONTWAIT flag per call to recv() function
        auto &&bBlockingEnabled = blockingEnabled();
        blockingEnabled(false);
#else
        flags |= MSG_DONTWAIT;
#endif
        // blocking mode cannot be enabled when making this call to check for connectivity
        auto &&result = recv(m_sockfd, nullptr, 0, flags);
#ifdef _WIN32
        blockingEnabled(bBlockingEnabled);
#endif
        if (result <= 0 && (_errno == NOTCONN || _errno == CONNRESET))
            bSuccess = false;
    }

    return bSuccess;
}

/**
 * Function to peek at the incoming data stream and store it in the buffer without removing sequence from the
 * input queue
 * @param pBuffer a pointer to a buffer in which the data will be received
 * @param length  the size of the buffer
 */
long TCP_Socket::peek(char *pBuffer,
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
long TCP_Socket::read(char *pBuffer,
                      size_t length,
                      int flags)
{
    long result = 0;
    if (pBuffer != nullptr && initialized())
    {
        result = recv(m_sockfd, pBuffer, length, flags);
        switch (result)
        {
            case -1:
            if (_errno == EAGAIN || _errno == EWOULDBLOCK)
                result = 0; // read nothing
            else if ((flags & MSG_PEEK) == 0)
            {
                lock("std_out_mutex");
                perror(("Error from " + getQualifiedMethodName(__func__)).c_str());
                unlock("std_out_mutex");
            }

            break;

            case 0: // the socket has been closed on the other end
            disconnect();
            break;

            default:
            // do nothing;
            break;
        }
    }

    return result;
}

/**
 * Function to write data to the socket
 * @param data a buffer containing the character sequence to be written to the socket
 */
bool TCP_Socket::write(const std::string &data)
{
    bool bSuccess = false;
    if (initialized())
    {
        auto &&result = send(m_sockfd, data.c_str(), int(data.length()), 0);
        switch (result)
        {
            case SOCKET_ERROR: // we got an error, check errno
            if (_errno == EAGAIN || _errno == EWOULDBLOCK)
                std::this_thread::sleep_for(std::chrono::milliseconds(m_sendTimeout));
            else
            {
                lock("std_out_mutex");
                logMsg("warning", LoggingLevel::Enum::Warning,
                       "Error sending socket: " + std::string(strerror(_errno)) + "\n",
                       getQualifiedMethodName(__func__));
                unlock("std_out_mutex");
            }

            break;

            case 0: // the socket has been closed on the other end
            disconnect();
            lock("std_out_mutex");
            logMsg("warning", LoggingLevel::Enum::Warning,
                   "The socket disconnected: " + std::string(strerror(_errno)) + "\n",
                   getQualifiedMethodName(__func__));
            unlock("std_out_mutex");
            break;

            default:
            bSuccess = true;
            break;
        }
    }

    return bSuccess;
}

}

}
