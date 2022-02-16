#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "export_library.h"
#include "mutex_mappable.h"
#include "synchronizable.h"
#include <algorithm>
#include <vector>

// forward declarations
namespace functional { class Any; }
namespace utilities { class VariableRegistryEntry; }

namespace messaging
{

// forward declarations
class Subscriber;

/**
 * This class implements a message publisher in the publisher-subscriber design pattern
 */
class Publisher
: public attributes::concrete::MutexMappable<int, std::recursive_mutex>,
  public attributes::concrete::Synchronizable<int, std::recursive_mutex>
{
public:

    /**
     * Using declarations
     */
    using attributes::concrete::Synchronizable<int, std::recursive_mutex>::lock; // resolves msvc link error?
    using attributes::concrete::Synchronizable<int, std::recursive_mutex>::tryLock; // resolves msvc link
                                                                                    // error?
    using attributes::concrete::Synchronizable<int, std::recursive_mutex>::unlock; // resolves msvc link error?

    /**
     * Constructor
     */
    EXPORT_STEM Publisher(void);

private:

    /**
     * Copy constructor
     */
    EXPORT_STEM Publisher(const Publisher &publisher) = delete;

    /**
     * Move constructor
     */
    EXPORT_STEM Publisher(Publisher &&publisher) = delete;

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Publisher(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Publisher &operator = (const Publisher &publisher) = delete;

    /**
     * Move assignment operator
     */
    EXPORT_STEM Publisher &operator = (Publisher &&publisher) = delete;

public:

    /**
     * Add a subscriber
     */
    EXPORT_STEM virtual bool addSubscriber(Subscriber *pSubscriber);

    /**
     * Add a vector of subscribers
     */
    EXPORT_STEM virtual bool addSubscribers(const std::vector<Subscriber *> &subscribers) final;

protected:

    /**
     * Get this publisher's subscribers
     */
    EXPORT_STEM virtual std::vector<Subscriber *> *getSubscribers(void);

public:

    /**
     * Function to determine if this object has the specified subscriber
     */
    EXPORT_STEM virtual bool hasSubscriber(Subscriber *pSubscriber) const;

    /**
     * Function to notify all subscribers
     * @param recipient the addressee
     */
    EXPORT_STEM virtual bool notify(const std::string &recipient);

    /**
     * Function to notify all subscribers
     * @param recipients a vector of addressees
     */
    EXPORT_STEM virtual bool notify(const std::vector<std::string> &recipients);

    /**
     * Function to notify all subscribers to receive the specified list of messages
     * @param recipients a vector of addressees to which messages will be delivered
     * @param message    the first message (argument) to be delivered to the recipient
     * @param messages   a variadic list of additional messages (arguments) to be delivered to recipients
     */
    template<typename Message, typename ... Messages>
    typename std::enable_if<!std::is_same<typename std::decay<Message>::type,
                                   std::vector<utilities::VariableRegistryEntry>>::value &&
                                   !std::is_same<typename std::decay<Message>::type,
                                   std::vector<functional::Any>>::value, bool>::type
    notify(const std::vector<std::string> &recipients,
           Message &&message,
           Messages && ... messages)
    {
        bool bSuccess = true;
        for (auto &&recipient : recipients)
        {
            bSuccess = notify(recipient, std::forward<Message>(message),
                                         std::forward<Messages>(messages) ...);
            if (!bSuccess)
                break;
        }

        return bSuccess;
    }

    /**
     * Function to notify all subscribers to receive the specified list of messages
     * @param recipient the addressee to which messages will be delivered
     * @param message   the first message (argument) to be delivered to the recipient
     * @param messages  a variadic list of additional messages (arguments) to be delivered to the recipient
     */
    template<typename Message, typename ... Messages>
    inline typename std::enable_if<!std::is_same<typename std::decay<Message>::type,
                                   std::vector<utilities::VariableRegistryEntry>>::value &&
                                   !std::is_same<typename std::decay<Message>::type,
                                   std::vector<functional::Any>>::value, bool>::type
    notify(const std::string &recipient,
           Message &&message,
           Messages && ... messages)
    {
        return notify(recipient, { functional::Any(message),
                                   functional::Any(messages) ... });
    }

    /**
     * Function to notify all subscribers to receive the specified vector of recipient-message vector pairs
     * @param recipientMessageVectorPairs a vector of recipient, message (argument) vector pairs
     */
    EXPORT_STEM virtual bool notify(std::vector<std::pair<std::string,
                                    std::vector<functional::Any>>> &&recipientMessageVectorPairs);

    /**
     * Function to notify all subscribers to receive the specified vector of recipient-message vector pairs
     * @param recipientMessageVectorPairs a vector of recipient, message (argument) vector pairs
     */
    EXPORT_STEM virtual bool notify(std::vector<std::pair<std::string,
                                    std::vector<functional::Any>>> &recipientMessageVectorPairs);

    /**
     * Function to notify all subscribers to receive the specified list of messages
     * @param recipient the addressee to which messages will be delivered
     * @param messages  upon success, will be delivered to the recipient
     */
    EXPORT_STEM virtual bool notify(const std::string &recipient,
                                    std::vector<functional::Any> &&messages);

    /**
     * Function to notify all subscribers to receive the specified list of messages
     * @param recipient the addressee to which messages will be delivered
     * @param messages  upon success, will be delivered to the recipient
     */
    EXPORT_STEM virtual bool notify(const std::string &recipient,
                                    std::vector<functional::Any> &messages);

    /**
     * Remove a subscriber
     */
    EXPORT_STEM virtual bool removeSubscriber(Subscriber *pSubscriber);

    /**
     * Remove all subscribers
     */
    EXPORT_STEM virtual bool removeSubscribers(void);

protected:

    /**
     * set of subscribers to be provided with notifications
     */
    std::vector<Subscriber *> *m_pSubscribers;
};

}

#endif
