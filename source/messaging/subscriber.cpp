#include "publisher.h"
#include "subscriber.h"

namespace messaging
{

/**
 * Constructor
 */
Subscriber::Subscriber(void)
: m_pMessageDispatcher(new MessageDispatcher())
{

}

/**
 * Constructor
 */
Subscriber::Subscriber(std::shared_ptr<MessageDispatcher> pMessageDispatcher)
: m_pMessageDispatcher(pMessageDispatcher)
{

}

/**
 * Destructor
 */
Subscriber::~Subscriber(void)
{
    unsubscribeFromAll();
}

/**
 * Add a publisher to which this object subscribes
 */
bool Subscriber::addPublisher(Publisher *pPublisher)
{
    lock();

    bool bSuccess = (pPublisher != nullptr);
    if (bSuccess)
    {
        auto &&itPublisher = std::find(m_publishers.cbegin(),
                                       m_publishers.cend(),
                                       pPublisher);
        if (itPublisher == m_publishers.cend())
        {
            m_publishers.push_back(pPublisher);
            pPublisher->addSubscriber(this);
        }
    }

    unlock();

    return bSuccess;
}

/**
 * Add a vector of publishers
 */
bool Subscriber::addPublishers(const std::vector<Publisher *> &publishers)
{
    bool bSuccess = true; // assume success
    auto &&itPublisher = m_publishers.cbegin();
    while (bSuccess && itPublisher != publishers.cend())
    {
        bSuccess = addPublisher(*itPublisher++);
    }

    return bSuccess;
}

/**
 * Get this subscriber's message dispatcher
 */
std::shared_ptr<MessageDispatcher> Subscriber::getMessageDispatcher(void) const
{
    return m_pMessageDispatcher;
}

/**
 * Get the publishers to which this object subscribes
 */
std::vector<Publisher *> Subscriber::getPublishers(void)
{
    return m_publishers;
}

/**
 * Function to receive and process messages from the publisher to which this object subscribes
 * @param recipient the addressee to which messages will be delivered
 * @param messages  upon success, will be delivered to the recipient
 */
bool Subscriber::process(const std::string &recipient,
                         std::vector<functional::Any> &messages)
{
    bool bSuccess = (m_pMessageDispatcher != nullptr);
    if (bSuccess)
        bSuccess = m_pMessageDispatcher->dispatch(recipient, messages);

    return bSuccess;
}

/**
 * Set this subscriber's message dispatcher
 */
void Subscriber::setMessageDispatcher(std::shared_ptr<MessageDispatcher> pMessageDispatcher)
{
    m_pMessageDispatcher = pMessageDispatcher;
}

/**
 * Un-subscribe from the specified publisher; returns true if the current object has successfully been removed
 * as a subscriber of the given publisher
 */
bool Subscriber::unsubscribe(Publisher *pPublisher)
{
    lock();

    auto &&itPublisher = std::find(m_publishers.cbegin(),
                                   m_publishers.cend(),
                                   pPublisher);
    bool bSuccess = (itPublisher != m_publishers.cend());
    if (bSuccess)
    {
        auto *pPublisher = *itPublisher;
        m_publishers.erase(itPublisher);
        pPublisher->removeSubscriber(this);
    }

    unlock();

    return bSuccess;
}

/**
 * Un-subscribe from all publishers to which this object currently subscribes
 */
bool Subscriber::unsubscribeFromAll(void)
{
    lock();

    bool bSuccess = true; // assume success
    auto &&itPublisher = m_publishers.begin();
    while (bSuccess && itPublisher != m_publishers.end())
    {
        auto *pPublisher = *itPublisher;
        itPublisher = m_publishers.erase(itPublisher);
        bSuccess = (pPublisher != nullptr);
        if (bSuccess)
            bSuccess = pPublisher->removeSubscriber(this);
    }

    unlock();

    return bSuccess;
}

}
