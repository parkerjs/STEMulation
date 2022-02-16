#include "butcherTableau.h"

namespace math
{

namespace integrators
{

/**
 * Copy constructor
 */
ButcherTableau::ButcherTableau(const ButcherTableau &butcherTableau)
{
    operator = (butcherTableau);
}

/**
 * Move constructor
 */
ButcherTableau::ButcherTableau(ButcherTableau &&butcherTableau)
{
    operator = (std::move(butcherTableau));
}

/**
 * Destructor
 */
ButcherTableau::~ButcherTableau(void)
{

}

/**
 * Copy assignment operator
 */
ButcherTableau &ButcherTableau::operator = (const ButcherTableau &butcherTableau)
{
    if (&butcherTableau != this)
    {
        m_coefficients = butcherTableau.m_coefficients;
        m_orders = butcherTableau.m_orders;
        m_stages = butcherTableau.m_stages;
    }

    return *this;
}

/**
 * Move assignment operator
 */
ButcherTableau &ButcherTableau::operator = (ButcherTableau &&butcherTableau)
{
    if (&butcherTableau != this)
    {
        m_coefficients = std::move(butcherTableau.m_coefficients);
        m_orders = std::move(butcherTableau.m_orders);
        m_stages = std::move(butcherTableau.m_stages);
    }

    return *this;
}

/**
 * clone() function
 */
ButcherTableau *ButcherTableau::clone(void) const
{
    return new ButcherTableau(*this);
}

/**
 * Function to retrieve the i, j-th coefficient from this Butcher Tableau
 */
double ButcherTableau::coefficient(std::size_t i, std::size_t j) const
{
    auto columns = m_stages + m_orders;

    return m_coefficients[i * columns + j];
}

/**
 * Retrieve this Butcher Tableau's coefficient matrix
 */
const std::vector<double> &ButcherTableau::coefficients(void) const
{
    return m_coefficients;
}

/**
 * Retrieve the number of methods of differing orders associated with this Butcher Tableau
 */
std::size_t ButcherTableau::orders(void) const
{
    return m_orders;
}

/**
 * Retrieve the number of stages associated with this Butcher Tableau
 */
std::size_t ButcherTableau::stages(void) const
{
    return m_stages;
}

}

}
