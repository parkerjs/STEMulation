#include "dictionary.h"
#include "downloader.h"
#include "downloaderPreferences.h"
#include "httpHeaders.h"
#include "receiver.h"
#include "socket.h"
#include "static_message_dispatcher.h"
#include "URL.h"
#include <cstring>
#include <fstream>
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
#include <sys/fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#endif

// file-scoped variables
static constexpr char className[] = "Downloader";

// using namespace declarations
using namespace attributes::concrete;
using namespace messaging;
using namespace networking::sockets;
using namespace utilities;

namespace networking
{

// register static subscribers...
static StaticMessageRecipientRegistrar staticMessageRegistrar(
{ std::make_pair("StaticLoggable::setLoggingStreamMap<std::string, ToggleableStream>",
                 &Downloader::setLoggingStreamMap<std::string, ToggleableStream>),
  std::make_pair("StaticSynchronizable::setMutexMap<std::string, std::mutex *>",
                 &Downloader::setMutexMap<std::string, std::mutex *>)});

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 * @param url          the web address from which the resource(s) will be downloaded
 */
Downloader::Downloader(const tDependencies &dependencies,
                       const std::string &url)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  m_httpRequestMethod(HttpRequestMethod::Enum::Get),
  m_pHttpRequestHeaders(new HttpHeaders),
  m_pHttpResponseHeaders(new HttpHeaders),
  m_pSocket(nullptr),
  m_pURL(new URL(url))
{

}

/**
 * Copy constructor
 */
Downloader::Downloader(const Downloader &downloader)
: DependencyInjectableVirtualBaseInitializer(1, downloader),
  m_pHttpRequestHeaders(nullptr),
  m_pHttpResponseHeaders(nullptr),
  m_pSocket(nullptr),
  m_pURL(nullptr)
{
    operator = (downloader);
}

/**
 * Move constructor
 */
Downloader::Downloader(Downloader &&downloader)
: DependencyInjectableVirtualBaseInitializer(1, std::move(downloader)),
  m_pHttpRequestHeaders(nullptr),
  m_pHttpResponseHeaders(nullptr),
  m_pSocket(nullptr),
  m_pURL(nullptr)
{
    operator = (std::move(downloader));
}

/**
 * Destructor
 */
Downloader::~Downloader(void)
{
    if (m_pHttpRequestHeaders != nullptr)
        delete m_pHttpRequestHeaders;

    if (m_pHttpResponseHeaders != nullptr)
        delete m_pHttpResponseHeaders;

    if (m_pSocket != nullptr)
        delete m_pSocket;

    if (m_pURL != nullptr)
        delete m_pURL;
}

/**
 * Copy assignment operator
 */
Downloader &Downloader::operator = (const Downloader &downloader)
{
    if (&downloader != this)
    {
        m_httpRequestMethod = downloader.m_httpRequestMethod;

        if (m_pHttpRequestHeaders != nullptr)
        {
            delete m_pHttpRequestHeaders;
            m_pHttpRequestHeaders = nullptr;
        }

        if (downloader.m_pHttpRequestHeaders != nullptr)
            m_pHttpRequestHeaders = downloader.m_pHttpRequestHeaders->clone();

        if (m_pHttpResponseHeaders != nullptr)
        {
            delete m_pHttpResponseHeaders;
            m_pHttpResponseHeaders = nullptr;
        }

        if (downloader.m_pHttpResponseHeaders != nullptr)
            m_pHttpResponseHeaders = downloader.m_pHttpResponseHeaders->clone();

        // do not clone sockets, just instantiate another one later
        if (m_pSocket != nullptr)
        {
            delete m_pSocket;
            m_pSocket = nullptr;
        }

        // need to set url before calling createSocket()
        if (m_pURL != nullptr)
        {
            delete m_pURL;
            m_pURL = nullptr;
        }

        if (downloader.m_pURL != nullptr)
            m_pURL = downloader.m_pURL->clone();

        m_pSocket = createSocket();
    }

    return *this;
}

/**
 * Move assignment operator
 */
Downloader &Downloader::operator = (Downloader &&downloader)
{
    if (&downloader != this)
    {
        m_httpRequestMethod = std::move(downloader.m_httpRequestMethod);

        if (m_pHttpRequestHeaders != nullptr)
        {
            delete m_pHttpRequestHeaders;
            m_pHttpRequestHeaders = nullptr;
        }

        m_pHttpRequestHeaders = std::move(downloader.m_pHttpRequestHeaders);
        downloader.m_pHttpRequestHeaders = nullptr;

        if (m_pHttpResponseHeaders != nullptr)
        {
            delete m_pHttpResponseHeaders;
            m_pHttpResponseHeaders = nullptr;
        }

        m_pHttpResponseHeaders = std::move(downloader.m_pHttpResponseHeaders);
        downloader.m_pHttpResponseHeaders = nullptr;

        if (m_pSocket != nullptr)
        {
            delete m_pSocket;
            m_pSocket = nullptr;
        }

        m_pSocket = std::move(downloader.m_pSocket);
        downloader.m_pSocket = nullptr;

        if (m_pURL != nullptr)
        {
            delete m_pURL;
            m_pURL = nullptr;
        }

        m_pURL = std::move(downloader.m_pURL);
        downloader.m_pURL = nullptr;
    }

    return *this;
}

/**
 * clone() function
 */
Downloader *Downloader::clone(void) const
{
    auto *pDownloader = new Downloader(*this);
    if (pDownloader != nullptr)
        pDownloader->setup();

    return pDownloader;
}

/**
 * Configure function (using a variable token map)
 */
bool Downloader::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}

/**
 * create() function
 * @param dependencies a tuple of this object's required injection dependencies
 * @param url          the web address from which the resource(s) will be downloaded
 */
Downloader *Downloader::create(const tDependencies &dependencies,
                               const std::string &url)
{
    Downloader *pDownloader = nullptr;
    if (Downloader::dependenciesInitialized(dependencies))
    {
        pDownloader = new Downloader(dependencies, url);
        pDownloader->setup();
    }

    return pDownloader;
}

/**
 * Function to create a socket
 */
Socket *Downloader::createSocket(void)
{
    return Socket::create(m_pURL);
}

/**
 * Default function implementation to download data from a server. Function returns true if data was
 * successfully read from the server
 * @param data an std::string that will be populated with bytes read from the server.
 */
bool Downloader::download(std::string &data)
{
    auto *pDownloaderPreferences = getDependency<DownloaderPreferences *>();
    bool bSuccess = (pDownloaderPreferences != nullptr);
    if (bSuccess)
    {
        auto &&bKeepSocketConnectionAlive = pDownloaderPreferences->keepSocketConnectionAlive();
        auto &&connectRetryTimeout = pDownloaderPreferences->getConnectRetryTimeout();
        auto &&maxConnectRetryAttempts = pDownloaderPreferences->getMaximumConnectRetryAttempts();
        auto &&maxRecvRetryAttempts = pDownloaderPreferences->getMaximumReceiveRetryAttempts();
        auto &&receiveBufferSize = pDownloaderPreferences->getReceiveBufferSize();
        auto &&receiveRetryTimeout = pDownloaderPreferences->getReceiveRetryTimeout();

        if (m_pSocket == nullptr)
            m_pSocket = createSocket();

        bSuccess = (m_pSocket != nullptr);
        if (bSuccess)
        {
            std::size_t receiveAttempts = 0;
            do
            {
                std::size_t connectionAttempts = 0;
                while (connectionAttempts < maxConnectRetryAttempts)
                {
                    bool bConnected = m_pSocket->isConnected();
                    if (!bConnected)
                        bConnected = m_pSocket->connect();

                    if (bConnected)
                    {
                        auto &&bBlockingEnabled = pDownloaderPreferences->socketBlockingEnabled();
                        auto &&serverReceiveTimeout = pDownloaderPreferences->getServerReceiveTimeout();
                        auto &&serverSendTimeout = pDownloaderPreferences->getServerSendTimeout();

                        m_pSocket->blockingEnabled(bBlockingEnabled);
                        m_pSocket->keepAlive(bKeepSocketConnectionAlive);
                        m_pSocket->setServerReceiveTimeout(serverReceiveTimeout);
                        m_pSocket->setServerSendTimeout(serverSendTimeout);

                        bool bRequestSuccess = request();
                        if (bRequestSuccess)
                            break;
                        else
                        {
                            lock("std_out_mutex");
                            logMsg("warning", LoggingLevel::Enum::Warning,
                                   "Bad HTTP " + std::string(getHttpRequestMethod()) + " request.\n",
                                   getQualifiedMethodName(__func__));
                            unlock("std_out_mutex");

                            m_pSocket->disconnect();

                            return false;
                        }
                    }

                    // wait for the specified retry timeout and try again...
                    ++connectionAttempts;

                    lock("std_out_mutex");
                    logMsg("warning", LoggingLevel::Enum::Warning,
                           "Connection attempt # " + std::to_string(connectionAttempts) + " of " +
                           std::to_string(maxConnectRetryAttempts) + " failed, retrying in " +
                           std::to_string(connectRetryTimeout / 1000) + " seconds...\n",
                           getQualifiedMethodName(__func__));
                    unlock("std_out_mutex");

                    std::this_thread::sleep_for(std::chrono::milliseconds(connectRetryTimeout));
                }

                bool bConnectionSuccess = (connectionAttempts < maxConnectRetryAttempts);
                if (bConnectionSuccess)
                {
                    bool bServerSupportsKeepAlive = false;
                    long totalBytesRead = 0;

                    auto *pReceiver = Receiver::create(m_pHttpResponseHeaders, m_pSocket, receiveBufferSize);
                    if (pReceiver == nullptr) // error has occurred while attempting to instantiate receiver,
                    {                         // close the connection
                        m_pSocket->disconnect();

                        lock("std_out_mutex");
                        perror(("Warning from " + getQualifiedMethodName(__func__)).c_str());

                        logMsg("error", LoggingLevel::Enum::Error,
                               "Receiver could not be instantiated, closing connection.\n",
                               getQualifiedMethodName(__func__));
                        unlock("std_out_mutex");
                    }
                    else
                    {
                        if (bKeepSocketConnectionAlive)
                            bServerSupportsKeepAlive = m_pHttpResponseHeaders->keepAliveSupported();

                        totalBytesRead = pReceiver->receive(data);
                    }

                    bool bReceiveSuccess = true; // assume success
                    if (totalBytesRead < 0 || pReceiver == nullptr)
                    {
                        bReceiveSuccess = false;
                        if (totalBytesRead < 0)
                            m_pSocket->getLastError();

                        lock("std_out_mutex");
                        logMsg("warning", LoggingLevel::Enum::Warning,
                               "Receive attempt # " + std::to_string(1 + receiveAttempts) + " failed, retrying "
                               "in " + std::to_string(receiveRetryTimeout / 1000) + " seconds...\n",
                               getQualifiedMethodName(__func__));
                        unlock("std_out_mutex");

                        // wait for the specified retry timeout and try again...
                        std::this_thread::sleep_for(std::chrono::milliseconds(receiveRetryTimeout));

                        // clear the buffer
                        data.clear();

                        m_pSocket->disconnect();
                    }

                    if (pReceiver != nullptr)
                        delete pReceiver;

                    bSuccess = bReceiveSuccess;
                    if (!bKeepSocketConnectionAlive || !bServerSupportsKeepAlive || !bReceiveSuccess)
                        m_pSocket->disconnect();

                    if (!bReceiveSuccess)
                        continue;
                }

                break;
            }
            while (receiveAttempts++ < maxRecvRetryAttempts);
        }
    }

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string Downloader::getClassName(void) const
{
    return className;
}

/**
 * Get the full HTTP request string
 */
std::string Downloader::getHttpRequest(void) const
{
    std::string httpRequest;
    auto *pDownloaderPreferences = getDependency<DownloaderPreferences *>();
    if (m_pURL != nullptr && pDownloaderPreferences != nullptr)
    {
        auto &&host = m_pURL->getHost();
        auto &&requestURI = m_pURL->getRequestURI();
        auto &&httpVersion = pDownloaderPreferences->getHttpVersion();
        httpRequest = std::string(m_httpRequestMethod) + " " + requestURI + " HTTP/" + httpVersion +
                      "\r\nHost: " + host + "\r\n";

        if (m_pHttpRequestHeaders != nullptr)
        {
            auto &&userName = m_pURL->getUserName();
            auto &&userPassword = m_pURL->getUserPassword();

            if (!userName.empty() && !userPassword.empty())
            {
                auto &&authorization =  "Basic " + userName + ":" + userPassword;
                m_pHttpRequestHeaders->addEntry("Authorization", authorization);
            }

            std::ostringstream oss;
            oss << *m_pHttpRequestHeaders;
            httpRequest += oss.str();
        }

        httpRequest += "\r\n";
    }

    return httpRequest;
}

/**
 * Get this object's HTTP request headers
 */
HttpHeaders *Downloader::getHttpRequesteHeaders(void)
{
    return m_pHttpRequestHeaders;
}

/**
 * Get this object's HTTP response headers
 */
HttpHeaders *Downloader::getHttpResponseHeaders(void)
{
    std::string data;
    download(data);

    return m_pHttpResponseHeaders;
}

/**
 * Get the port
 */
std::string Downloader::getPort(void) const
{
    return m_pSocket != nullptr ? m_pSocket->getPort() : "";
}

/**
 * Function to send request to the server, if connected. Upon success, returns true.
 */
inline bool Downloader::request(void)
{
    auto *pDownloaderPreferences = getDependency<DownloaderPreferences *>();
    bool bSuccess = (pDownloaderPreferences != nullptr && m_pSocket != nullptr);
    if (bSuccess)
    {
        // build the request
        if (pDownloaderPreferences->keepSocketConnectionAlive() && m_pHttpRequestHeaders != nullptr)
            m_pHttpRequestHeaders->addEntry("Connection", "keep-alive");

        bSuccess = m_pSocket->initialized();
        if (!bSuccess)
            bSuccess = m_pSocket->initialize();

        if (bSuccess)
        {
            auto &&request = getHttpRequest();
            bSuccess = m_pSocket->write(request);
        }
    }

    return bSuccess;
}

/**
 * Set the port
 */
void Downloader::setPort(const std::string &port)
{
    if (m_pURL != nullptr && m_pURL->getPort() != port)
    {
        if (m_pSocket != nullptr)
            m_pSocket->disconnect();

        m_pURL->setPort(port);
    }
}

/**
 * Setup function
 */
bool Downloader::setup(void)
{
    // TODO: implement this

    return true;
}

}
