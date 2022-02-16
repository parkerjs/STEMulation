#ifndef PRIORITY_PUBLISHER_H
#define PRIORITY_PUBLISHER_H

#include "priority_subscriber.h"
#include "publisher.h"
#include <algorithm>
#include <iostream>
#include <stack>

namespace messaging
{

/**
 * This class implements a priority message publisher in the publisher-subscriber design pattern; messages are
 * published in order of priority
 */
class PriorityPublisher
: public Publisher
{
public:

    /**
     * Using declarations
     */
    using Publisher::notify;

    /**
     * Constructor
     */
    EXPORT_STEM PriorityPublisher(void);

private:

    /**
     * Copy constructor
     */
    EXPORT_STEM PriorityPublisher(const PriorityPublisher &publisher) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM PriorityPublisher(PriorityPublisher &&publisher) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~PriorityPublisher(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM PriorityPublisher &operator = (const PriorityPublisher &publisher) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM PriorityPublisher &operator = (PriorityPublisher &&publisher) = delete;

public:

    /**
     * Add a subscriber
     */
    EXPORT_STEM virtual bool addSubscriber(Subscriber *pSubscriber) override;

private:

    /**
     * Get this publisher's subscribers
     */
    EXPORT_STEM virtual std::vector<Subscriber *> *getSubscribers(void) override;

public:

    /**
     * Function to notify all subscribers to receive the specified list of messages
     * @param recipient the addressee to which messages will be delivered
     * @param messages  upon success, will be delivered to the recipient
     */
    EXPORT_STEM virtual bool notify(const std::string &recipient,
                                    std::vector<functional::Any> &messages) override;

    /**
     * Remove a subscriber
     */
    EXPORT_STEM virtual bool removeSubscriber(Subscriber *pSubscriber) override;

    /**
     * Remove all subscribers
     */
    EXPORT_STEM virtual bool removeSubscribers(void) override;

private:

    /**
     * stack of sorted subscriber vectors
     */
    std::stack<std::vector<Subscriber *>> *m_pSortedSubscriberStack;
};

}

#endif
