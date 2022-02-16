#ifndef FINITE_DIFFERENCE_H
#define FINITE_DIFFERENCE_H

#include "finite_difference_scheme.h"
#include "real_matrix_2d.h"
#include "sequence_differentiator.h"

namespace math
{

namespace differentiators
{

namespace sequence
{

/**
 * This class computes the numerical derivative of a sequence using an n-point finite difference about each
 * point
 */
template<typename T>
class FiniteDifference
: public SequenceDifferentiator<T>
{
public:

    /**
     * Type alias declarations
     */
    using Matrix2d = linear_algebra::matrix::Matrix<2, T>;

    /**
     * Constructor
     * @param n the finite difference order
     */
    FiniteDifference(std::size_t n = 4)
    : SequenceDifferentiator<T>(n)
    {

    }

    /**
     * Copy constructor
     */
    FiniteDifference(const FiniteDifference<T> &differentiator)
    {
        operator = (differentiator);
    }

    /**
     * Move constructor
     */
    FiniteDifference(FiniteDifference<T> &&differentiator)
    {
        operator = (std::move(differentiator));
    }

    /**
     * Destructor
     */
    virtual ~FiniteDifference(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    FiniteDifference<T> &operator = (const FiniteDifference<T> &differentiator)
    {
        if (&differentiator != this)
        {
            SequenceDifferentiator<T>::operator = (differentiator);

            m_coefficients = differentiator.m_coefficients;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    FiniteDifference<T> &operator = (FiniteDifference<T> &&differentiator)
    {
        if (&differentiator != this)
        {
            SequenceDifferentiator<T>::operator = (std::move(differentiator));

            m_coefficients = std::move(differentiator.m_coefficients);
        }

        return *this;
    }

    /**
     * Calculate the m-th order numerical derivative of y with respect to x
     * @param x     a vector of abscissas
     * @param y     a vector of ordinates
     * @param order the order of the derivative (default = 1)
     */
    virtual std::vector<T> calcDerivative(const std::vector<T> &x,
                                          const std::vector<T> &y,
                                          std::size_t order = 1)
    {
        std::vector<T> dydx;

        calcDerivative(x, y, dydx, order);

        return dydx;
    }

    /**
     * Calculate the m-th order numerical derivative of y with respect to x
     * @param      x     a vector of abscissas
     * @param      y     a vector of ordinates
     * @param[out] dydx  the numerical derivative of y with respect to x
     * @param      order the order of the derivative (default = 1)
     */
    virtual void calcDerivative(const std::vector<T> &x,
                                const std::vector<T> &y,
                                std::vector<T> &dydx,
                                std::size_t order = 1)
    {
        auto n = this->m_n;
        auto xSize = x.size();
        if (1 + (n << 1) > xSize)
        {
            n = xSize >> 1;
            n = xSize % 2 ? n : n - 1;
        }

        if (dydx.size() < y.size())
            dydx.resize(y.size());

        std::vector<T> u(1 + (n << 1));
        auto uSize = u.size();
        for (std::size_t i = 0; i < xSize; ++i)
        {
            std::size_t j = i < n ? 0 : i - n;
            j = (j + uSize) > xSize ? xSize - uSize : j;
            auto k = j + uSize;
            if (k != uSize + j)
                u.resize(k - j);

            std::copy(std::next(x.cbegin(), j),
                      std::next(x.cbegin(), j + u.size()),
                      u.begin());

            if (i < n || i + n >= xSize)
                calcWeights(u, order, x[i]);
            else
                calcWeights(u, order);

            dydx[i] = 0.0;

            auto &&itCoefficients = m_coefficients.cbegin();
            for (auto l = j; l < k; ++l, ++itCoefficients)
            {
                itCoefficients += order;
                auto coefficient = *itCoefficients;
                dydx[i] += coefficient * y[l];
            }
        }
    }

    /**
     * Compute the finite difference coefficient matrix up to and including the m-th order difference
     * @param  x      a vector of abscissas
     * @param  order  the order of the derivative
     * @param  scheme the finite difference scheme NOTE: x0 is chosen according to the finite difference scheme
     *                as follows:
     *                1) Arbitrary: x0 = minimum grid point value
     *                2) Backward:  x0 = maximum grid point value
     *                3) Central:   x0 = average of minimum and maximum grid point values
     *                4) Forward:   x0 = minimum grid point value
     * @return        a finite difference coefficient matrix, where the columns of the coefficient matrix
     *                correspond to the 0th, 1st, ... m-th order differences, while the rows correspond to the
     *                grid points
     */
    virtual Matrix2d &calcWeights(const std::vector<T> &x,
                                  std::size_t order,
                                  const FiniteDifferenceScheme &scheme =
                                  FiniteDifferenceScheme::Enum::Central) final
    {
        return calcWeights(x,
                           order,
                           scheme,
                           0.0);
    }

    /**
     * Compute the finite difference coefficient matrix up to and including the m-th order difference using
     * evenly-spaced grid points
     * @param  dx     the abscissa spacing
     * @param  order  the order of the derivative
     * @param  n      the number of sub-intervals
     * @param  scheme the finite difference scheme NOTE: x0 is chosen according to the finite difference scheme
     *                as follows:
     *                1) Arbitrary: x0 = minimum grid point value
     *                2) Backward:  x0 = maximum grid point value
     *                3) Central:   x0 = average of minimum and maximum grid point values
     *                4) Forward:   x0 = minimum grid point value
     * @return        a finite difference coefficient matrix, where the columns of the coefficient matrix
     *                correspond to the 0th, 1st, ... m-th order differences, while the rows correspond to the
     *                grid points
     */
    virtual Matrix2d &calcWeights(T dx,
                                  std::size_t order,
                                  std::size_t n,
                                  const FiniteDifferenceScheme &scheme =
                                  FiniteDifferenceScheme::Enum::Central) final
    {
        std::vector<T> x(n + 1);
        for (std::size_t i = 0; i <= n; ++i)
            x[i] = i * dx;

        return calcWeights(x,
                           order,
                           scheme,
                           0.0);
    }

    /**
     * Compute the finite difference coefficient matrix using the grid points in x in the vicinity of x0 up to
     * and including the m-th order difference
     * @param  x     a vector of abscissas
     * @param  order the order of the derivative
     * @param  x0    the neighborhood about which the finite difference is taken
     * @return       a finite difference coefficient matrix, where the columns of the coefficient matrix
     *               correspond to the 0th, 1st, ... m-th order differences, while the rows correspond to the
     *               grid points
     */
    inline virtual Matrix2d &calcWeights(const std::vector<T> &x,
                                         std::size_t order,
                                         T x0) final
    {
        return calcWeights(x,
                           order,
                           FiniteDifferenceScheme::Enum::Arbitrary,
                           x0);
    }

    /**
     * Generate finite difference coefficients of optimal order using Lagrange interpolation; upon success,
     * returns the finite difference coefficient matrix
     * @param  x      a vector of abscissas
     * @param  order  the order of the derivative
     * @param  scheme the finite difference scheme
     * @param  x0     the neighborhood about which the finite difference is taken
     * @return        a finite difference coefficient matrix, where the columns of the coefficient matrix
     *                correspond to the 0th, 1st, ... m-th order differences, while the rows correspond to the
     *                grid points
     */
    virtual Matrix2d &calcWeights(const std::vector<T> &x,
                                  std::size_t order,
                                  const FiniteDifferenceScheme &scheme,
                                  T x0) final
    {
        calcWeights(x,
                    order,
                    scheme,
                    m_coefficients,
                    x0);

        return m_coefficients;
    }

    /**
     * Generate finite difference coefficients of optimal order using Lagrange interpolation
     * @param      x            a vector of abscissas
     * @param      order        the order of the derivative
     * @param      scheme       the finite difference scheme
     * @param[out] coefficients the finite difference coefficient matrix, where the columns of the coefficient
     *                          matrix correspond to the 0th, 1st, ... m-th order differences, while the rows
     *                          correspond to the grid points
     * @param      x0           the neighborhood about which the finite difference is taken
     */
    static void calcWeights(const std::vector<T> &x,
                            std::size_t order,
                            const FiniteDifferenceScheme &scheme,
                            Matrix2d &coefficients,
                            T x0 = 0.0)
    {
        auto n = x.size();
        auto s = n % 2;
        auto mp1 = order + 1;
        std::size_t p = 0;
        int w = 1;
        switch (scheme)
        {
            case FiniteDifferenceScheme::Enum::Backward:
            w = -1;

            case FiniteDifferenceScheme::Enum::Forward:
            x0 = *x.begin();
            break;

            case FiniteDifferenceScheme::Enum::Central:
            p = n >> 1;
            x0 = 0.5 * (x[p - !s] + x[p]);

            case FiniteDifferenceScheme::Enum::Arbitrary:
            default: // do nothing
            break;
        }

        coefficients.resize(n, mp1);
        auto pcol = p * mp1;
        coefficients[pcol] = w;
        T c1 = 1.0;
        auto c4 = x[p] - x0;
        for (int i = 1, q = p; i < signed(n); ++i)
        {
            std::size_t l = i < signed(order) ? i : order;
            q = p ? (q + ((i + !s) % 2 ? i : -i)) : i;
            auto qcol = q * mp1;
            T c2 = 1.0;
            auto c5 = c4;
            c4 = x[q] - x0;
            for (int j = 0, r = p; j < i; ++j)
            {
                r = p ? (r + ((j + !s) % 2 ? j : -j)) : j;
                std::size_t rcol = r * mp1;
                auto c3 = x[q] - x[r];
                c2 *= c3;
                if (j + 1 == i)
                {
                    auto qcolpk = qcol + l;
                    auto rcolpk = rcol + l;
                    for (std::size_t k = l; rcolpk > rcol; --k, --qcolpk, --rcolpk)
                    {
                        coefficients[qcolpk] = c1 * ( k * coefficients[rcolpk - 1] -
                                                     c5 * coefficients[rcolpk]) / c2;
                    }

                    coefficients[qcol] = -c1 * c5 * coefficients[rcol] / c2;
                }

                for (std::size_t k = l, rcolpk = rcol + k; rcolpk > rcol; --k, --rcolpk)
                    coefficients[rcolpk] = (c4 * coefficients[rcolpk] -
                                             k * coefficients[rcolpk - 1]) / c3;

                coefficients[rcol] *= c4 / c3;
            }

            c1 = c2;
        }

        coefficients[pcol] *= w;
    }

    /**
     * Get the finite difference coefficient matrix
     */
    inline virtual Matrix2d &getCoefficientMatrix(void) final
    {
        return m_coefficients;
    }

protected:

    /**
     * finite difference coefficient matrix
     */
    Matrix2d m_coefficients;
};

}

}

}

#endif
