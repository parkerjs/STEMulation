#ifndef PARTIAL_H
#define PARTIAL_H

#include "cartesian_product.h"
#include "iterative_differentiator.h"
#include "loggable.h"
#include "logging_level.h"
#include "reflective.h"
#include "sequence_differentiator.h"

namespace math
{

namespace differentiators
{

namespace function
{

/**
 * This class computes the (mixed) partial of a vector- or matrix-valued function with respect to the
 * independent variable(s). The partial is computed using an iterative process in which finite differences are
 * repeatedly computed until the desired error tolerance is achieved. An n-point finite central difference is
 * used to approximate the partial at each point. The default computation interval is set to (x - 1/2, x +
 * 1/2). The function to be evaluated should accept an std::vector as its input argument and should return a 2d
 * matrix as its return argument
 */
template<typename T>
class Partial
: public IterativeDifferentiator<T>,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
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
    Partial(T interval = 1.0,
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
    Partial(const Partial<T> &differentiator)
    {
        operator = (differentiator);
    }

    /**
     * Move constructor
     */
    Partial(Partial<T> &&differentiator)
    {
        operator = (std::move(differentiator));
    }

    /**
     * Destructor
     */
    virtual ~Partial(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Partial<T> &operator = (const Partial<T> &differentiator)
    {
        if (&differentiator != this)
        {
            IterativeDifferentiator<T>::operator = (differentiator);
            SequenceDifferentiator<T>::operator = (differentiator);

            m_abscissas = differentiator.m_abscissas;
            m_dfdx = differentiator.m_dfdx;
            m_finiteDifference = differentiator.m_finiteDifference;
            m_p = differentiator.m_p;
            m_sets = differentiator.m_sets;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Partial<T> &operator = (Partial<T> &&differentiator)
    {
        if (&differentiator != this)
        {
            IterativeDifferentiator<T>::operator = (std::move(differentiator));
            SequenceDifferentiator<T>::operator = (std::move(differentiator));

            m_abscissas = std::move(differentiator.m_abscissas);
            m_dfdx = std::move(differentiator.m_dfdx);
            m_finiteDifference = std::move(differentiator.m_finiteDifference);
            m_p = std::move(differentiator.m_p);
            m_sets = std::move(differentiator.m_sets);
        }

        return *this;
    }

    /**
     * Evaluate the (mixed) partial derivative of a matrix/vector with respect to the dependent variables
     * @param function the function for which the derivative will be calculated
     * @param indices  a vector of numerical indices that specifies the direction(s) in which the partial
     *                 derivatives are to be taken, i.e., the vector [0 1] indicates that the partial will
     *                 first be taken with respect to the first variable, and then with respect to the second
     *                 variable
     * @param order    a vector that defines the order of the derivative to be taken in the direction that
     *                 corresponds to that which is specified in the direction index vector
     * @param x0       the vector for which the partial is evaluated
     */
    template<typename Function>
    Matrix2d calcDerivative(Function &&function,
                            const std::vector<std::size_t> &indices,
                            const std::vector<std::size_t> &order,
                            const std::vector<T> &x0)
    {
        Matrix2d dfdx;

        calcDerivative(std::forward<Function>(function),
                       indices,
                       order,
                       x0,
                       dfdx);

        return dfdx;
    }

    /**
     * Evaluate the (mixed) partial derivative of a matrix/vector with respect to the dependent variables
     * @param      function the function for which the derivative will be calculated
     * @param      indices  a vector of numerical indices that specifies the direction(s) in which the partial
     *                      derivatives are to be taken, i.e., the vector [0 1] indicates that the partial will
     *                      first be taken with respect to the first variable, and then with respect to the
     *                      second variable
     * @param      order    a vector that defines the order of the derivative to be taken in the direction that
     *                      corresponds to that which is specified in the direction index vector
     * @param      x0       the vector for which the partial is evaluated
     * @param[out] dfdx     will contain the partial derivative matrix
     */
    template<typename Function>
    void calcDerivative(Function &&function,
                        std::vector<std::size_t> indices,
                        std::vector<std::size_t> order,
                        const std::vector<T> &x0,
                        Matrix2d &dfdx)
    {
        std::size_t i = 0, n = m_n;
        initialize(indices, order, x0);

        // looping here can be expensive, so a higher tolerance is better...
        std::vector<T> tuple(x0.size());
        do
        {
            // generate a grid on which the function will be evaluated
            generateSetsForGrid(indices, x0);

            // evaluate the function at the grid points
            auto gridSize = m_sets.cardinality();
            m_sets.getTuple(0, tuple, m_p);
            Matrix2d y(std::forward<Function>(function)(tuple));
            auto size = y.size();
            Matrix2d z(size, gridSize);
            auto &&it_y = y.cbegin();
            auto &&it_z = z.begin();
            while (it_y != y.cend())
            {
                *it_z = *it_y++;
                it_z += gridSize;
            }

            for (std::size_t j = 1; j < gridSize; ++j)
            {
                m_sets.getTuple(j, tuple, m_p);
                auto &&g = std::forward<Function>(function)(tuple);
                auto &&it_g = g.cbegin();
                it_z = std::next(z.begin(), j);
                while (it_g != g.cend())
                {
                    *it_z = *it_g++;
                    it_z += gridSize;
                }
            }

            auto columns = y.columns();
            auto rows = y.rows();
            if (m_dfdx.rows() != rows || m_dfdx.columns() != columns)
                m_dfdx.resize(0.0, rows, columns);

            evaluate(z, order); // apply finite difference weighting
            if (i > 0)
            {
                std::size_t j = 0;
                for (; j < size; ++j)
                {
                    if (std::abs(dfdx[j] - m_dfdx[j]) > m_tolerance)
                        break;
                }

                if (j >= size)
                    break;
            }

            dfdx = m_dfdx;
            ++m_n; // increase the number of finite difference terms
        }
        while (++i <= m_iterations);

        m_n = n;
    }

protected:

    /**
     * Evaluate the (mixed) partial of a matrix/vector with respect to the independent variable(s)
     * @param z     a 2d vector of function evaluations
     * @param order a vector that defines the order of the derivative to be taken in the direction that
     *              corresponds to that which is specified in the direction index vector
     */
    virtual void evaluate(Matrix2d &z,
                          const std::vector<std::size_t> &order) final
    {
        auto columns = z.columns();
        auto p = m_n;
        auto du = m_interval / p;

        // evaluate (mixed) partial for each element in the matrix
        for (std::size_t h = 0, hcol = 0; h < m_dfdx.size(); hcol += columns)
        {
            auto n = columns;
            for (std::size_t i = 0; i < order.size(); ++i)
            {
                // collapse each dimension one at a time...
                auto &&coeff = m_finiteDifference.calcWeights(du, order[i], p);
                auto &&column = coeff.column(order[i]);
                for (std::size_t j = 0, l = 0; j < n; ++l)
                {
                    T dz = 0.0;
                    for (std::size_t k = 0; k <= p; ++k, ++j)
                        dz += column[k] * z[hcol + j];

                    z[hcol + l] = dz;
                }

                n /= (p + 1);
            }

            m_dfdx[h++] = z[hcol];
        }
    }

    /**
     * Compute Cartesian sets that will be used to generate a grid on which to evaluate the partial derivative
     * @param indices a vector of numerical indices that specifies the direction(s) in which the partial
     *                derivatives are to be taken, i.e., the vector [0 1] indicates that the partial will first
     *                be taken with respect to the first variable, and then with respect to the second variable
     * @param x0      the vector of values for which the partial is evaluated
     */
    virtual void generateSetsForGrid(const std::vector<std::size_t> &indices,
                                     const std::vector<T> &x0) final
    {
        auto x0size = x0.size();
        auto dSize = indices.size();
        auto k = x0size - 1;
        auto du = m_interval / m_n;
        auto np1 = m_n + 1;
        if (m_abscissas.size() != np1)
            m_abscissas.resize(np1);

        // create sets of abscissas for function evaluation also generate the permutation vector p
        for (std::size_t i = 0; i < dSize; ++i)
        {
            auto u0 = x0[indices[i]] - 0.5 * m_interval;
            for (std::size_t j = 0; j < np1; ++j)
                m_abscissas[j] = u0 + j * du;

            m_sets.replace(std::next(m_sets.begin(), k), m_abscissas);
            m_p[k--] = indices[i];
        }

        // now add singleton dimensions as necessary
        for (std::size_t i = 0, j; i < x0size; ++i)
        {
            // search for differential directions not specified in index vector
            for (j = 0; j < dSize; ++j)
            {
                if (i == indices[j])
                    break;
            }

            if (j >= dSize)
            {
                m_sets.replace(std::next(m_sets.begin(), k), x0[i]);
                m_p[k--] = i;
            }
        }
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "Partial";
    }

    /**
     * Initializations
     * @param indices a vector of numerical indices that specifies the direction(s) in which the partial
     *                derivatives are to be taken, i.e., the vector [0 1] indicates that the partial will first
     *                be taken with respect to the first variable, and then with respect to the second variable
     * @param order   a vector that defines the order of the derivative to be taken in the direction that
     *                corresponds to that which is specified in the direction index vector
     * @param x0      the vector for which the partial is evaluated
     */
    void initialize(std::vector<std::size_t> &indices,
                    std::vector<std::size_t> &order,
                    const std::vector<T> &x0)
    {
        if (indices.size() != order.size()) // check for uniqueness of directions specified in indices
        {
            logMsg(std::cout, utilities::LoggingLevel::Enum::Warning,
                   "The size of the vector specifying the order of each partial (i.e. 1st order, 2nd order "
                   "derivative, etc.) must be the same size as the direction vector specifying which "
                   "partials are to be taken.\n",
                   getQualifiedMethodName(__func__));

            if (indices.size() > order.size())
                indices.resize(order.size());
            else
                order.resize(indices.size());
        }

        // now search partial order vector and remove indices in indices that correspond to a zero-order
        // partial derivative...remove the corresponding index in order as well.
        for (std::size_t i = 0; i < indices.size();)
        {
            if (order[i] == 0)
            {
                indices.erase(std::next(indices.begin(), i));
                order.erase(std::next(order.begin(), i));
            }
            else
            {
                ++i;
            }
        }

        // resize the vector set
        auto x0Size = x0.size();
        if (m_sets.size() != x0Size)
            m_sets.resize(x0Size, { 1 });

        // resize the permutation vector
        if (m_p.size() != x0Size)
            m_p.resize(x0Size);
    }

protected:

    /**
     * the abscissas at which the derivative will be evaluated (for internal use only)
     */
    std::vector<T> m_abscissas;

    /**
     * output matrix (for internal use only)
     */
    Matrix2d m_dfdx;

    /**
     * this object's finite difference calculator
     */
    sequence::FiniteDifference<T> m_finiteDifference;

    /**
     * defines the order in which the tuples (generated by the sets) are to be permuted (for internal use only)
     */
    std::vector<std::size_t> m_p;

    /**
     * sets used to generate a grid on which to evaluate mixed partial differential equations's (for internal
     * use only)
     */
    sets::CartesianProduct<T> m_sets;
};

}

}

}

#endif
