#ifndef STATIC_MESSAGE_DISPATCHER_H
#define STATIC_MESSAGE_DISPATCHER_H

#include "message_dispatcher.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace messaging
{

/**
 * Class for static message dispatch to designated recipients
 */
class StaticMessageDispatcher final
{
private:

    /**
     * Constructor
     */
    StaticMessageDispatcher(void) = delete;

    /**
     * Copy constructor
     */
    StaticMessageDispatcher(const StaticMessageDispatcher &dispatcher) = delete;

    /**
     * Move constructor
     */
    StaticMessageDispatcher(StaticMessageDispatcher &&dispatcher) = delete;

    /**
     * Destructor
     */
    virtual ~StaticMessageDispatcher(void) = delete;

    /**
     * Copy assignment operator
     */
    StaticMessageDispatcher &operator = (const StaticMessageDispatcher &dispatcher) = delete;

    /**
     * Move assignment operator
     */
    StaticMessageDispatcher &operator = (StaticMessageDispatcher &&dispatcher) = delete;

public:

    /**
     * Forwarding function to add a recipient to which messages will be delivered; see the MessageDispatcher
     * class for available overloads
     */
    template<typename ... Args>
    inline static void addRecipient(Args && ... args)
    {
        getMessageDispatcher()->addRecipient(std::forward<Args>(args) ...);
    }

    /**
     * Function to dispatch messages to the designated recipient
     * @param recipient the addressee to which messages will be delivered
     * @param messages  upon success, will be delivered to the recipient
     */
    inline static bool dispatch(const std::string &recipient,
                                std::vector<functional::Any> &messages)
    {
        return getMessageDispatcher()->dispatch(recipient, messages);
    }

    /**
     * Get this subscriber's message dispatcher
     */
    inline static std::shared_ptr<MessageDispatcher> getMessageDispatcher(void)
    {
        static std::shared_ptr<MessageDispatcher> pMessageDispatcher(new MessageDispatcher);

        return pMessageDispatcher;
    }

    /**
     * Retrieve a static map of name-recipient pairs
     */
    inline static std::map<std::string, BaseRecipient *> &getRecipients(void)
    {
        return getMessageDispatcher()->getRecipients();
    }

    /**
     * Function to remove a recipient
     */
    inline static bool removeRecipient(const std::string &recipient)
    {
        return getMessageDispatcher()->removeRecipient(recipient);
    }

    /**
     * Retrieve the type_info associated with the specified recipient; if a recipient with the given name
     * doesn't exist, the function returns nullptr
     */
    inline const std::type_info *type(const std::string &recipient) const
    {
        return getMessageDispatcher()->type(recipient);
    }
};

/**
 * This class serves as a helper class to facilitate static message recipient registration
 */
struct StaticMessageRecipientRegistrar
{
    /**
     * Constructor
     */
    template<typename ... NameRecipientPairs>
    StaticMessageRecipientRegistrar(NameRecipientPairs && ... nameRecipientPairs)
    {
        int dummy[] = { 0, ((void)StaticMessageDispatcher::addRecipient(
                        std::forward<NameRecipientPairs>(nameRecipientPairs)), 0) ... };

        // silence unused variable warnings...
        (void)(dummy);
    }
};

}

#endif
