#ifndef MESSAGE_DISPATCHER_H
#define MESSAGE_DISPATCHER_H

#include "message_recipient.h"
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace messaging
{

/**
 * Class for dispatching messages to designated recipients
 */
class MessageDispatcher final
{
public:

    /**
     * Constructor
     */
    MessageDispatcher(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    MessageDispatcher(const MessageDispatcher &dispatcher) = delete;

    /**
     * Move constructor
     */
    MessageDispatcher(MessageDispatcher &&dispatcher) = delete;

public:

    /**
     * Destructor
     */
    virtual ~MessageDispatcher(void)
    {
        for (auto &&recipient : m_recipients)
        {
            auto *pRecipient = recipient.second;
            if (pRecipient != nullptr)
                delete pRecipient;
        }
    }

private:

    /**
     * Copy assignment operator
     */
    MessageDispatcher &operator = (const MessageDispatcher &dispatcher) = delete;

    /**
     * Move assignment operator
     */
    MessageDispatcher &operator = (MessageDispatcher &&dispatcher) = delete;

public:

    /**
     * Function to add a member recipient to which messages will be delivered
     * @param recipient the addressee to which messages will be delivered
     * @param function  a member callback function
     * @param instance  an instance of the class upon which the member callback function will be invoked
     */
    template<typename Function, typename Instance>
    void addRecipient(const std::string &recipient,
                      Function &&function,
                      Instance &&instance)
    {
        auto &&itRecipient = m_recipients.find(recipient);
        if (itRecipient != m_recipients.cend())
            removeRecipient(recipient);

        m_recipients.emplace(recipient, new Recipient<Function>(std::forward<Function>(function),
                                                                std::forward<Instance>(instance)));
    }

    /**
     * Function to add a member recipient to which messages will be delivered
     * @param nameRecipientInstanceTriplet an std::tuple in which the first element specifies the name of the
     *                                     recipient, the second element specifies the member callback
     *                                     function, and the third element gives the instance upon which the
     *                                     callback is to be invoked
     */
    template<typename Name, typename Function, typename Instance>
    inline void addRecipient(std::tuple<Name, Function, Instance> &&nameRecipientInstanceTriplet)
    {
        addRecipient(std::get<0>(nameRecipientInstanceTriplet),
                     std::forward<Function>(std::get<1>(nameRecipientInstanceTriplet)),
                     std::forward<Instance>(std::get<2>(nameRecipientInstanceTriplet)));
    }

    /**
     * Function to add a non-member recipient to which messages will be delivered
     * @param recipient the addressee to which messages will be delivered
     * @param function  a non-member callback function
     */
    template<typename Function>
    void addRecipient(const std::string &recipient,
                      Function &&function)
    {
        auto &&itRecipient = m_recipients.find(recipient);
        if (itRecipient != m_recipients.cend())
            removeRecipient(recipient);

        m_recipients.emplace(recipient, new Recipient<Function>(std::forward<Function>(function)));
    }

    /**
     * Function to add a non-member recipient to which messages will be delivered
     * @param nameRecipientPair an std::pair in which the key specifies the name of the recipient and the value
     *                          specifies the non-member callback function
     */
    template<typename Name, typename Function>
    inline void addRecipient(std::pair<Name, Function> &&nameRecipientPair)
    {
        addRecipient(nameRecipientPair.first, std::forward<Function>(nameRecipientPair.second));
    }

    /**
     * Function to dispatch messages to the designated recipient
     * @param recipient the addressee to which messages will be delivered
     * @param messages  upon success, will be delivered to the recipient
     */
    template<typename Message>
    bool dispatch(const std::string &recipient,
                  std::vector<Message> &messages)
    {
        bool bSuccess = true;
        auto &&itRecipient = m_recipients.find(recipient);
        if (itRecipient != m_recipients.cend())
        {
            auto *pRecipient = itRecipient->second;
            bSuccess = (pRecipient != nullptr);
            if (bSuccess)
                bSuccess = pRecipient->receive(messages);
        }

        return bSuccess;
    }

    /**
     * Retrieve a recipient by name
     */
    virtual BaseRecipient *getRecipient(const std::string &recipient) final
    {
        BaseRecipient *pRecipient = nullptr;
        auto &&itRecipient = m_recipients.find(recipient);
        if (itRecipient != m_recipients.cend())
        {
            pRecipient = itRecipient->second;
        }

        return pRecipient;
    }

    /**
     * Get this object's map of name-recipient pairs
     */
    inline virtual std::map<std::string, BaseRecipient *> &getRecipients(void) final
    {
        return m_recipients;
    }

    /**
     * Function to remove a recipient
     */
    virtual bool removeRecipient(const std::string &recipient) final
    {
        auto &&itRecipient = m_recipients.find(recipient);
        bool bSuccess = (itRecipient != m_recipients.cend());
        if (bSuccess)
        {
            auto *pRecipient = itRecipient->second;
            bSuccess = (pRecipient != nullptr);
            if (bSuccess)
                delete pRecipient;

            m_recipients.erase(itRecipient);
        }

        return bSuccess;
    }

    /**
     * Retrieve the type_info associated with the specified recipient; if a recipient with the given name
     * doesn't exist, the function returns nullptr
     */
    virtual const std::type_info *type(const std::string &recipient) const final
    {
        const std::type_info *pTypeInfo = nullptr;
        auto &&itRecipient = m_recipients.find(recipient);
        bool bSuccess = (itRecipient != m_recipients.cend());
        if (bSuccess)
            pTypeInfo = &itRecipient->second->type();

        return pTypeInfo;
    }

private:

    /**
     * map of name-recipient pairs
     */
    std::map<std::string, BaseRecipient *> m_recipients;
};

}

#endif
