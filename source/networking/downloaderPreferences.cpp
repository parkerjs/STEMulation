#include "cli_argument_processor.h"
#include "downloaderPreferences.h"
#include "file_system.h"
#include "priorityPublisher.h"

// using namespace declarations
using namespace messaging;
using namespace utilities;
using namespace utilities::file_system;

namespace networking
{

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
DownloaderPreferences::DownloaderPreferences(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  m_bBlockingEnabled(true),
  m_bKeepSocketConnectionAlive(false),
  m_connectRetryTimeout(1000),
  m_httpVersion("1.1"),
  m_maxConnectRetryAttempts(3),
  m_maxRecvRetryAttempts(3),
  m_maxThreads(32),
  m_receiveBufferSize(65536),
  m_receiveRetryTimeout(1000),
  m_receiveTimeout(1000),
  m_sendTimeout(1000)
{
    auto *pPublisher = getDependency<PriorityPublisher *>();
    if (pPublisher != nullptr)
        pPublisher->addSubscriber(this);

    getMessageDispatcher()->addRecipient("DownloaderPreferences::getConnectRetryTimeout",
                                         [this] (std::size_t &timeout) { timeout = m_connectRetryTimeout; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getHttpVersion",
                                         [this] (std::string &httpVersion)
                                         {
                                             httpVersion = m_httpVersion;
                                         });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getMaximumConnectRetryAttempts",
                                         [this] (std::size_t &numRetryAttempts)
                                         {
                                             numRetryAttempts = m_maxConnectRetryAttempts;
                                         });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getMaximumReceiveRetryAttempts",
                                         [this] (std::size_t &numRetryAttempts)
                                         {
                                             numRetryAttempts = m_maxRecvRetryAttempts;
                                         });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getMaximumThreads",
                                         [this] (long &maxThreads) { maxThreads = m_maxThreads; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getReceiveBufferSize",
                                         [this] (std::size_t &size) { size = m_receiveBufferSize; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getReceiveRetryTimeout",
                                         [this] (std::size_t &timeout) { timeout = m_receiveRetryTimeout; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getServerReceiveTimeout",
                                         [this] (std::size_t &timeout) { timeout = m_receiveTimeout; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getServerSendTimeout",
                                         [this] (std::size_t &timeout) { timeout = m_sendTimeout; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::getVariableRegistry",
                                         [this] (VariableRegistry *&pRegistry) { pRegistry = &m_registry; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::keepSocketConnectionAlive(bool)",
                                         (void (DownloaderPreferences::*)(bool))
                                         &DownloaderPreferences::keepSocketConnectionAlive, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::keepSocketConnectionAlive(void)",
                                         [this] (bool &bEnabled) { bEnabled = m_bKeepSocketConnectionAlive; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::socketBlockingEnabled(bool)",
                                         (void (DownloaderPreferences::*)(bool))
                                         &DownloaderPreferences::socketBlockingEnabled, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::socketBlockingEnabled(void)",
                                         [this] (bool &bEnabled) { bEnabled = m_bBlockingEnabled; });
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setConnectRetryTimeout",
                                         &DownloaderPreferences::setConnectRetryTimeout, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setHttpVersion",
                                         &DownloaderPreferences::setHttpVersion, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setMaximumConnectRetryAttempts",
                                         &DownloaderPreferences::setMaximumConnectRetryAttempts, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setMaximumReceiveRetryAttempts",
                                         &DownloaderPreferences::setMaximumReceiveRetryAttempts, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setMaximumThreads",
                                         &DownloaderPreferences::setMaximumThreads, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setReceiveBufferSize",
                                         &DownloaderPreferences::setReceiveBufferSize, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setReceiveRetryTimeout",
                                         &DownloaderPreferences::setReceiveRetryTimeout, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setServerReceiveTimeout",
                                         &DownloaderPreferences::setServerReceiveTimeout, this);
    getMessageDispatcher()->addRecipient("DownloaderPreferences::setServerSendTimeout",
                                         &DownloaderPreferences::setServerSendTimeout, this);
    getMessageDispatcher()->addRecipient("Loggable<std::string, ToggleableStream>::setLoggingStreamMap",
                                         &Loggable<std::string, ToggleableStream>::setLoggingStreamMap, this);
    getMessageDispatcher()->addRecipient("Setupable::setup",
                                         &Setupable::setup, this);
    getMessageDispatcher()->addRecipient("TokenMapConfigurable::configure",
                                         &TokenMapConfigurable::configure, this);

    setPriority<TokenMapConfigurable>(5);
}

/**
 * Destructor
 */
DownloaderPreferences::~DownloaderPreferences(void)
{

}

/**
 * Configure function (using a variable token map)
 */
bool DownloaderPreferences::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}

/**
 * create() function
 * @param dependencies a tuple of this object's required injection dependencies
 */
DownloaderPreferences *DownloaderPreferences::create(const tDependencies &dependencies)
{
    DownloaderPreferences *pDownloaderPreferences = nullptr;
    if (DownloaderPreferences::dependenciesInitialized(dependencies))
        pDownloaderPreferences = new DownloaderPreferences(dependencies);

    return pDownloaderPreferences;
}

/**
 * Setup function
 */
bool DownloaderPreferences::setup(void)
{
    // setup token map-configurable variables...
    m_registry["connectionRetryTimeout"] = m_connectRetryTimeout;
    m_registry["httpVersion"] = m_httpVersion;
    m_registry["keepSocketConnectionsAlive"] = m_bKeepSocketConnectionAlive;
    m_registry["maximumConnectionRetryAttempts"] = m_maxConnectRetryAttempts;
    m_registry["maximumReceiveRetryAttempts"] = m_maxRecvRetryAttempts;
    m_registry["maximumDownloadThreads"] = m_maxThreads;
    m_registry["receiveBufferSize"] = m_receiveBufferSize;
    m_registry["receiveRetryTimeout"] = m_receiveRetryTimeout;
    m_registry["serverReceiveTimeout"] = m_receiveTimeout;
    m_registry["serverSendTimeout"] = m_sendTimeout;
    m_registry["socketBlockingModeEnabled"] = m_bBlockingEnabled;

    return true;
}

}
