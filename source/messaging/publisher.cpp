#include "publisher.h"
#include "subscriber.h"

namespace messaging
{

/**
 * Constructor
 */
Publisher::Publisher(void)
: Synchronizable<int, std::recursive_mutex>(MutexMappable<int, std::recursive_mutex>::getMutexMap()),
  m_pSubscribers(new std::vector<Subscriber *>())
{

}

/**
 * Destructor
 */
Publisher::~Publisher(void)
{
    removeSubscribers();

    if (m_pSubscribers != nullptr)
        delete m_pSubscribers;
}

/**
 * Add a subscriber
 */
bool Publisher::addSubscriber(Subscriber *pSubscriber)
{
    this->lock();
    bool bSuccess = (pSubscriber != nullptr && m_pSubscribers != nullptr);
    if (bSuccess)
    {
        auto &&itSubscriber = std::find(m_pSubscribers->begin(),
                                        m_pSubscribers->end(),
                                        pSubscriber);
        if (itSubscriber == m_pSubscribers->end())
        {
            m_pSubscribers->push_back(pSubscriber);
            bSuccess = pSubscriber->addPublisher(this);
        }
    }

    this->unlock();

    return bSuccess;
}

/**
 * Add a vector of subscribers
 */
bool Publisher::addSubscribers(const std::vector<Subscriber *> &subscribers)
{
    bool bSuccess = true; // assume success
    auto &&itSubscriber = subscribers.cbegin();
    while (bSuccess && itSubscriber != subscribers.cend())
    {
        bSuccess = addSubscriber(*itSubscriber++);
    }

    return bSuccess;
}

/**
 * Get this publisher's subscribers
 */
std::vector<Subscriber *> *Publisher::getSubscribers(void)
{
    return m_pSubscribers;
}

/**
 * Function to determine if this object has the specified subscriber
 */
bool Publisher::hasSubscriber(Subscriber *pSubscriber) const
{
    bool bSuccess = (m_pSubscribers != nullptr && pSubscriber != nullptr);
    if (bSuccess)
    {
        bSuccess = std::find(m_pSubscribers->cbegin(),
                             m_pSubscribers->cend(),
                             pSubscriber) != m_pSubscribers->cend();
    }

    return bSuccess;
}

/**
 * Function to notify all subscribers
 * @param recipient the addressee
 */
bool Publisher::notify(const std::string &recipient)
{
    return notify(recipient, std::vector<functional::Any>{ });
}

/**
 * Function to notify all subscribers
 * @param recipients a vector of addressees
 */
bool Publisher::notify(const std::vector<std::string> &recipients)
{
    bool bSuccess = true;
    for (auto &&recipient : recipients)
    {
        bSuccess = notify(recipient);
        if (!bSuccess)
            break;
    }

    return bSuccess;
}

/**
 * Function to notify all subscribers to receive the specified vector of recipient-message vector pairs
 * @param recipientMessageVectorPairs a vector of recipient, message (argument) vector pairs
 */
bool Publisher::notify(std::vector<std::pair<std::string,
                       std::vector<functional::Any>>> &&recipientMessageVectorPairs)
{
    return notify(recipientMessageVectorPairs);
}

/**
 * Function to notify all subscribers to receive the specified vector of recipient-message vector pairs
 * @param recipientMessageVectorPairs a vector of recipient, message (argument) vector pairs
 */
bool Publisher::notify(std::vector<std::pair<std::string,
                       std::vector<functional::Any>>> &recipientMessageVectorPairs)
{
    bool bSuccess = true; // assume success
    auto &&itRecipientMessageVectorPair = recipientMessageVectorPairs.begin();
    while (bSuccess && itRecipientMessageVectorPair != recipientMessageVectorPairs.end())
    {
        bSuccess = notify(itRecipientMessageVectorPair->first, itRecipientMessageVectorPair->second);
        if (bSuccess)
            ++itRecipientMessageVectorPair;
    }

    return bSuccess;
}

/**
 * Function to notify all subscribers to receive the specified list of messages
 * @param recipient the addressee to which messages will be delivered
 * @param messages  upon success, will be delivered to the recipient
 */
bool Publisher::notify(const std::string &recipient,
                       std::vector<functional::Any> &&messages)
{
    return notify(recipient, messages);
}

/**
 * Function to notify all subscribers to receive the specified list of messages
 * @param recipient the addressee to which messages will be delivered
 * @param messages  upon success, will be delivered to the recipient
 */
bool Publisher::notify(const std::string &recipient,
                       std::vector<functional::Any> &messages)
{
    this->lock();
    auto *pSubscribers = getSubscribers();
    bool bSuccess = (pSubscribers != nullptr);
    if (bSuccess)
    {
        auto &&itSubscriber = pSubscribers->begin();
        while (bSuccess && itSubscriber != pSubscribers->end())
        {
            auto *pSubscriber = *itSubscriber++;
            bSuccess = (pSubscriber != nullptr);
            if (bSuccess)
                bSuccess = pSubscriber->process(recipient, messages);
        }
    }

    this->unlock();

    return bSuccess;
}

/**
 * Remove a subscriber
 */
bool Publisher::removeSubscriber(Subscriber *pSubscriber)
{
    this->lock();

    bool bSuccess = (m_pSubscribers != nullptr);
    if (bSuccess)
    {
        auto &&itSubscriber = std::find(m_pSubscribers->begin(),
                                        m_pSubscribers->end(),
                                        pSubscriber);
        bSuccess = (itSubscriber != m_pSubscribers->cend());
        if (bSuccess)
        {
            auto *pSubscriber = *itSubscriber;
            m_pSubscribers->erase(itSubscriber);
            pSubscriber->unsubscribe(this);
        }
    }

    this->unlock();

    return bSuccess;
}

/**
 * Remove all subscribers
 */
bool Publisher::removeSubscribers(void)
{
    this->lock();

    bool bSuccess = (m_pSubscribers != nullptr);
    if (bSuccess)
    {
        auto &&itSubscriber = m_pSubscribers->begin();
        while (bSuccess && itSubscriber != m_pSubscribers->end())
        {
            auto *pSubscriber = *itSubscriber;
            itSubscriber = m_pSubscribers->erase(itSubscriber);
            bSuccess = (pSubscriber != nullptr);
            if (bSuccess)
                bSuccess = pSubscriber->unsubscribe(this);
        }
    }

    this->unlock();

    return bSuccess;
}

}
