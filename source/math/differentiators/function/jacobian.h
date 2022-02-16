#ifndef JACOBIAN_H
#define JACOBIAN_H

#include "derivative.h"
#include "real_matrix_2d.h"

namespace math
{

namespace differentiators
{

namespace function
{

/**
 * This class computes the Jacobian matrix of a vector-valued systems of equations. The partial of f with
 * respect to x is calculated using an iterative process in which finite differences are repeatedly computed
 * with respect to each independent variable until the desired error tolerance is achieved. An n-point finite
 * central difference is used to approximate the partial at each point. The default computation interval is (x
 * - 1/2, x + 1/2)
 */
template<typename T>
class Jacobian
: public IterativeDifferentiator<T>,
  public sequence::SequenceDifferentiator<T>
{
public:

    /**
     * Type alias declarations
     */
    using Matrix2d = linear_algebra::matrix::Matrix<2, T>;

    /**
     * Constructor
     * @param interval   the interval over which the iteration will occur
     * @param iterations the maximum number of iterations allowed to compute the derivative within the error
     *                   tolerance
     * @param n          the finite difference order
     * @param tolerance  the error tolerance
     */
    Jacobian(T interval = 1.0,
             std::size_t iterations = 15,
             std::size_t n = 4,
             T tolerance = 1.0e-2)
    : IterativeDifferentiator<T>(interval,
                                 iterations,
                                 tolerance),
      SequenceDifferentiator<T>(n),
      m_x0(1)
    {

    }

    /**
     * Copy constructor
     */
    Jacobian(const Jacobian<T> &differentiator)
    {
        operator = (differentiator);
    }

    /**
     * Move constructor
     */
    Jacobian(Jacobian<T> &&differentiator)
    {
        operator = (std::move(differentiator));
    }

    /**
     * Destructor
     */
    virtual ~Jacobian(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Jacobian<T> &operator = (const Jacobian<T> &differentiator)
    {
        if (&differentiator != this)
        {
            IterativeDifferentiator<T>::operator = (differentiator);
            SequenceDifferentiator<T>::operator = (differentiator);

            m_differentiator = differentiator.m_differentiator;
            m_x0 = differentiator.m_x0;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Jacobian<T> &operator = (Jacobian<T> &&differentiator)
    {
        if (&differentiator != this)
        {
            IterativeDifferentiator<T>::operator = (std::move(differentiator));
            SequenceDifferentiator<T>::operator = (std::move(differentiator));

            m_differentiator = std::move(differentiator.m_differentiator);
            m_x0 = std::move(differentiator.m_x0);
        }

        return *this;
    }

    /**
     * Calculate the Jacobian matrix J of order m evaluated at a vector x for a given system of equations f_1,
     * f_2, ... f_n
     * @param function the function for which the derivative will be calculated
     * @param x        a vector of abscissas for which the derivative is evaluated
     * @param order    the order of the derivative (default = 1)
     */
    template<typename Function>
    Matrix2d calcDerivative(Function &&function,
                            const std::vector<T> &x,
                            std::size_t order = 1)
    {
        Matrix2d J;

        calcDerivative(std::forward<Function>(function),
                       x,
                       J,
                       order);

        return J;
    }

    /**
     * Calculate the Jacobian matrix J of order m evaluated at a vector x for a given system of equations f_1,
     * f_2, ... f_n
     * @param      function the function for which the derivative will be calculated
     * @param      x        a vector of abscissas for which the derivative is evaluated
     * @param[out] J        will contain the Jacobian matrix
     * @param      order    the order of the derivative (default = 1)
     */
    template<typename Function>
    void calcDerivative(Function &&function,
                        std::vector<T> x,
                        Matrix2d &J,
                        std::size_t order = 1)
    {
        m_differentiator.setFiniteDifferenceOrder(m_n);
        m_differentiator.setInterval(m_interval);
        m_differentiator.setMaxIterations(m_iterations);
        m_differentiator.setTolerance(m_tolerance);

        std::size_t i = 0, j = 0, k = 0;
        auto df = [&function, &i, &j, &x] (T &x0)
        {
            std::swap(x[j], x0);
            auto y = std::forward<Function>(function)(x)[i];
            std::swap(x[j], x0);

            return y;
        };

        auto n = x.size();
        J.resize(n, n);
        for (i = 0, k = 0; i < n; ++i)
        {
            for (j = 0; j < n; ++j)
            {
                m_x0[0] = x[j];
                J[k++] = m_differentiator.calcDerivative(df,
                                                         m_x0,
                                                         order).front();
            }
        }
    }

protected:

    /**
     * this object's derivative calculator object (for internal use only)
     */
    Derivative<T> m_differentiator;

    /**
     * temporary variables
     */
    std::vector<T> m_x0;
};

}

}

}

#endif
