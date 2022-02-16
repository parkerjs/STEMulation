#ifndef STANDARD_DEVIATION_H
#define STANDARD_DEVIATION_H

#include "statistical_variance.h"

namespace math
{

namespace statistical
{

/**
 * Class for computing standard deviation of a collection of samples
 */
template<typename T>
class StandardDeviation
: public StatisticalVariance<T>
{
public:

    /**
     * Forwarding constructor
     */
    template<typename ... Args>
    StandardDeviation(Args && ... args)
    : StatisticalVariance<T>(std::forward<Args>(args) ...)
    {

    }

    /**
     * Destructor
     */
    virtual ~StandardDeviation(void) override
    {

    }

    /**
     * Forwarding assignment operator
     */
    template<typename R>
    StandardDeviation<T> &operator = (R &&rhs)
    {
        StatisticalVariance<T>::operator = (std::forward<R>(rhs));

        return *this;
    }

    /**
     * Function to calculate the result
     */
    inline virtual T calculate(void) const override
    {
        return std::sqrt(StatisticalVariance<T>::calculate());
    }

    /**
     * clone() function
     */
    inline virtual StandardDeviation<T> *clone(void) const override
    {
        return new StandardDeviation<T>(*this);
    }
};

}

}

#endif
