#ifndef AVERAGE_H
#define AVERAGE_H

#include "sum.h"

namespace math
{

namespace statistical
{

/**
 * Class for computing the average of a collection of samples
 */
template<typename T>
class Average
: public Sum<T>
{
public:

    /**
     * Forwarding constructor
     */
    template<typename ... Args> Average(Args && ... args)
    : Sum<T>(std::forward<Args>(args) ...)
    {

    }

    /**
     * Destructor
     */
    virtual ~Average(void) override
    {

    }

private:

    /**
     * Forwarding assignment operator
     */
    template<typename R>
    Average<T> &operator = (R &&rhs)
    {
        Sum<T>::operator = (std::forward<R>(rhs));

        return *this;
    }

public:

    /**
     * Function to calculate the result
     */
    inline virtual T calculate(void) const override
    {
        return this->m_numSamples > 0 ? this->m_sum / this->m_numSamples : T(0.0);
    }

    /**
     * clone() function
     */
    inline virtual Average<T> *clone(void) const override
    {
        return new Average<T>(*this);
    }
};

}

}

#endif
