#ifndef MESSAGE_PACKET_H
#define MESSAGE_PACKET_H

#include "nameable.h"
#include <algorithm>
#include <string>
#include <vector>

namespace messaging
{

/**
 * This class implements a container to store a packet of messages along with the intended recipient(s) of
 * those messages
 */
template<typename Holder>
class MessagePacket
: public attributes::interfaces::Nameable
{
public:

    /**
     * Constructor
     * @param name the type of message associated with this object
     */
    MessagePacket(const std::string &name)
    : m_name(name),
      m_priority(-1),
      m_receiveProcessingAvailabilityTime(0.0),
      m_sendAvailabilityTime(0.0),
      m_time(0.0)
    {

    }

    /**
     * Constructor
     * @param name       the type of message associated with this object
     * @param recipients the intended receivers of this object's messages
     * @param message    the first message
     * @param messages   a variadic list of additional messages
     */
    template<typename Message, typename ... Messages,
             typename std::enable_if<!std::is_same<typename std::decay<Message>::type, Holder>::value,
                                     int>::type = 0>
    MessagePacket(const std::string &name,
                  const std::vector<std::string> &recipients,
                  Message &&message,
                  Messages && ... messages)
    : MessagePacket(name)
    {
        // add recipients
        for (auto &&recipient : recipients)
            addRecipient(recipient);

        // add messages
        addMessages({ Holder(message),
                      Holder(messages) ... });
    }

    /**
     * Constructor
     * @param name      the type of message associated with this object
     * @param recipient the intended receiver of this object's messages
     * @param message   the first message
     * @param messages  a variadic list of additional messages
     */
    template<typename Message, typename ... Messages,
             typename std::enable_if<!std::is_same<typename std::decay<Message>::type, Holder>::value,
                                     int>::type = 0>
    MessagePacket(const std::string &name,
                  const std::string &recipient,
                  Message &&message,
                  Messages && ... messages)
    : MessagePacket(name,
                    std::vector<std::string>{ recipient},
                    std::forward<Message>(message),
                    std::forward<Messages>(messages) ...)
    {

    }

private:

    /**
     * Copy constructor
     */
    MessagePacket(const MessagePacket<Holder> &packet) = delete;

    /**
     * Move constructor
     */
    MessagePacket(MessagePacket<Holder> &&packet) = delete;

public:

    /**
     * Destructor
     */
    virtual ~MessagePacket(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    MessagePacket<Holder> &operator = (const MessagePacket<Holder> &packet) = delete;

    /**
     * Move assignment operator
     */
    MessagePacket<Holder> &operator = (MessagePacket<Holder> &&packet) = delete;

public:

    /**
     * Add a message to this object
     */
    template<typename Message>
    typename std::enable_if<!std::is_same<typename std::decay<Message>::type, Holder>::value, void>::type
    addMessage(Message &&message)
    {
        addMessage(Holder(message));
    }

    /**
     * Add a message to this object
     */
    inline virtual void addMessage(const Holder &message) final
    {
        m_messages.push_back(message);
    }

    /**
     * Add messages to this object
     * @param message  the first message
     * @param messages a variadic list of additional messages
     */
    template<typename Message, typename ... Messages>
    typename std::enable_if<!std::is_same<typename std::decay<Message>::type, Holder>::value, void>::type
    addMessages(Message &&message,
                Messages && ... messages)
    {
        addMessages({ Holder(message),
                      Holder(messages) ... });
    }

    /**
     * Add messages to this object
     */
    inline virtual void addMessages(std::vector<Holder> &&messages) final
    {
        addMessages(messages);
    }

    /**
     * Add a messages to this object
     */
    void addMessages(std::vector<Holder> &messages)
    {
        for (auto &&message : messages)
        {
            addMessage(message);
        }
    }

    /**
     * Add a recipient to this object
     */
    virtual void addRecipient(const std::string &recipient) final
    {
        auto &&itRecipient = std::lower_bound(m_recipients.begin(),
                                              m_recipients.end(),
                                              recipient);
        if (itRecipient == m_recipients.end() || *itRecipient != recipient)
        {
            m_recipients.insert(itRecipient, recipient);
        }
    }

    /**
     * create() function
     * @param name       the type of message associated with this object
     * @param recipients the intended receivers of this object's messages
     * @param message    the first message
     * @param messages   a variadic list of additional messages
     */
    inline static MessagePacket<Holder> *create(const std::string &name)
    {
        return new MessagePacket<Holder>(name);
    }

    /**
     * create() function
     * @param name       the type of message associated with this object
     * @param recipients the intended receivers of this object's messages
     * @param message    the first message
     * @param messages   a variadic list of additional messages
     */
    template<typename Message, typename ... Messages>
    static typename std::enable_if<!std::is_same<typename std::decay<Message>::type, Holder>::value,
                                   MessagePacket<Holder> *>::type
    create(const std::string &name,
           const std::vector<std::string> &recipients,
           Message &&message,
           Messages && ... messages)
    {
        return new MessagePacket<Holder>(name,
                                         recipients,
                                         std::forward<Message>(message),
                                         std::forward<Messages>(messages) ...);
    }

    /**
     * create() function
     * @param name      the type of message associated with this object
     * @param recipient the intended receiver of this object's messages
     * @param message   the first message
     * @param messages  a variadic list of additional messages
     */
    template<typename Message, typename ... Messages>
    static inline typename std::enable_if<!std::is_same<typename std::decay<Message>::type, Holder>::value,
                                          MessagePacket<Holder> *>::type
    create(const std::string &name,
           const std::string &recipient,
           Message &&message,
           Messages && ... messages)
    {
        return new MessagePacket<Holder>(name,
                                         recipient,
                                         std::forward<Message>(message),
                                         std::forward<Messages>(messages) ...);
    }

    /**
     * Get this object's messages
     */
    inline virtual std::vector<Holder> &getMessages(void) final
    {
        return m_messages;
    }

    /**
     * Get the name associated with this message
     */
    inline virtual std::string getName(void) const override
    {
        return m_name;
    }

    /**
     * Get the priority assigned to this message
     */
    inline virtual int getPriority(void) const final
    {
        return m_priority;
    }

    /**
     * Get the time at which this message is available for processing
     */
    inline virtual double getReceiveProcessingAvailabilityTime(void) const final
    {
        return m_receiveProcessingAvailabilityTime;
    }

    /**
     * Get the recipients of this object's messages
     */
    inline virtual std::vector<std::string> &getRecipients(void) final
    {
        return m_recipients;
    }

    /**
     * Get the time at which this message is available to be sent
     */
    inline virtual double getSendAvailabilityTime(void) const final
    {
        return m_sendAvailabilityTime;
    }

    /**
     * Get the time associated with this object's messages
     */
    inline virtual double getTime(void) const final
    {
        return m_time;
    }

    /**
     * Remove a recipient from this object
     */
    virtual bool removeRecipient(const std::string &recipient) final
    {
        auto &&itRecipient = std::find(m_recipients.begin(),
                                       m_recipients.end(),
                                       recipient);
        bool bSuccess = (itRecipient != m_recipients.end());
        if (bSuccess)
        {
            m_recipients.erase(itRecipient);
        }

        return bSuccess;
    }

    /**
     * Set this object's messages
     */
    inline virtual void setMessages(const std::vector<Holder> &messages) final
    {
        m_messages = messages;
    }

    /**
     * Set the name associated with this message
     */
    inline virtual void setName(const std::string &name) override
    {
        m_name = name;
    }

    /**
     * Set the priority assigned to this message
     */
    inline virtual void setPriority(int priority) final
    {
        m_priority = priority;
    }

    /**
     * Set the time at which this message is available for processing
     */
    inline virtual void setReceiveProcessingAvailabilityTime(double processingAvailabilityTime) final
    {
        m_receiveProcessingAvailabilityTime = processingAvailabilityTime;
    }

    /**
     * Set the recipients of this object's messages
     */
    inline virtual void setRecipients(const std::vector<std::string> &recipients) final
    {
        m_recipients = recipients;
    }

    /**
     * Set the time at which this message is available to be sent
     */
    inline virtual void setSendAvailabilityTime(double sendAvailabilityTime) final
    {
        m_sendAvailabilityTime = sendAvailabilityTime;
    }

    /**
     * Set the time associated with this object's message
     */
    inline virtual void setTime(double time) final
    {
        m_time = time;
    }

protected:

    /**
     * this object's messages
     */
    std::vector<Holder> m_messages;

    /**
     * the name associated with this message
     */
    std::string m_name;

    /**
     * the priority assigned to this message
     */
    int m_priority;

    /**
     * the time at which this message is available for processing
     */
    double m_receiveProcessingAvailabilityTime;

    /**
     * the intended recipients  of this object's messages
     */
    std::vector<std::string> m_recipients;

    /**
     * the time at which this message is available to be sent
     */
    double m_sendAvailabilityTime;

    /**
     * the time associated with this object's messages
     */
    double m_time;
};

}

#endif
