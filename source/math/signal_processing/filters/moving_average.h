#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "average.h"
#include "moving_sum.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing moving averages
 */
template<typename T>
class MovingAverage
: public MovingSum<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Forwarding constructor
     */
    template<typename ... Args> MovingAverage(Args && ... args)
    : MovingSum<T>(std::forward<Args>(args) ...)
    {
        if (this->m_pCalculator != nullptr)
            delete this->m_pCalculator;

        this->m_pCalculator = new statistical::Average<T>();
    }

    /**
     * Destructor
     */
    virtual ~MovingAverage(void) override
    {

    }

    /**
     * Forwarding assignment operator
     */
    template<typename R>
    MovingAverage<T> &operator = (R &&rhs)
    {
        MovingSum<T>::operator = (std::forward<R>(rhs));

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual MovingAverage<T> *clone(void) const override
    {
        return new MovingAverage<T>(*this);
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "MovingAverage";
    }
};

}

}

}

#endif
