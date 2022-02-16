#include "kalman.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace kalman
{

/**
 * Constructor
 */
Kalman::Kalman(void)
{

}

/**
 * Copy constructor
 */
Kalman::Kalman(const Kalman &kalman)
{
    operator = (kalman);
}

/**
 * Move constructor
 */
Kalman::Kalman(Kalman &&kalman)
{
    operator = (std::move(kalman));
}

/**
 * Destructor
 */
Kalman::~Kalman(void)
{

}

/**
 * Copy assignment operator
 */
Kalman &Kalman::operator = (const Kalman &kalman)
{
    if (&kalman != this)
    {
        EstimationFilter::operator = (kalman);

        m_P = kalman.m_P;
        m_Q = kalman.m_Q;
        m_R = kalman.m_R;
    }

    return *this;
}

/**
 * Move assignment operator
 */
Kalman &Kalman::operator = (Kalman &&kalman)
{
    if (&kalman != this)
    {
        kalman.swap(*this);
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string Kalman::getClassName(void) const
{
    return "Kalman";
}

/**
 * Setup function
 */
bool Kalman::setup(void)
{
    bool bSuccess = EstimationFilter::setup();
    if (bSuccess)
    {
        m_registry["errorCovariance"] = m_P;
        m_registry["measurementCovariance"] = m_R;
        m_registry["processCovariance"] = m_Q;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void Kalman::swap(Kalman &kalman)
{
    EstimationFilter::swap(kalman);

    m_P.swap(kalman.m_P);
    m_Q.swap(kalman.m_Q);
    m_R.swap(kalman.m_R);
}

}

}

}

}
