#include "priorityPublisher.h"

namespace messaging
{

/**
 * Constructor
 */
PriorityPublisher::PriorityPublisher(void)
: m_pSortedSubscriberStack(new std::stack<std::vector<Subscriber *>>())
{

}

/**
 * Destructor
 */
PriorityPublisher::~PriorityPublisher(void)
{
    if (m_pSortedSubscriberStack != nullptr)
        delete m_pSortedSubscriberStack;
}

/**
 * Add a subscriber
 */
bool PriorityPublisher::addSubscriber(Subscriber *pSubscriber)
{
    bool bSuccess = m_pSortedSubscriberStack->empty();
    if (bSuccess)
        bSuccess = Publisher::addSubscriber(pSubscriber);
    else
        std::cout << "Warning from PriorityPublisher::addSubscriber(): Cannot add additional subscribers "
                  << "during notification." << std::endl << std::endl;

    return bSuccess;
}

/**
 * Get this publisher's subscribers
 */
std::vector<Subscriber *> *PriorityPublisher::getSubscribers(void)
{
    if (m_pSortedSubscriberStack->empty())
        return m_pSubscribers;
    else
        return &m_pSortedSubscriberStack->top();
}

/**
 * Function to notify all subscribers to receive the specified list of messages
 * @param recipient the addressee to which messages will be delivered
 * @param messages  upon success, will be delivered to the recipient
 */
bool PriorityPublisher::notify(const std::string &recipient,
                               std::vector<functional::Any> &messages)
{
    this->lock();
    bool bSuccess = (m_pSubscribers != nullptr && m_pSortedSubscriberStack != nullptr);
    if (bSuccess)
    {
        m_pSortedSubscriberStack->push(*m_pSubscribers);
        auto &&subscribers = m_pSortedSubscriberStack->top();

        // now sort priority subscribers in order of priority
        std::sort(subscribers.begin(), subscribers.end(), [&recipient] (auto *pLeft, auto *pRight)
        {
            int leftPriority = -1;
            auto *pLeftSubscriber = dynamic_cast<PrioritySubscriber<> *>(pLeft);
            if (pLeftSubscriber != nullptr)
                leftPriority = pLeftSubscriber->getPriority(recipient);

            int rightPriority = -1;
            auto *pRightSubscriber = dynamic_cast<PrioritySubscriber<> *>(pRight);
            if (pRightSubscriber != nullptr)
                rightPriority = pRightSubscriber->getPriority(recipient);

            return leftPriority > rightPriority;
        });

        if (bSuccess)
            bSuccess = Publisher::notify(recipient, messages);

        m_pSortedSubscriberStack->pop();
    }

    this->unlock();

    return bSuccess;
}

/**
 * Remove a subscriber
 */
bool PriorityPublisher::removeSubscriber(Subscriber *pSubscriber)
{
    bool bSuccess = (m_pSortedSubscriberStack->empty());
    if (bSuccess)
        Publisher::removeSubscriber(pSubscriber);
    else
        std::cout << "Warning from PriorityPublisher::removeSubscriber(): Cannot remove subscribers "
                  << "during notification." << std::endl << std::endl;

    return bSuccess;
}

/**
 * Remove all subscribers
 */
bool PriorityPublisher::removeSubscribers(void)
{
    bool bSuccess = (m_pSortedSubscriberStack->empty());
    if (bSuccess)
        Publisher::removeSubscribers();
    else
        std::cout << "Warning from PriorityPublisher::removeSubscribers(): Cannot remove subscribers "
                  << "during notification." << std::endl << std::endl;

    return bSuccess;
}

}
