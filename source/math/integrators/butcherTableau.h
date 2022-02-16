#ifndef BUTCHER_TABLEAU_H
#define BUTCHER_TABLEAU_H

#include "cloneable.h"
#include "export_library.h"
#include <vector>

namespace math
{

namespace integrators
{

/**
 * This container class provides storage and retrieval of Butcher Tableau coefficients for use with various
 * Runge-Kutta methods
 */
class ButcherTableau
: public attributes::interfaces::Cloneable<ButcherTableau>
{
public:

    /**
     * Constructor
     */
    template<std::size_t N,
             std::size_t STAGES = N - 1,
             std::size_t ORDER = N - STAGES,
             typename std::enable_if<N == STAGES + ORDER, int>::type = 0>
    ButcherTableau(const double (&coefficients)[N][N],
                   std::size_t = STAGES,
                   std::size_t = ORDER)
    : m_coefficients(N * N),
      m_orders(ORDER),
      m_stages(STAGES)
    {
        auto &&itCoefficient = m_coefficients.begin();
        for (std::size_t i = 0; i < N; ++i)
            for (std::size_t j = 0; j < N; ++j)
                *itCoefficient++ = coefficients[i][j];
    }

    /**
     * Copy constructor
     */
    EXPORT_STEM ButcherTableau(const ButcherTableau &butcherTableau);

    /**
     * Move constructor
     */
    EXPORT_STEM ButcherTableau(ButcherTableau &&butcherTableau);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~ButcherTableau(void);

    /**
     * Copy assignment operator
     */
    EXPORT_STEM ButcherTableau &operator = (const ButcherTableau &butcherTableau);

    /**
     * Move assignment operator
     */
    EXPORT_STEM ButcherTableau &operator = (ButcherTableau &&butcherTableau);

    /**
     * clone() function
     */
    EXPORT_STEM virtual ButcherTableau *clone(void) const override;

    /**
     * Function to retrieve the i, j-th coefficient from this Butcher Tableau
     */
    EXPORT_STEM virtual double coefficient(std::size_t i, std::size_t j) const final;

    /**
     * Retrieve this Butcher Tableau's coefficient matrix
     */
    EXPORT_STEM virtual const std::vector<double> &coefficients(void) const final;

    /**
     * Retrieve the number of methods of differing orders associated with this Butcher Tableau
     */
    EXPORT_STEM virtual std::size_t orders(void) const final;

    /**
     * Retrieve the number of stages associated with this Butcher Tableau
     */
    EXPORT_STEM virtual std::size_t stages(void) const final;

private:

    /**
     * this Butcher Tableau's coefficient matrix stored as a one-dimensional vector
     */
    std::vector<double> m_coefficients;

    /**
     * the number of methods of differing orders associated with this Butcher Tableau
     */
    std::size_t m_orders;

    /**
     * the number of stages associated with this Butcher Tableau
     */
    std::size_t m_stages;
};

}

}

#endif
