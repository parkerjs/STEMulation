#ifndef PRIORITY_SUBSCRIBER_H
#define PRIORITY_SUBSCRIBER_H

#include "subscriber.h"
#include "tuple_has_type.h"
#include <map>

namespace messaging
{

// forward declarations
template<typename ... Messengers>
class PrioritySubscriber;

/**
 * This class implements a priority message subscriber in the publisher-subscriber design pattern; message are
 * received and processed in order of priority
 */
template<>
class PrioritySubscriber<>
: virtual public Subscriber
{
protected:

    /**
     * Constructor
     */
    PrioritySubscriber(void)
    : m_pMessengerPrioritiesMap(new std::map<std::string, int>())
    {

    }

private:

    /**
     * Copy constructor
     */
    PrioritySubscriber(const PrioritySubscriber<> &subscriber) = delete;

    /**
     * Move constructor
     */
    PrioritySubscriber(PrioritySubscriber<> &&subscriber) = delete;

public:

    /**
     * Destructor
     */
    virtual ~PrioritySubscriber(void) override
    {
        if (m_pMessengerPrioritiesMap != nullptr)
        {
            delete m_pMessengerPrioritiesMap;
            m_pMessengerPrioritiesMap = nullptr; // not sure why I have to do this, but it silences static
                                                 // analyzer warnings; perhaps it is because derived classes
                                                 // inherit this class virtually?
        }
    }

private:

    /**
     * Copy assignment operator
     */
    PrioritySubscriber<> &operator = (const PrioritySubscriber<> &subscriber) = delete;

    /**
     * Move assignment operator
     */
    PrioritySubscriber<> &operator = (PrioritySubscriber<> &&subscriber) = delete;

public:

    /**
     * Get this subscriber's priority for receiving messages for the given recipient
     */
    inline int getPriority(const std::string &recipient) const
    {
        int priority = -1;
        auto *pTypeInfo = getMessageDispatcher()->type(recipient);
        if (pTypeInfo != nullptr)
            priority = getPriority(*pTypeInfo);

        return priority;
    }

    /**
     * Get this subscriber's priority for receiving messages of the specified type
     */
    template<typename T>
    inline int getPriority(void) const
    {
        return getPriority(typeid(T));
    }

protected:

    /**
     * Get this subscriber's priority for receiving messages of the specified type
     */
    inline int getPriority(const std::type_info &type) const
    {
        if (m_pMessengerPrioritiesMap != nullptr)
        {
            auto &&itMessengerPriorityPair = m_pMessengerPrioritiesMap->find(type.name());
            if (itMessengerPriorityPair != m_pMessengerPrioritiesMap->cend())
                return itMessengerPriorityPair->second;
        }

        return -1;
    }

public:

    /**
     * Set this subscriber's priority for receiving messages of the specified type
     */
    template<typename T>
    inline bool setPriority(int priority)
    {
        return setPriority(typeid(T), priority);
    }

protected:

    /**
     * Set this subscriber's priority for receiving messages of the specified type
     */
    inline bool setPriority(const std::type_info &type, int priority)
    {
        bool bSuccess = (m_pMessengerPrioritiesMap != nullptr);
        if (bSuccess)
        {
            auto &&itMessengerPriorityPair = m_pMessengerPrioritiesMap->find(type.name());
            bSuccess = (itMessengerPriorityPair != m_pMessengerPrioritiesMap->cend());
            if (bSuccess)
                itMessengerPriorityPair->second = priority;
        }

        return bSuccess;
    }

    /**
     * map of messenger priority pairs
     */
    std::map<std::string, int> *m_pMessengerPrioritiesMap;
};

/**
 * This class implements a generic prioritized message subscriber; it can receive and process messages for
 * specific object types in an order dictated by the type's associated priority assignment
 */
template<typename ... Messengers>
class PrioritySubscriber
: virtual public PrioritySubscriber<>
{
public:

    /**
     * Constructor
     */
    PrioritySubscriber(void)
    {
        m_pMessengerPrioritiesMap->insert({ { typeid(Messengers).name(), 0 } ... });
    }

private:

    /**
     * Copy constructor
     */
    PrioritySubscriber(const PrioritySubscriber<Messengers ...> &subscriber) = delete;

    /**
     * Move constructor
     */
    PrioritySubscriber(PrioritySubscriber<Messengers ...> &&subscriber) = delete;

public:

    /**
     * Destructor
     */
    virtual ~PrioritySubscriber(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    PrioritySubscriber<Messengers ...> &operator = (const PrioritySubscriber<Messengers ...> &subscriber) = delete;

    /**
     * Move assignment operator
     */
    PrioritySubscriber<Messengers ...> &operator = (PrioritySubscriber<Messengers ...> &&subscriber) = delete;
};

}

#endif
