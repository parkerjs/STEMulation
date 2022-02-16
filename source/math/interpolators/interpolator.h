#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "cloneable.h"
#include <vector>

namespace math
{

namespace interpolators
{

/**
 * This class serves as an abstract base for classes that implement algorithms for numerical interpolation
 */
template<typename T>
class Interpolator
: public attributes::interfaces::Cloneable<Interpolator<T>>
{
public:

    /**
     * Constructor
     */
    Interpolator(void)
    : m_bExtrapolate(false)
    {

    }

    /**
     * Copy constructor
     */
    Interpolator(const Interpolator<T> &interpolator)
    {
        operator = (interpolator);
    }

    /**
     * Move constructor
     */
    Interpolator(Interpolator<T> &&interpolator)
    {
        operator = (std::move(interpolator));
    }

    /**
     * Destructor
     */
    virtual ~Interpolator(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Interpolator<T> &operator = (const Interpolator<T> &interpolator)
    {
        if (&interpolator != this)
        {
            m_bExtrapolate = interpolator.m_bExtrapolate;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Interpolator<T> &operator = (Interpolator<T> &&interpolator)
    {
        if (&interpolator != this)
        {
            m_bExtrapolate = std::move(interpolator.m_bExtrapolate);
        }

        return *this;
    }

    /**
     * Function call operator
     * @param xi the abscissa for which interpolation will be performed
     * @param x  a vector of abscissas corresponding to a vector of known ordinates
     * @param y  a vector of known ordinates
     */
    inline T operator () (const T &xi, const std::vector<T> &x, const std::vector<T> &y)
    {
        return interpolate(x.cbegin(), x.cend(), y.cbegin(), xi);
    }

    /**
     * Function call operator
     * @param xi a vector of abscissas for which interpolation will be performed
     * @param x  a vector of abscissas corresponding to a vector of known ordinates
     * @param y  a vector of known ordinates
     */
    inline std::vector<T> operator () (const std::vector<T> &xi, const std::vector<T> &x, const std::vector<T> &y)
    {
        std::vector<T> yi(xi.size());
        std::transform(xi.cbegin(), xi.cend(), yi.begin(),
                       [this, &x, &y] (auto &&t) { return operator () (t, x, y); });

        return yi;
    }

    /**
     * Set extrapolation status
     */
    inline virtual void extrapolate(bool bExtrapolate)
    {
        m_bExtrapolate = bExtrapolate;
    }

    /**
     * Query extrapolation status
     */
    inline virtual bool extrapolate(void) const
    {
        return m_bExtrapolate;
    }

    /**
     * Function to perform interpolation
     * @param it_xBegin an iterator (or pointer) to the beginning of the x data sequence
     * @param it_xEnd   a pointer to the element following the end of the range defining the x data sequence
     *                  (if there are N elements [0 ... N-1], the iterator (or pointer) refers to the element
     *                  that would follow element N-1)
     * @param it_yBegin an iterator (or pointer) to the beginning of the y data sequence
     * @param xi        the abscissa for which interpolation will be performed
     */
    template<typename input_iterator> inline typename
    std::enable_if<!std::is_pointer<input_iterator>::value, T>::type
    interpolate(input_iterator it_xBegin, input_iterator it_xEnd, input_iterator it_yBegin, const T &xi)
    {
        return interpolate(&it_xBegin[0], &it_xBegin[0] + std::distance(it_xBegin, it_xEnd), &it_yBegin[0],
                           xi);
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
    virtual T interpolate(const T *p_xBegin, const T *p_xEnd, const T *p_yBegin, const T &xi) = 0;

    /**
     * Function to perform interpolation
     * @param it_xBegin  an iterator (or pointer) to the beginning of the x data sequence
     * @param it_xEnd    a pointer to the element following the end of the range defining the x data sequence
     *                   (if there are N elements [0 ... N-1], the iterator (or pointer) refers to the element
     *                   that would follow element N-1)
     * @param it_yBegin  an iterator (or pointer) to the beginning of the y data sequence
     * @param it_xiBegin an iterator (or pointer) to the beginning of the xi data sequence (the xi range
     *                   contains the elements for which interpolation will be performed)
     * @param it_xiEnd   a pointer to the element following the end of the range defining the xi data sequence
     *                   (if there are M elements [0 ... M-1], the iterator (or pointer) refers to the element
     *                   that would follow element M-1)
     * @param it_yiBegin an iterator (or pointer) to the beginning of a range that will store the values
     *                   arising from the interpolation
     */
    template<typename input_iterator, typename output_iterator>
    inline void interpolate(input_iterator it_xBegin, input_iterator it_xEnd, input_iterator it_yBegin,
                            input_iterator it_xiBegin, input_iterator it_xiEnd, output_iterator it_yiBegin)
    {
        while (it_xiBegin != it_xiEnd)
            *it_yiBegin++ = interpolate(it_xBegin, it_xEnd, it_yBegin, *it_xiBegin++);
    }

protected:

    /**
     * extrapolation flag
     */
    bool m_bExtrapolate;
};

}

}

#endif
