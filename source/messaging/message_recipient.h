#ifndef MESSAGE_RECIPIENT_H
#define MESSAGE_RECIPIENT_H

#include "any.h"
#include "function_traits.h"
#include <vector>

namespace messaging
{

// forward declarations
template<typename Function, bool = std::is_member_function_pointer<Function>::value> class Recipient;

/**
 * This class facilitates the handling of dispatched messages and contains methods to forward them to the
 * intended recipient(s)
 */
class BaseRecipient
{
public:

    /**
     * Constructor
     */
    BaseRecipient(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    BaseRecipient(const BaseRecipient &recipient) = delete;

    /**
     * Move constructor
     */
    BaseRecipient(BaseRecipient &&recipient) = delete;

public:

    /**
     * Destructor
     */
    virtual ~BaseRecipient(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    BaseRecipient &operator = (const BaseRecipient &recipient) = delete;

    /**
     * Move assignment operator
     */
    BaseRecipient &operator = (BaseRecipient &&recipient) = delete;

public:

    /**
     * Function to receive messages
     */
    virtual bool receive(std::vector<functional::Any> &messages) = 0;

    /**
     * Return this object's type_info
     */
    virtual const std::type_info &type(void) const = 0;
};

/**
 * This class facilitates the handling of dispatched messages and contains methods to forward them to the
 * intended recipient(s) (specialization for member functions)
 */
template<typename Function>
class Recipient<Function, true>
: public BaseRecipient
{
private:

    /**
     * Typedef declarations
     */
    typedef typename traits::helpers::function_traits<Function>::instance_type Class;

public:

    /**
     * Constructor
     */
    Recipient(Function function, const Class &instance)
    : m_function(std::forward<Function>(function)),
      m_pInstance(&instance)
    {

    }

    /**
     * Constructor
     */
    Recipient(Function function, Class *pInstance)
    : m_function(std::forward<Function>(function)),
      m_pInstance(pInstance)
    {

    }

    /**
     * Destructor
     */
    virtual ~Recipient(void) override
    {

    }

private:

    /**
     * Invoke the receiving function and expand the vector of messages into an argument list
     */
    template<typename Message, std::size_t ... indices, typename T = Function>
    inline typename std::enable_if<std::is_same<typename
                                   traits::helpers::function_traits<T>::return_type, bool>::value, bool>::type
    invoke(std::vector<Message> &messages,
           std::index_sequence<indices ...>)
    {
        bool bSuccess = (m_pInstance != nullptr && m_function != nullptr);
        if (bSuccess)
            bSuccess = (m_pInstance->*m_function)(messages[indices] ...);

        return bSuccess;
    }

    /**
     * Invoke the receiving function and expand the vector of messages into an argument list
     */
    template<typename Message, std::size_t ... indices, typename T = Function,
             typename std::enable_if<!std::is_same<
             typename traits::helpers::function_traits<T>::return_type, bool>::value, int>::type = 0>
    inline bool invoke(std::vector<Message> &messages,
                       std::index_sequence<indices ...>)
    {
        bool bSuccess = (m_pInstance != nullptr && m_function != nullptr);
        if (bSuccess)
            (m_pInstance->*m_function)(messages[indices] ...);

        return bSuccess;
    }

public:

    /**
     * Function to receive messages
     * @param messages upon success, will be delivered to the recipient
     */
    inline virtual bool receive(std::vector<functional::Any> &messages) override final
    {
        constexpr auto arity = traits::helpers::function_traits<Function>::arity;
        if (messages.size() < arity)
            throw "Incorrect number of arguments...";

        try
        {
            return invoke(messages,
                          std::make_index_sequence<arity>{ });
        }
        catch (const functional::bad_any_cast &)
        {
            // if a bad_any_cast is thrown, perhaps the recipient expects a vector of any objects
            std::vector<functional::Any> anys = { functional::Any(messages) };

            return invoke(anys,
                          std::make_index_sequence<arity>{ });
        }
    }

    /**
     * Return this object's type_info
     */
    inline virtual const std::type_info &type(void) const override
    {
        return typeid(Class);
    }

private:

    /**
     * an instance of the member function
     */
    Function m_function;

    /**
     * a pointer to an instance of the class to which the member function belongs
     */
    Class *m_pInstance;
};

/**
 * This class facilitates the handling of dispatched messages and contains methods to forward them to the
 * intended recipient(s)
 */
template<typename Function>
class Recipient<Function, false>
: public BaseRecipient
{
public:

    /**
     * Constructor
     */
    Recipient(Function function)
    : m_function(function)
    {

    }

    /**
     * Destructor
     */
    virtual ~Recipient(void) override
    {

    }

private:

    /**
     * Invoke the receiving function and expand the vector of messages into an argument list
     */
    template<typename Message, typename T = Function, std::size_t ... indices>
    inline typename std::enable_if<std::is_pointer<T>::value && std::is_same<typename
                                   traits::helpers::function_traits<T>::return_type, bool>::value, bool>::type
    invoke(std::vector<Message> &messages,
           std::index_sequence<indices ...>)
    {
        bool bSuccess = (m_function != nullptr);
        if (bSuccess)
            bSuccess = m_function(messages[indices] ...);

        return bSuccess;
    }

    /**
     * Invoke the receiving function and expand the vector of messages into an argument list
     */
    template<typename Message, typename T = Function, std::size_t ... indices>
    inline typename std::enable_if<std::is_pointer<T>::value && !std::is_same<typename
                                   traits::helpers::function_traits<T>::return_type, bool>::value, bool>::type
    invoke(std::vector<Message> &messages,
           std::index_sequence<indices ...>)
    {
        bool bSuccess = (m_function != nullptr);
        if (bSuccess)
            m_function(messages[indices] ...);

        return bSuccess;
    }

    /**
     * Invoke the receiving function and expand the vector of messages into an argument list
     */
    template<typename Message, typename T = Function, std::size_t ... indices>
    inline typename std::enable_if<!std::is_pointer<T>::value && std::is_same<typename
                                   traits::helpers::function_traits<T>::return_type, bool>::value, bool>::type
    invoke(std::vector<Message> &messages,
           std::index_sequence<indices ...>)
    {
        return m_function(messages[indices] ...);
    }

    /**
     * Invoke the receiving function and expand the vector of messages into an argument list
     */
    template<typename Message, typename T = Function, std::size_t ... indices>
    inline typename std::enable_if<!std::is_pointer<T>::value && !std::is_same<typename
                                   traits::helpers::function_traits<T>::return_type, bool>::value, bool>::type
    invoke(std::vector<Message> &messages,
           std::index_sequence<indices ...>)
    {
        m_function(messages[indices] ...);

        return true;
    }

public:

    /**
     * Function to receive messages
     * @param messages upon success, will be delivered to the recipient
     */
    inline virtual bool receive(std::vector<functional::Any> &messages) override final
    {
        constexpr auto arity = traits::helpers::function_traits<Function>::arity;
        if (messages.size() < arity)
            throw std::bad_function_call();

        return invoke(messages, std::make_index_sequence<arity>{ });
    }

    /**
     * Return this object's type_info
     */
    inline virtual const std::type_info &type(void) const override
    {
        return typeid(void *);
    }

private:

    /**
     * an instance of the function
     */
    Function m_function;
};

}

#endif
