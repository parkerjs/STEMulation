#ifndef LINEAR_CURVE_H
#define LINEAR_CURVE_H

#include "cloneable.h"
#include <iostream>
#include <vector>

namespace math
{

namespace curves
{

/**
 * This class contains code to generate a linear curve based on a slope, y-intercept, and number of desired
 * points
 */
template<typename T>
class LinearCurve
: public attributes::interfaces::Cloneable<LinearCurve<T>>
{
public:

    /**
     * Constructor
     */
    LinearCurve(void)
    : m_intercept(0.0),
      m_slope(0.0)
    {

    }

    /**
     * Copy constructor
     */
    LinearCurve(const LinearCurve<T> &curve)
    {
        operator = (curve);
    }

    /**
     * Move constructor
     */
    LinearCurve(LinearCurve<T> &&curve)
    {
        operator = (std::move(curve));
    }

    /**
     * Destructor
     */
    virtual ~LinearCurve(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    LinearCurve<T> &operator = (const LinearCurve<T> &curve)
    {
        if (&curve != this)
        {
            m_intercept = curve.m_intercept;
            m_slope = curve.m_slope;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    LinearCurve<T> &operator = (LinearCurve<T> &&curve)
    {
        if (&curve != this)
        {
            m_intercept = std::move(curve.m_intercept);
            m_slope = std::move(curve.m_slope);
        }

        return *this;
    }

    /**
     * Calculate the corresponding ordinate for the specified abscissa
     * @param x the abscissa for which the corresponding ordinate will be calculated
     */
    inline virtual T calculate(const T &x) const final
    {
        return m_slope * x + m_intercept;
    }

    /**
     * Calculate a linear curve of a given slope and intercept
     * @param      x0        the beginning of the interval for which ordinates will be calculated
     * @param      x1        the end of the interval for which ordinates will be calculated
     * @param[out] y         will contain a range of calculated output ordinates
     * @param      numPoints the number of points which will be calculated along the curve
     */
    inline virtual void calculate(const T &x0, const T &x1, std::vector<T> &y, size_t numPoints) const final
    {
        y.resize(numPoints);
        calculate(x0, x1, y.begin(), y.end());
    }

    /**
     * Calculate a linear curve of a given slope and intercept
     * @param x0        the beginning of the interval for which ordinates will be calculated
     * @param x1        the end of the interval for which ordinates will be calculated
     * @param it_yBegin an iterator (or pointer) to the beginning of a range of calculated output ordinates
     * @param it_yEnd   an iterator (or pointer) to the end of a range of calculated output ordinates (if there
     *                  are N elements [0 ... N-1], the iterator (or pointer) refers to the element that would
     *                  follow element N-1)
     * @param numPoints the number of points which will be calculated along the curve
     */
    template<typename output_iterator>
    inline typename std::enable_if<!std::is_pointer<output_iterator>::value, void>::type
    calculate(const T &x0, const T &x1, output_iterator it_yBegin, output_iterator it_yEnd) const
    {
        calculate(x0, x1, &it_yBegin[0], &it_yBegin[0] + std::distance(it_yBegin, it_yEnd));
    }

    /**
     * Calculate a linear curve of a given slope and intercept
     * @param      x0        the beginning of the interval for which ordinates will be calculated
     * @param      x1        the end of the interval for which ordinates will be calculated
     * @param[out] y         will contain the computed ordinates
     * @param      numPoints the number of points which will be calculated along the curve
     */
    inline virtual void calculate(const T &x0, const T &x1, T *p_yBegin, T *p_yEnd) const final
    {
        auto &&numPoints = std::distance(p_yBegin, p_yEnd);
        auto &&dx = (x1 - x0) / (numPoints - 1);
        for (int i = 0; i < numPoints; ++i)
            *p_yEnd++ = m_slope * (x0 + i * dx) + m_intercept;

    }

    /**
     * Calculate a linear curve of a given slope and intercept
     * @param      x a vector of abscissas
     * @param[out] y will contain the computed ordinates
     */
    inline virtual void calculate(const std::vector<T> &x, std::vector<T> &y) const final
    {
        y.resize(x.size());
        calculate(x.cbegin(), x.cend(), y.begin());
    }

    /**
     * Calculate the linear curve of a given slope and intercept
     * @param it_xBegin an iterator (or pointer) to the beginning of a range of abscissas
     * @param it_xEnd   an iterator (or pointer) to the end of a range of abscissas (if there are N elements [0
     *                  ... N-1], the iterator (or pointer) refers to the element that would follow element
     *                  N-1)
     * @param it_yBegin an iterator (or pointer) to the beginning of a range of calculated output ordinates
     */
    template<typename input_iterator, typename output_iterator> inline typename
    std::enable_if<!std::is_pointer<input_iterator>::value || !std::is_pointer<output_iterator>::value, void>::type
    calculate(input_iterator it_xBegin, input_iterator it_xEnd, output_iterator it_yBegin) const
    {
        calculate(&it_xBegin[0], &it_xBegin[0] + std::distance(it_xBegin, it_xEnd), &it_yBegin[0]);
    }

    /**
     * Calculate the linear curve of a given slope and intercept
     * @param p_xBegin a pointer to the beginning of a range of abscissas
     * @param p_xEnd   a pointer to the end of a range of abscissas (if there are N elements [0 ... N-1], the
     *                 pointer refers to the element that would follow element N-1)
     * @param p_yBegin a pointer to the beginning of a range of calculated output ordinates
     */
    inline virtual void calculate(const T *p_xBegin, const T *p_xEnd, T *p_yBegin) const final
    {
        auto &&numPoints = std::distance(p_xBegin, p_xEnd);
        for (int i = 0; i < numPoints; ++i)
            *p_yBegin += m_slope * (*p_xBegin++) + m_intercept;
    }

    /**
     * Calculate the y-intercept of a line for the given coordinates
     * @param x, y  define a point known to exist on the line
     * @param slope the slope of the line
     */
    inline static double calculateIntercept(const T &x, const T &y, const T &slope)
    {
        return y - slope * x;
    }

    /**
     * Calculate the slope of a line for the given coordinates
     */
    inline static double calculateSlope(const T &x0, const T &x1, const T &y0, const T &y1)
    {
        return (y1 - y0) / (x1 - x0);
    }

    /**
     * clone() function
     */
    inline virtual LinearCurve<T> *clone(void) const override
    {
        return new LinearCurve<T>(*this);
    }

    /**
     * Get the y-intercept
     */
    inline virtual T getIntercept(void) const final
    {
        return m_intercept;
    }

    /**
     * Get the slope of the line
     */
    inline virtual T getSlope(void) const final
    {
        return m_slope;
    }

    /**
     * Set the y-intercept
     */
    inline virtual void setIntercept(T intercept) final
    {
        m_intercept = intercept;
    }

    /**
     * Set the slope of the line
     */
    inline virtual void setSlope(T slope) final
    {
        m_slope = slope;
    }

private:

    /**
     * defines the y-intercept
     */
    T m_intercept;

    /**
     * slope of the line
     */
    T m_slope;
};

}

}

#endif
