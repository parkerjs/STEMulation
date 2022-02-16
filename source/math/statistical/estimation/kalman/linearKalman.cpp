#include "appliedEstimationFilter.h"
#include "linearKalman.h"
#include "matrix2d.h"
#include "stateMap.h"
#include "stateVector.h"

// file-scoped variables
static constexpr char factoryName[] = "LinearKalman";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::statistical::estimation;
using namespace math::statistical::estimation::kalman;
using namespace utilities;

namespace math
{

namespace statistical
{

namespace estimation
{

namespace kalman
{

// register factories...
static FactoryRegistrar<EstimationFilter, Kalman>
factory(factoryName, &LinearKalman::create);

/**
 * Constructor
 */
LinearKalman::LinearKalman(void)
{

}

/**
 * Copy constructor
 */
LinearKalman::LinearKalman(const LinearKalman &kalman)
{
    operator = (kalman);
}

/**
 * Move constructor
 */
LinearKalman::LinearKalman(LinearKalman &&kalman)
{
    operator = (std::move(kalman));
}

/**
 * Destructor
 */
LinearKalman::~LinearKalman(void)
{

}

/**
 * Copy assignment operator
 */
LinearKalman &LinearKalman::operator = (const LinearKalman &kalman)
{
    if (&kalman != this)
    {
        ExtendedKalman::operator = (kalman);

        m_H = kalman.m_H;
    }

    return *this;
}

/**
 * Move assignment operator
 */
LinearKalman &LinearKalman::operator = (LinearKalman &&kalman)
{
    if (&kalman != this)
    {
        kalman.swap(*this);
    }

    return *this;
}

/**
 * clone() function
 */
LinearKalman *LinearKalman::clone(void) const
{
    auto *pKalman = new LinearKalman(*this);
    if (!pKalman->setup())
    {
        delete pKalman;
        pKalman = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Cloned an instance of \"" + pKalman->getClassName() + "\".\n",
               getQualifiedMethodName(__func__));
    }

    if (pKalman == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to clone/setup a new instance!\n",
               getQualifiedMethodName(__func__));
    }

    return pKalman;
}

/**
 * create() function
 */
LinearKalman *LinearKalman::create(void)
{
    auto *pKalman = new LinearKalman();
    if (!pKalman->setup())
    {
        delete pKalman;
        pKalman = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Created an instance of \"" + pKalman->getClassName() + "\".\n",
               getQualifiedStaticMethodName(__func__, LinearKalman));
    }

    if (pKalman == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to instantiate/setup a new instance!\n",
               getQualifiedStaticMethodName(__func__, LinearKalman));
    }

    return pKalman;
}

/**
 * Get the name of this class
 */
std::string LinearKalman::getClassName(void) const
{
    return "LinearKalman";
}

/**
 * Get the factory name of this constructible
 */
std::string LinearKalman::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Perform a measurement update
 * @param stateMeasurement a reference to a state vector object
 */
bool LinearKalman::measurementUpdate(StateVector &stateMeasurement)
{
    auto &&z = stateMeasurement;
    bool bSuccess = (m_bInitialized && m_pAppliedEstimationFilter != nullptr && !z.empty());
    if (bSuccess)
    {
        auto &&HT = m_H.transpose();
        auto &&I = Matrix2d::identity(m_xh.rows());

        Matrix2d P(m_P); // error covariance prior to measurement update
        Matrix2d x(m_xh); // state estimate prior to measurement update
        auto &&S = m_H * P * HT + m_R; // residual covariance
        auto &&K = P * HT * S.calcInverse(); // Kalman gain
        auto &&yh = m_H * x;
        auto &&v = m_pAppliedEstimationFilter->computeMeasurementResidual(yh, z); // measurement residual
        m_xh = x + K * v; // state estimate update

        if (m_lambda > 0.0)
        {
            computeReversePrediction(m_H,
                                     S,
                                     v,
                                     z);

            K = P * HT * S.calcInverse(); // Kalman gain
            m_xh = x + K * v; // state estimate update
        }

        m_P = (I - K * m_H) * P; // error covariance update

        // adaptive estimation of process noise
        adaptProcessCovariance(x, P);

    }

    if (m_pAppliedEstimationFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Applied estimation filter is null! A selection must "
               "be made before the object can be initialized.\n",
               getQualifiedMethodName(__func__));
    }

    if (!m_bInitialized)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Filter has not been initialized!\n",
               getQualifiedMethodName(__func__));
    }

    if (z.empty())
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Measurement vector is empty!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Setup function
 */
bool LinearKalman::setup(void)
{
    bool bSuccess = ExtendedKalman::setup();
    if (bSuccess)
    {
        m_registry["measurement"] = m_H;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void LinearKalman::swap(LinearKalman &kalman)
{
    ExtendedKalman::swap(kalman);

    m_H.swap(kalman.m_H);
}

}

}

}

}
