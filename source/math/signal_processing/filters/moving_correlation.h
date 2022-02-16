#ifndef MOVING_CORRELATION_H
#define MOVING_CORRELATION_H

#include "moving_covariance.h"
#include "correlation.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing moving correlation
 */
template<typename T>
class MovingCorrelation
: public MovingCovariance<T>
{
public:

    /**
     * Typedef declarations
     */
    using MovingCovariance<T>::filter;

    /**
     * Constructor
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    MovingCorrelation(int period, bool bBiasedEstimate = false)
    : MovingCovariance<T>(period, bBiasedEstimate)
    {
        if (this->m_pCalculator != nullptr)
            delete this->m_pCalculator;

        this->m_pCalculator = new Statistical::Correlation<T>(bBiasedEstimate);
    }

    /**
     * Copy constructor
     */
    MovingCorrelation(const MovingCorrelation<T> &filter)
    : MovingCovariance<T>(filter)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    MovingCorrelation(MovingCorrelation<T> &&filter)
    : MovingCovariance<T>(std::move(filter))
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~MovingCorrelation(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    MovingCorrelation<T> &operator = (const MovingCorrelation<T> &filter)
    {
        if (&filter != this)
        {
            MovingCovariance<T>::operator = (filter);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    MovingCorrelation<T> &operator = (MovingCorrelation<T> &&filter)
    {
        if (&interpolator != this)
        {
            MovingCovariance<T>::operator = (std::move(filter));
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual MovingCorrelation<T> *clone(void) const override
    {
        return new MovingCorrelation<T>(*this);
    }
};

}

}

}

#endif
