#ifndef MESSENGER_H
#define MESSENGER_H

#include "any.h"
#include "initializable.h"
#include "message_packet.h"
#include <memory>

namespace messaging
{

/**
 * This class provides an abstract interface for types that can send, receive, and process messages
 */
class Messenger
: public attributes::interfaces::Initializable
{
public:

    /**
     * Typedef declarations
     */
    typedef MessagePacket<functional::Any> tMessagePacket;

    /**
     * Constructor
     */
    Messenger(void)
    {

    }

protected:

    /**
     * Copy constructor
     */
    Messenger(const Messenger &messenger) = delete;

    /**
     * Move constructor
     */
    Messenger(Messenger &&messenger) = delete;

public:

    /**
     * Destructor
     */
    virtual ~Messenger(void)
    {
        // clear this object's vector of messages queued for processing
        m_messagesQueuedForProcessing.clear();

        // clear this object's vector of messages queued for send
        m_messagesQueuedForSend.clear();

        // this object's queue of received messages
        m_receivedMessages.clear();
    }

protected:

    /**
     * Copy assignment operator
     */
    Messenger &operator = (const Messenger &messsenger) = delete;

    /**
     * Move assignment operator
     */
    Messenger &operator = (Messenger &&messenger) = delete;

public:

    /**
     * Clear this object's vector of messages queued for processing
     */
    inline virtual void clearMessagesQueuedForProcessing(void) final
    {
        m_messagesQueuedForProcessing.clear();
    }

    /**
     * Clear this object's vector of messages queued for send
     */
    inline virtual void clearMessagesQueuedForSend(void) final
    {
        m_messagesQueuedForSend.clear();
    }

    /**
     * Clear this object's queue of received messages
     */
    inline virtual void clearReceivedMessages(void) final
    {
        m_receivedMessages.clear();
    }

    /**
     * Create a message
     */
    template<typename ... Args>
    inline std::shared_ptr<tMessagePacket> createMessage(Args && ... args)
    {
        return std::shared_ptr<tMessagePacket>(tMessagePacket::create(std::forward<Args>(args) ...));
    }

    /**
     * Get this object's vector of messages queued for processing
     */
    inline virtual std::vector<std::shared_ptr<tMessagePacket>> &getMessagesQueuedForProcessing(void) final
    {
        return m_messagesQueuedForProcessing;
    }

    /**
     * Get this object's vector of messages queued for send
     */
    inline virtual std::vector<std::shared_ptr<tMessagePacket>> &getMessagesQueuedForSend(void) final
    {
        return m_messagesQueuedForSend;
    }

    /**
     * Get this object's queue of received messages
     */
    inline virtual std::vector<std::shared_ptr<tMessagePacket>> &getReceivedMessages(void) final
    {
        return m_receivedMessages;
    }

    /**
     * Initialization function
     */
    inline virtual bool initialize(void) override
    {
        clearMessagesQueuedForProcessing();
        clearMessagesQueuedForSend();
        clearReceivedMessages();

        return true;
    }

protected:

    /**
     * Sort messages in order of priority
     */
    static void prioritizeMessages(std::vector<std::shared_ptr<tMessagePacket>> &messages)
    {
        // define the comparator function
        auto &&comparator = [] (auto &&pLeft, auto &&pRight)
                            {
                                int leftPriority = -1;
                                if (pLeft != nullptr)
                                    leftPriority = pLeft->getPriority();

                                int rightPriority = -1;
                                if (pRight != nullptr)
                                    rightPriority = pRight->getPriority();

                                return leftPriority > rightPriority;
                            };

        // sort tasks in order of priority
        std::sort(messages.begin(), messages.end(), comparator);
    }

public:

    /**
     * Function to process a received message
     */
    virtual bool processReceivedMessage(tMessagePacket *pMessagePacket) = 0;

    /**
     * Process this object's received messages
     * @param time the current simulation time
     */
    virtual bool processReceivedMessages(void)
    {
        bool bSuccess = queueReceivedMessagesForProcessing();
        if (bSuccess)
        {
            prioritizeMessages(m_messagesQueuedForProcessing);

            auto &&itMessagePacket = m_messagesQueuedForProcessing.begin();
            while (itMessagePacket != m_messagesQueuedForProcessing.end())
            {
                auto pMessagePacket = *itMessagePacket;
                bSuccess = (pMessagePacket != nullptr);
                if (bSuccess)
                {
                    bool bProcessed = processReceivedMessage(pMessagePacket.get());
                    if (bProcessed)
                    {
                        // remove message from this object's vector of messages queued for processing
                        itMessagePacket = m_messagesQueuedForProcessing.erase(itMessagePacket);

                        continue;
                    }
                }

                ++itMessagePacket;
            }
        }

        return bSuccess;
    }

    /**
     * Function to queue a message for processing
     */
    virtual bool queueMessageForProcessing(std::shared_ptr<tMessagePacket> pMessagePacket)
    {
        bool bSuccess = (pMessagePacket != nullptr);
        if (bSuccess)
        {
            m_messagesQueuedForProcessing.push_back(pMessagePacket);
        }

        return bSuccess;
    }

    /**
     * Function to queue a message for send
     */
    virtual bool queueMessageForSend(std::shared_ptr<tMessagePacket> pMessagePacket)
    {
        bool bSuccess = (pMessagePacket != nullptr);
        if (bSuccess)
        {
            m_messagesQueuedForSend.push_back(pMessagePacket);
        }

        return bSuccess;
    }

    /**
     * Function to queue received messages for processing
     */
    virtual bool queueReceivedMessagesForProcessing(void)
    {
        bool bSuccess = true; // assume success
        auto &&itMessagePacket = m_receivedMessages.begin();
        while (itMessagePacket != m_receivedMessages.end())
        {
            auto pMessagePacket = *itMessagePacket;
            bSuccess = (pMessagePacket != nullptr);
            if (bSuccess)
            {
                bool bMessageQueuedForProcessing = queueMessageForProcessing(pMessagePacket);
                if (bMessageQueuedForProcessing)
                {
                    itMessagePacket = m_receivedMessages.erase(itMessagePacket);

                    continue;
                }
            }

            ++itMessagePacket;
        }

        return bSuccess;
    }

    /**
     * Function to receive a message
     */
    virtual bool receiveMessage(std::shared_ptr<tMessagePacket> pMessagePacket)
    {
        bool bSuccess = (pMessagePacket != nullptr);
        if (bSuccess)
        {
            auto &&itMessagePacket = std::find(m_receivedMessages.cbegin(),
                                               m_receivedMessages.cend(),
                                               pMessagePacket);
            bSuccess = (itMessagePacket == m_receivedMessages.cend());
            if (bSuccess)
            {
                m_receivedMessages.push_back(pMessagePacket);
            }
        }

        return bSuccess;
    }

    /**
     * Function to send an outgoing message
     */
    virtual bool sendMessage(std::shared_ptr<tMessagePacket> pMessagePacket) = 0;

    /**
     * Function to send outgoing messages according to availability
     * @param time the current simulation time
     */
    virtual bool sendMessages(void)
    {
        bool bSuccess = true; // assume success
        auto &&itMessagePacket = m_messagesQueuedForSend.begin();
        while (bSuccess && itMessagePacket != m_messagesQueuedForSend.end())
        {
            auto pMessagePacket = *itMessagePacket;
            bSuccess = (pMessagePacket != nullptr);
            if (bSuccess)
            {
                bool bMessageSent = sendMessage(pMessagePacket);
                if (bMessageSent)
                {
                    itMessagePacket = m_messagesQueuedForSend.erase(itMessagePacket);

                    continue;
                }
            }

            ++itMessagePacket;
        }

        return bSuccess;
    }

protected:

    /**
     * this object's vector of messages queued for processing
     */
    std::vector<std::shared_ptr<tMessagePacket>> m_messagesQueuedForProcessing;

    /**
     * this object's vector of messages queued for send
     */
    std::vector<std::shared_ptr<tMessagePacket>> m_messagesQueuedForSend;

    /**
     * this object's queue of received messages
     */
    std::vector<std::shared_ptr<tMessagePacket>> m_receivedMessages;
};

}

#endif
