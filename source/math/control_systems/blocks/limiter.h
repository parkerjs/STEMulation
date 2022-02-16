#ifndef LIMITER_H
#define LIMITER_H

#include "loggable.h"
#include "nonlinearity.h"
#include <limits>

namespace math
{

namespace control_systems
{

namespace blocks
{

/**
 * This class implements a signal limiter
 */
template<typename T>
class Limiter
: public attributes::concrete::Loggable<std::string, std::ostream>,
  public Nonlinearity<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Enumerations
     */
    enum class LimitType { Lower,
                           Upper };

    /**
     * Constructor
     */
    Limiter(void)
    : Limiter(-std::numeric_limits<T>::max(),
              +std::numeric_limits<T>::max())
    {

    }

    /**
     * Constructor
     * @param lowerLimit the lower limit value
     * @param upperLimit the upper limit value
     */
    Limiter(T lowerLimit,
            T upperLimit)
    : m_lowerLimit(lowerLimit),
      m_upperLimit(upperLimit)
    {

    }

    /**
     * Copy constructor
     */
    Limiter(const Limiter<T> &limiter)
    {
        operator = (limiter);
    }

    /**
     * Move constructor
     */
    Limiter(Limiter<T> &&limiter)
    {
        operator = (std::move(limiter));
    }

    /**
     * Destructor
     */
    virtual ~Limiter(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Limiter<T> &operator = (const Limiter<T> &limiter)
    {
        if (&limiter != this)
        {
            Loggable<std::string, std::ostream>::operator = (limiter);
            Nonlinearity<T>::operator = (limiter);

            m_lowerLimit = limiter.m_lowerLimit;
            m_upperLimit = limiter.m_upperLimit;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Limiter<T> &operator = (Limiter<T> &&limiter)
    {
        if (&limiter != this)
        {
            Loggable<std::string, std::ostream>::operator = (std::move(limiter));
            Nonlinearity<T>::operator = (std::move(limiter));

            m_lowerLimit = std::move(limiter.m_lowerLimit);
            m_upperLimit = std::move(limiter.m_upperLimit);
        }

        return *this;
    }

    /**
     * Function to apply this limiter to a signal using this object's upper and lower limits
     * @param signal the input to be limited
     */
    inline virtual T apply(T signal) override
    {
        return Limiter<T>::apply(signal,
                                 m_lowerLimit,
                                 m_upperLimit);
    }

    /**
     * Function to apply this limiter to a signal using the supplied limit value and LimitType (upper or lower)
     * @param signal    the input to be limited
     * @param limit     the limit value that will be applied to the input signal
     * @param limitType enumeration specifying the limit value as an upper or lower limit
     */
    static T apply(T signal,
                   T limit,
                   const LimitType &limitType)
    {
        switch (limitType)
        {
            case LimitType::Lower:
            if (signal < limit)
            {
                return limit;
            }

            break;

            case LimitType::Upper:
            if (signal > limit)
            {
                return limit;
            }

            break;

            default:
            logMsg(std::cout, utilities::LoggingLevel::Enum::Warning,
                   "Unrecognized limit type.\n",
                   getQualifiedStaticMethodName(__func__, Limiter));
        }

        return signal;
    }

    /**
     * Function to apply this limiter to a signal using the specified lower and upper limits
     * @param signal     the input to be limited
     * @param lowerLimit the lower limit value
     * @param upperLimit the upper limit value
     */
    static T apply(T signal,
                   T lowerLimit,
                   T upperLimit)
    {
        signal = apply(signal,
                       lowerLimit,
                       LimitType::Lower);
        signal = apply(signal,
                       upperLimit,
                       LimitType::Upper);

        return signal;
    }

    /**
     * Get the bandwidth of this limiter
     */
    inline virtual T getBandwidth(void) const final
    {
        return m_upperLimit - m_lowerLimit;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Limiter";
    }

    /**
     * Get the value of this limiter's lower limit
     */
    inline virtual T &getLowerLimit(void) final
    {
        return m_lowerLimit;
    }

    /**
     * Get the value of this limiter's lower limit
     */
    inline virtual T getLowerLimit(void) const final
    {
        return m_lowerLimit;
    }

    /**
     * Get the value of this limiter's upper limit
     */
    inline virtual T &getUpperLimit(void) final
    {
        return m_upperLimit;
    }

    /**
     * Get the value of this limiter's upper limit
     */
    inline virtual T getUpperLimit(void) const final
    {
        return m_upperLimit;
    }
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override
    {
        bool bSuccess = Nonlinearity<T>::readFromXML(pNode);
        if (bSuccess)
        {
            auto *pLowerLimitNode = pNode->first_node("lowerLimit");
            if (pLowerLimitNode != nullptr)
                setLowerLimit(std::stod(pLowerLimitNode->value()));

            auto *pUpperLimitNode = pNode->first_node("upperLimit");
            if (pUpperLimitNode != nullptr)
                setUpperLimit(std::stod(pUpperLimitNode->value()));
        }

        return bSuccess;
    }
#endif
    /**
     * Set the value of this limiter's lower limit
     */
    inline virtual void setLowerLimit(T lowerLimit) final
    {
        m_lowerLimit = lowerLimit;
    }

    /**
     * Set the value of this limiter's upper limit
     */
    inline virtual void setUpperLimit(T upperLimit) final
    {
        m_upperLimit = upperLimit;
    }

protected:

    /**
     * the value of this limiter's lower limit
     */
    T m_lowerLimit;

    /**
     * the value of this limiter's upper limit
     */
    T m_upperLimit;
};

}

}

}

#endif
