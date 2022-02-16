#ifndef LINEAR_INTERPOLATOR_H
#define LINEAR_INTERPOLATOR_H

#include "algorithm.h"
#include "interpolator.h"

namespace math
{

namespace interpolators
{

/**
 * This class implements an algorithm to perform linear interpolation
 */
template<typename T>
class LinearInterpolator
: public Interpolator<T>
{
public:

    /**
     * Using declarations
     */
    using Interpolator<T>::interpolate;

    /**
     * Constructor
     */
    LinearInterpolator(void)
    {

    }

    /**
     * Copy constructor
     */
    LinearInterpolator(const LinearInterpolator<T> &interpolator)
    : Interpolator<T>(interpolator)
    {
        operator = (interpolator);
    }

    /**
     * Move constructor
     */
    LinearInterpolator(LinearInterpolator<T> &&interpolator)
    : Interpolator<T>(std::move(interpolator))
    {
        operator = (std::move(interpolator));
    }

    /**
     * Destructor
     */
    virtual ~LinearInterpolator(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    LinearInterpolator<T> &operator = (const LinearInterpolator<T> &interpolator)
    {
        if (&interpolator != this)
        {
            Interpolator<T>::operator = (interpolator);
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    LinearInterpolator<T> &operator = (LinearInterpolator<T> &&interpolator)
    {
        if (&interpolator != this)
        {
            Interpolator<T>::operator = (std::move(interpolator));
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual LinearInterpolator<T> *clone(void) const override
    {
        return new LinearInterpolator<T>(*this);
    }

    /**
     * Function to perform interpolation
     * @param p_xBegin a pointer to the beginning of a range of abscissas bounded by [p_xBegin, p_xEnd)
     *                 associated with a range of known ordinates
     * @param p_xEnd   a pointer to the end of a range of abscissas bounded by [p_xBegin, p_xEnd) associated
     *                 with a range of known ordinates
     * @param p_yBegin a pointer to the beginning of a range of known ordinates
     * @param xi       the abscissa for which interpolation will be performed
     */
    virtual T interpolate(const T *p_xBegin, const T *p_xEnd, const T *p_yBegin, const T &xi) override
    {
        auto size = std::distance(p_xBegin, p_xEnd);
        auto *p_x = utilities::infimum(xi, p_xBegin, p_xEnd);
        if (p_x == p_xEnd)
        {
            if (!this->m_bExtrapolate)
                return *p_yBegin;

            p_x = p_xBegin;
        }
        else if (p_x == std::prev(p_xEnd))
        {
            if (!this->m_bExtrapolate)
                return p_yBegin[size - 1];

            --p_x;
        }

        p_yBegin += std::distance(p_xBegin, p_x);
        auto &&dx = p_x[1] - p_x[0];
        auto &&dy = p_yBegin[1] - p_yBegin[0];

        return p_yBegin[0] + (xi - p_x[0]) * dy / dx;
    }
};

}

}

#endif
