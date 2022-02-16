#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "export_library.h"
#include "message_dispatcher.h"
#include "mutex_mappable.h"
#include "synchronizable.h"
#include <memory>

namespace messaging
{

// forward declarations
class Publisher;

/**
 * This class implements a message subscriber in the publisher-subscriber design pattern
 */
class Subscriber
: public attributes::concrete::MutexMappable<int, std::recursive_mutex>,
  public attributes::concrete::Synchronizable<int, std::recursive_mutex>
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM Subscriber(void);

    /**
     * Constructor
     */
    template<typename ... NameRecipientPairs>
    Subscriber(NameRecipientPairs && ... nameRecipientPairs)
    : Subscriber()
    {
        int dummy[] = { 0, ((void)m_pMessageDispatcher->addRecipient(
                        std::forward<NameRecipientPairs>(nameRecipientPairs)), 0) ... };

        // silence unused variable warnings...
        (void)(dummy);
    }

    /**
     * Constructor
     */
    EXPORT_STEM Subscriber(std::shared_ptr<MessageDispatcher> pMessageDispatcher);

private:

    /**
     * Copy constructor
     */
    EXPORT_STEM Subscriber(const Subscriber &subscriber) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM Subscriber(Subscriber &&subscriber) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Subscriber(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Subscriber &operator = (const Subscriber &subscriber) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM Subscriber &operator = (Subscriber &&subscriber) = delete;

public:

    /**
     * Add a publisher to which this object subscribes
     */
    EXPORT_STEM virtual bool addPublisher(Publisher *pPublisher) final;

    /**
     * Add a vector of publishers
     */
    EXPORT_STEM virtual bool addPublishers(const std::vector<Publisher *> &publishers) final;

    /**
     * Get this subscriber's message dispatcher
     */
    EXPORT_STEM virtual std::shared_ptr<MessageDispatcher> getMessageDispatcher(void) const final;

protected:

    /**
     * Get the publishers to which this object subscribes
     */
    EXPORT_STEM virtual std::vector<Publisher *> getPublishers(void) final;

public:

    /**
     * Function to receive and process messages from the publisher to which this object subscribes
     * @param recipient the addressee to which messages will be delivered
     * @param messages  upon success, will be delivered to the recipient
     */
    EXPORT_STEM virtual bool process(const std::string &recipient,
                                     std::vector<functional::Any> &messages);

    /**
     * Set this subscriber's message dispatcher
     */
    EXPORT_STEM virtual void setMessageDispatcher(std::shared_ptr<MessageDispatcher> pMessageDispatcher) final;

    /**
     * Un-subscribe from the specified publisher; returns true if the current object has successfully been
     * removed as a subscriber of the given publisher
     */
    EXPORT_STEM virtual bool unsubscribe(Publisher *pPublisher) final;

    /**
     * Un-subscribe from all publishers to which this object currently subscribes
     */
    EXPORT_STEM virtual bool unsubscribeFromAll(void) final;

private:

    /**
     * message dispatcher object
     */
    std::shared_ptr<MessageDispatcher> m_pMessageDispatcher;

    /**
     * the publishers to which this object subscribes
     */
    std::vector<Publisher *> m_publishers;
};

}

#endif
