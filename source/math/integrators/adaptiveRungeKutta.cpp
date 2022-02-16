#include "adaptiveRungeKutta.h"
#include "butcherTableau.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateVector.h"
#include <cmath>

// using namespace declarations
using namespace math::control_systems;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace math
{

namespace integrators
{

/**
 * Constructor
 * @param butcherTableau  a reference to this Runge-Kutta method's Butcher Tableau
 * @param tolerance       the desired error tolerance (default = 1e-4)
 * @param minimumStepSize the minimum step size (default = 1.0e-10)
 * @param maximumStepSize the maximum step size (default = 1e-2)
 */
AdaptiveRungeKutta::AdaptiveRungeKutta(const ButcherTableau &butcherTableau,
                                       double tolerance,
                                       double minimumStepSize,
                                       double maximumStepSize)
: RungeKutta(butcherTableau),
  m_maximumStepSize(maximumStepSize),
  m_minimumStepSize(minimumStepSize),
  m_pState0(new StateVector()),
  m_tolerance(tolerance)
{

}

/**
 * Copy constructor
 */
AdaptiveRungeKutta::AdaptiveRungeKutta(const AdaptiveRungeKutta &rungeKutta)
: RungeKutta(rungeKutta),
  m_pState0(nullptr)
{
    operator = (rungeKutta);
}

/**
 * Move constructor
 */
AdaptiveRungeKutta::AdaptiveRungeKutta(AdaptiveRungeKutta &&rungeKutta)
: RungeKutta(std::move(rungeKutta)),
  m_pState0(nullptr)
{
    operator = (std::move(rungeKutta));
}

/**
 * Destructor
 */
AdaptiveRungeKutta::~AdaptiveRungeKutta(void)
{
    if (m_pState0 != nullptr)
        delete m_pState0;
}

/**
 * Copy assignment operator
 */
AdaptiveRungeKutta &AdaptiveRungeKutta::operator = (const AdaptiveRungeKutta &rungeKutta)
{
    if (&rungeKutta != this)
    {
        RungeKutta::operator = (rungeKutta);

        if (m_pState0 == nullptr)
            m_pState0 = new StateVector();

        if (rungeKutta.m_pState0 != nullptr)
            *m_pState0 = *rungeKutta.m_pState0;
    }

    return *this;
}

/**
 * Move assignment operator
 */
AdaptiveRungeKutta &AdaptiveRungeKutta::operator = (AdaptiveRungeKutta &&rungeKutta)
{
    if (&rungeKutta != this)
    {
        rungeKutta.swap(*this);
    }

    return *this;
}

/**
 * Get the name of this class
 */
std::string AdaptiveRungeKutta::getClassName(void) const
{
    return "AdaptiveRungeKutta";
}

/**
 * Get maximum step size
 */
double AdaptiveRungeKutta::getMaximumStepSize(void) const
{
    return m_maximumStepSize;
}

/**
 * Get minimum step size
 */
double AdaptiveRungeKutta::getMinimumStepSize(void) const
{
    return m_minimumStepSize;
}

/**
 * Get error tolerance
 */
double AdaptiveRungeKutta::getTolerance(void) const
{
    return m_tolerance;
}

/**
 * Initialization function
 */
bool AdaptiveRungeKutta::initialize(void)
{
    bool bSuccess = RungeKutta::initialize();
    if (bSuccess)
    {
        if (m_pState0 == nullptr)
            m_pState0 = new StateVector();

        m_tolerance = 1.0e-4;
        m_minimumStepSize = 1.0e-10;
        m_maximumStepSize = 1.0e-2;
    }

    return bSuccess;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool AdaptiveRungeKutta::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pMaximumStepSizeNode = pNode->first_node("maximumStepSize");
        if (pMaximumStepSizeNode != nullptr)
            m_maximumStepSize = std::stod(pMaximumStepSizeNode->value());

        auto *pMinimumStepSizeNode = pNode->first_node("minimumtepSize");
        if (pMinimumStepSizeNode != nullptr)
            m_minimumStepSize = std::stod(pMinimumStepSizeNode->value());

        auto *pToleranceNode = pNode->first_node("tolerance");
        if (pToleranceNode != nullptr)
            m_tolerance = std::stod(pToleranceNode->value());
    }

    return bSuccess;
}
#endif
/**
 * Set maximum step size
 */
void AdaptiveRungeKutta::setMaximumStepSize(double maximumStepSize)
{
    m_maximumStepSize = maximumStepSize;
}

/**
 * Set minimum step size
 */
void AdaptiveRungeKutta::setMinimumStepSize(double minimumStepSize)
{
    m_minimumStepSize = minimumStepSize;
}

/**
 * Set error tolerance
 */
void AdaptiveRungeKutta::setTolerance(double tolerance)
{
    m_tolerance = tolerance;
}

/**
 * Setup function
 */
bool AdaptiveRungeKutta::setup(void)
{
    bool bSuccess = RungeKutta::setup();
    if (bSuccess)
    {
        m_registry["maximumStepSize"] = m_maximumStepSize;
        m_registry["minimumtepSize"] = m_minimumStepSize;
        m_registry["tolerance"] = m_tolerance;
    }

    return bSuccess;
}

/**
 * Function to numerically solve an ordinary differential equation
 * @param x        the state vector
 * @param dynamics a function object which formulates the dynamics as a series of first-order differential
 *                 equations; the function object's function call operator should accept three arguments, the
 *                 first two of which are the input time and state vector; the last argument contains the
 *                 computed state derivative on output.
 * @param t0       the start of the interval
 * @param t1       the end of the interval
 */
bool AdaptiveRungeKutta::solve(StateVector &x,
                               tStateDynamicsFunction &dynamics,
                               double t0,
                               double t1)
{
    bool bSuccess = (m_pState0 != nullptr);
    if (bSuccess)
    {
        auto n = x.size();
        if (m_pState0->size() != n)
            m_pState->resize(n);

        auto &&butcherTableau = *m_pButcherTableau;
        auto &&derivativeTable = *m_pDerivativeTable;
        auto &&state = *m_pState;
        auto &&state0 = *m_pState0;
        auto s = butcherTableau.stages();
        auto h = t1 > t0 ? t1 - t0 : t0 - t1;
        if (h > m_maximumStepSize)
            h = m_maximumStepSize;

        auto t = t0;
        while (bSuccess && t < t1)
        {
            std::copy(x.cbegin(), x.cend(), state0.begin());
            bSuccess = RungeKutta::solve(x, dynamics, t, t + h);
            if (bSuccess)
            {
                // compute the local truncation error from the extended Tableau table
                auto &&w = *m_pStateDerivative; // reuse the state derivate vector, alias it for clarity
                std::fill(state.begin(), state.end(), 0.0);
                for (std::size_t j = 0; j < s; ++j)
                {
                    auto b_j = butcherTableau.coefficient(s + 1, j + 1);
                    auto m = j * n;
                    for (std::size_t k = 0; k < n; ++k)
                        w[k] += h * b_j * derivativeTable[m + k];
                }

                // determine the maximum state error
                double error = 0.0;
                for (std::size_t i = 0; i < n; i++)
                {
                    auto dw = std::fabs(w[i] - x[i]);
                    if (dw > error)
                        error = dw;
                }

                if (error < m_tolerance)
                    t += h;
                else
                    std::copy(state0.cbegin(), state0.cend(), x.begin()); // restore the state

                // calculate new h
                h = calcAdaptiveStepSize(h, error);
                if (h > m_maximumStepSize)
                    h = m_maximumStepSize;

                if (t + h > t1)
                    h = t1 - t;
                else if (h < m_minimumStepSize)
                {
                    logMsg(std::cout, utilities::LoggingLevel::Enum::Warning,
                           "Minimum step size exceeded, integration failed.\n",
                           getQualifiedMethodName(__func__));

                    bSuccess = false;
                }
            }
        }
    }

    return bSuccess;
}

/**
 * Swap function
 */
void AdaptiveRungeKutta::swap(AdaptiveRungeKutta &rungeKutta)
{
    RungeKutta::swap(rungeKutta);

    std::swap(m_maximumStepSize, rungeKutta.m_maximumStepSize);
    std::swap(m_minimumStepSize, rungeKutta.m_minimumStepSize);
    std::swap(m_pState0, rungeKutta.m_pState0);
    std::swap(m_tolerance, rungeKutta.m_tolerance);
}

}

}
