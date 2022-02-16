#include "dictionary.h"
#include "file_system.h"
#include "messageConfiguration.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif

// using namespace declarations
using namespace rapidxml;
using namespace utilities;
using namespace utilities::file_system;

namespace messaging
{

/**
 * Constructor
 */
MessageConfiguration::MessageConfiguration(void)
: m_priority(-1),
  m_receiveProcessingDelay(0.0),
  m_sendProcessingDelay(0.0),
  m_sendUpdateRate(0.0)
{

}

/**
 * Copy constructor
 */
MessageConfiguration::MessageConfiguration(const MessageConfiguration &configuration)
{
    operator = (configuration);
}

/**
 * Move constructor
 */
MessageConfiguration::MessageConfiguration(MessageConfiguration &&configuration)
{
    operator = (std::move(configuration));
}

/**
 * Destructor
 */
MessageConfiguration::~MessageConfiguration(void)
{

}

/**
 * Copy assignment operator
 */
MessageConfiguration &MessageConfiguration::operator = (const MessageConfiguration &configuration)
{
    if (&configuration != this)
    {
        Loggable<std::string, std::ostream>::operator = (configuration);

        m_priority = configuration.m_priority;
        m_receiveProcessingDelay = configuration.m_receiveProcessingDelay;
        m_sendProcessingDelay = configuration.m_sendProcessingDelay;
        m_sendUpdateRate = configuration.m_sendUpdateRate;
        m_type = configuration.m_type;
    }

    return *this;
}

/**
 * Move assignment operator
 */
MessageConfiguration &MessageConfiguration::operator = (MessageConfiguration &&configuration)
{
    if (&configuration != this)
    {
        Loggable<std::string, std::ostream>::operator = (std::move(configuration));

        configuration.swap(*this);
    }

    return *this;
}

/**
 * clone() function
 */
MessageConfiguration *MessageConfiguration::clone(void) const
{
    return new MessageConfiguration(*this);
}

/**
 * Configure function (using input file or input string)
 */
bool MessageConfiguration::configure(const std::string &filename)
{
    std::ifstream stream;
    bool bSuccess = FileSystem::openFileStream(filename, stream);
    if (bSuccess)
    {
        // process this object's variable registry
        Dictionary dictionary(&m_registry);

        auto &&tokenMap = dictionary.createTokenPairs<tTokenMap>(stream);
        stream.close();

        bSuccess = configure(tokenMap);
        if (!bSuccess)
        {
            logMsg(std::cout, LoggingLevel::Enum::Error,
                   "File-configuration using \"" + filename + "\" failed.\n",
                   getQualifiedMethodName(__func__));
        }
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to open or parse \"" + filename + "\".\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Configure function (using a variable token map)
 */
bool MessageConfiguration::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    bool bSuccess = dictionary.populate(tokenMap);
    if (!bSuccess)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to perform token map configuration!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string MessageConfiguration::getClassName(void) const
{
    return "MessageConfiguration";
}

/**
 * Retrieve the message priority
 */
int MessageConfiguration::getPriority(void) const
{
    return m_priority;
}

/**
 * Retrieve the message receive processing delay
 */
double MessageConfiguration::getReceiveProcessingDelay(void) const
{
    return m_receiveProcessingDelay;
}

/**
 * Retrieve the message send processing delay
 */
double MessageConfiguration::getSendProcessingDelay(void) const
{
    return m_sendProcessingDelay;
}

/**
 * Retrieve the message send update rate
 */
double MessageConfiguration::getSendUpdateRate(void) const
{
    return m_sendUpdateRate;
}

/**
 * Retrieve the message type
 */
std::string MessageConfiguration::getType(void) const
{
    return m_type;
}

/**
 * Initialization function
 */
bool MessageConfiguration::initialize(void)
{
    m_priority = -1;
    m_receiveProcessingDelay = 0.0;
    m_sendProcessingDelay = 0.0;
    m_sendUpdateRate = 0.0;
    m_type.clear();

    return true;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool MessageConfiguration::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "messageConfiguration") == 0);
    if (bSuccess)
    {
        auto *pPriorityNode = pNode->first_node("processingPriority");
        if (pPriorityNode != nullptr)
        {
            auto processingPriority = std::stoi(pPriorityNode->value());
            setPriority(processingPriority);
        }

        auto *pReceiveProcessingDelayNode = pNode->first_node("receiveProcessingDelay");
        if (pReceiveProcessingDelayNode != nullptr)
        {
            auto processingDelay = std::stod(pReceiveProcessingDelayNode->value());
            setReceiveProcessingDelay(processingDelay);
        }

        auto *pSendProcessingDelayNode = pNode->first_node("sendProcessingDelay");
        if (pSendProcessingDelayNode != nullptr)
        {
            auto processingDelay = std::stod(pSendProcessingDelayNode->value());
            setSendProcessingDelay(processingDelay);
        }

        auto *pSendUpdateRateNode = pNode->first_node("sendUpdateRate");
        if (pSendUpdateRateNode != nullptr)
        {
            auto updateRate = std::stod(pSendUpdateRateNode->value());
            setSendUpdateRate(updateRate);
        }

        auto *pTypeNode = pNode->first_node("type");
        if (pTypeNode != nullptr)
        {
            setType(pTypeNode->value());
        }
    }

    return bSuccess;
}
#endif
/**
 * Set the message priority
 */
void MessageConfiguration::setPriority(std::size_t priority)
{
    m_priority = priority;
}

/**
 * Set the message receive processing delay
 */
void MessageConfiguration::setReceiveProcessingDelay(double receiveProcessingDelay)
{
    m_receiveProcessingDelay = receiveProcessingDelay;
}

/**
 * Set the message send processing delay
 */
void MessageConfiguration::setSendProcessingDelay(double sendProcessingDelay)
{
    m_sendProcessingDelay = sendProcessingDelay;
}

/**
 * Set the message send update rate
 */
void MessageConfiguration::setSendUpdateRate(double sendUpdateRate)
{
    m_sendUpdateRate = sendUpdateRate;
}

/**
 * Set the message type
 */
void MessageConfiguration::setType(const std::string &type)
{
    m_type = type;
}

/**
 * Setup function
 */
bool MessageConfiguration::setup(void)
{
    // setup token map-configurable variables
    m_registry["priority"] = m_priority;
    m_registry["receiveProcessingDelay"] = m_receiveProcessingDelay;
    m_registry["sendProcessingDelay"] = m_sendProcessingDelay;
    m_registry["sendUpdateRate"] = m_sendUpdateRate;
    m_registry["type"] = m_type;

    return true;
}

/**
 * Swap function
 */
void MessageConfiguration::swap(MessageConfiguration &configuration)
{
    Loggable<std::string, std::ostream>::swap(configuration);

    std::swap(m_priority, configuration.m_priority);
    std::swap(m_receiveProcessingDelay, configuration.m_receiveProcessingDelay);
    std::swap(m_sendProcessingDelay, configuration.m_sendProcessingDelay);
    std::swap(m_sendUpdateRate, configuration.m_sendUpdateRate);
    m_type.swap(configuration.m_type);
}

}
