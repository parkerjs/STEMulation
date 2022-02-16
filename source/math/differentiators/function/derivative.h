#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "finite_difference.h"
#include "iterative_differentiator.h"
#include "sequence_differentiator.h"

namespace math
{

namespace differentiators
{

namespace function
{

/**
 * This class computes the derivative of a scalar-valued function with respect to the independent variable. The
 * derivative is computed using an iterative process in which finite differences are repeatedly computed until
 * the desired error tolerance is achieved. An n-point finite central difference is used to approximate the
 * derivative at each point. The default computation interval is set to (x - 1/2, x + 1/2)
 */
template<typename T>
class Derivative
: public IterativeDifferentiator<T>,
  public sequence::SequenceDifferentiator<T>
{
public:

    /**
     * Constructor
     * @param interval   the interval over which the iteration will occur
     * @param iterations the maximum number of iterations allowed to compute the derivative within the error
     *                   tolerance
     * @param n          the finite difference order
     * @param tolerance  the error tolerance
     */
    Derivative(T interval = 1.0,
               std::size_t iterations = 15,
               std::size_t n = 4,
               T tolerance = 1.0e-2)
    : IterativeDifferentiator<T>(interval,
                                 iterations,
                                 tolerance),
      SequenceDifferentiator<T>(n)
    {

    }

    /**
     * Copy constructor
     */
    Derivative(const Derivative<T> &differentiator)
    {
        operator = (differentiator);
    }

    /**
     * Move constructor
     */
    Derivative(Derivative<T> &&differentiator)
    {
        operator = (std::move(differentiator));
    }

    /**
     * Destructor
     */
    virtual ~Derivative(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Derivative<T> &operator = (const Derivative<T> &differentiator)
    {
        if (&differentiator != this)
        {
            IterativeDifferentiator<T>::operator = (differentiator);
            SequenceDifferentiator<T>::operator = (differentiator);

            m_abscissas = differentiator.m_abscissas;
            m_coefficients = differentiator.m_coefficients;
            m_finiteDifference = differentiator.m_finiteDifference;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Derivative<T> &operator = (Derivative<T> &&differentiator)
    {
        if (&differentiator != this)
        {
            IterativeDifferentiator<T>::operator = (std::move(differentiator));
            SequenceDifferentiator<T>::operator = (std::move(differentiator));

            m_abscissas = std::move(differentiator.m_abscissas);
            m_coefficients = std::move(differentiator.m_coefficients);
            m_finiteDifference = std::move(differentiator.m_differentiator);
        }

        return *this;
    }

    /**
     * Calculate the m-th order numerical derivative of y with respect to x; upon success, returns a matrix of
     * derivatives
     * @param function the function for which the derivative will be calculated
     * @param x        a vector of abscissas for which the derivative will be evaluated
     * @param order    the order of the derivative (default = 1)
     */
    template<typename Function>
    std::vector<T> calcDerivative(Function &&function,
                                  const std::vector<T> &x,
                                  std::size_t order = 1)
    {
        std::vector<T> dydx;

        calcDerivative(std::forward<Function>(function),
                       x,
                       dydx,
                       order);

        return dydx;
    }

    /**
     * Calculate the m-th order numerical derivative of y with respect to x
     * @param      function the function for which the derivative will be calculated
     * @param      x        a vector of abscissas for which the derivative will be evaluated
     * @param[out] dydx     will contain the numerical derivative of y with respect to x
     * @param      order    the order of the derivative (default = 1)
     */
    template<typename Function>
    void calcDerivative(Function &&function,
                        const std::vector<T> &x,
                        std::vector<T> &dydx,
                        std::size_t order = 1)
    {
        auto n = m_n;
        auto xSize = x.size();
        dydx.resize(xSize, 1);
        T df[2];
        for (std::size_t i = 0; i < xSize; ++i)
        {
            std::size_t j = 0;
            df[1] = evaluate(std::forward<Function>(function),
                             x[i],
                             order);
            do
            {
                ++m_n;
                df[0] = df[1];
                df[1] = evaluate(std::forward<Function>(function),
                                 x[i],
                                 order);
            }
            while (std::abs(df[1] - df[0]) > m_tolerance && ++j < m_iterations);

            m_n = n;
            dydx[i] = df[1];
        }
    }

private:

    /**
     * Approximate the m-th order numerical derivative of f with respect to x
     * @param function the function for which the derivative will be calculated
     * @param x0       the neighborhood about which the finite difference is taken
     * @param order    the order of the derivative (default = 1)
     */
    template<typename Function>
    T evaluate(Function &&function,
               T x0,
               std::size_t order = 1)
    {
        auto np1 = m_n + 1;
        m_abscissas.resize(np1);

        auto u0 = x0 - 0.5f * m_interval;
        auto du = m_interval / m_n;
        for (std::size_t i = 0; i <= m_n; ++i)
            m_abscissas[i] = u0 + i * du;

        m_coefficients.resize(np1);
        m_finiteDifference.calcWeights(m_abscissas, order);
        m_finiteDifference.getCoefficientMatrix().column(order, m_coefficients);

        T dydx = 0.0;
        for (std::size_t i = 0; i <= m_n; ++i)
            dydx += m_coefficients[i] * std::forward<Function>(function)(m_abscissas[i]);

        return dydx;
    }

protected:

    /**
     * the abscissas at which the derivative will be evaluated (for internal use only)
     */
    std::vector<T> m_abscissas;

    /**
     * a column of coefficients from the calculated finite difference weight table (for internal use only)
     */
    std::vector<T> m_coefficients;

    /**
     * this object's finite difference calculator
     */
    sequence::FiniteDifference<T> m_finiteDifference;
};

}

}

}

#endif
