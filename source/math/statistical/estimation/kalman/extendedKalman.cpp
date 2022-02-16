#include "appliedEstimationFilter.h"
#include "extendedKalman.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateVector.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "ExtendedKalman";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::statistical::estimation;
using namespace math::statistical::estimation::kalman;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
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
factory(factoryName, &ExtendedKalman::create);

/**
 * Constructor
 */
ExtendedKalman::ExtendedKalman(void)
: m_gamma(0.0),
  m_lambda(0.0)
{

}

/**
 * Copy constructor
 */
ExtendedKalman::ExtendedKalman(const ExtendedKalman &kalman)
{
    operator = (kalman);
}

/**
 * Move constructor
 */
ExtendedKalman::ExtendedKalman(ExtendedKalman &&kalman)
{
    operator = (std::move(kalman));
}

/**
 * Destructor
 */
ExtendedKalman::~ExtendedKalman(void)
{

}

/**
 * Copy assignment operator
 */
ExtendedKalman &ExtendedKalman::operator = (const ExtendedKalman &kalman)
{
    if (&kalman != this)
    {
        Kalman::operator = (kalman);

        m_gamma = kalman.m_gamma;
        m_lambda = kalman.m_lambda;
    }

    return *this;
}

/**
 * Move assignment operator
 */
ExtendedKalman &ExtendedKalman::operator = (ExtendedKalman &&kalman)
{
    if (&kalman != this)
    {
        kalman.swap(*this);
    }

    return *this;
}

/**
 * Adaptively estimate the process covariance
 * @param x the state estimate prior to measurement update
 * @param P the error covariance prior to measurement update
 */
void ExtendedKalman::adaptProcessCovariance(const Matrix2d &x,
                                            const Matrix2d &P)
{
    if (m_gamma > 0.0)
    {
        // compute state estimate residual - at this
        // point, m_xh has been updated by the measurement
        m_temp_q = m_xh;
        m_temp_q -= x;

        // compute estimated process noise
        Matrix2d::outerProduct(m_temp_q, m_temp_q, m_tempQ);
        m_tempQ += P;
        m_tempQ -= m_P;
        m_tempQ -= m_Q;

        // combine new estimate of process noise in a moving average
        m_Q += (m_tempQ - m_Q) / m_gamma;
    }
}

/**
 * Adjust the residual covariance
 * @param A      the dynamics Jacobian
 * @param H      the measurement Jacobian
 * @param lambda the norm residual ratio
 * @param S      the residual covariance
 */
void ExtendedKalman::adjustResidualCovariance(const Matrix2d &A,
                                              const Matrix2d &H,
                                              double lambda,
                                              Matrix2d &S)
{
//  Matrix2d P(A * m_P * A.calcTranspose() + lambda * m_Q);
    S = A * m_P;
    S.postMultiplyTranspose(A, S);
    S += lambda * m_Q;

//  S = H * P * H.calcTranspose() + m_R;
    S = H * S;
    S.postMultiplyTranspose(H, S);
    S += m_R;
}

/**
 * clone() function
 */
ExtendedKalman *ExtendedKalman::clone(void) const
{
    auto *pKalman = new ExtendedKalman(*this);
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
 * Adaptive reverse prediction
 * @param H the measurement Jacobian
 * @param S the residual covariance
 * @param v the measurement residual at time k + 1
 * @param z the state measurement vector
 */
void ExtendedKalman::computeReversePrediction(const Matrix2d &H,
                                              Matrix2d &S,
                                              const Matrix2d &v,
                                              const Matrix2d &z)
{
    if (m_pAppliedEstimationFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Applied estimation filter is null! A selection must "
               "be made before the object can be initialized.\n",
               getQualifiedMethodName(__func__));
    }
    else
    {
        auto &&A = m_pAppliedEstimationFilter->dynamicsJacobian(m_dt, m_xh);
        auto &&xh = A.calcInverse() * m_xh; // revere state prediction at time k
        auto &&yh = m_pAppliedEstimationFilter->measurementModel(xh);

        // reverse prediction residual at time k
        auto &&w = m_pAppliedEstimationFilter->computeMeasurementResidual(yh, z);

        double eps[2]; // norm residual squares at time k and k + 1
        auto &&SI = S.calcInverse(); // inverse covariance

        // norm residual square at time k
//      eps[0] = (w.calcTranspose() * SI * w)[0]; // norm residual square at time k
        m_temp_wSIw = SI;
        m_temp_wSIw.preMultiplyTranspose(w, m_temp_wSIw);
        m_temp_wSIw *= w;
        eps[0] = m_temp_wSIw[0];

        // norm residual square at time k + 1
//      eps[1] = (v.calcTranspose() * SI * v)[0]; // norm residual square at time k + 1
        m_temp_vSIv = SI;
        m_temp_vSIv.preMultiplyTranspose(v, m_temp_vSIv);
        m_temp_vSIv *= v;
        eps[1] = m_temp_vSIv[0];

        // norm residual ratio
        auto lambda = fabs(eps[0] - eps[1]) / eps[1];
        if (lambda > m_lambda)
            adjustResidualCovariance(A, H, lambda, S);
    }
}

/**
 * Configure function (using input file or input string)
 */
bool ExtendedKalman::configure(tTokenMap &tokenMap)
{
    bool bSuccess = Kalman::configure(tokenMap);
    if (bSuccess)
    {
        setAdaptiveWindow(m_gamma);
    }

    return bSuccess;
}

/**
 * create() function
 */
ExtendedKalman *ExtendedKalman::create(void)
{
    auto *pKalman = new ExtendedKalman();
    if (!pKalman->setup())
    {
        delete pKalman;
        pKalman = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Created an instance of \"" + pKalman->getClassName() + "\".\n",
               getQualifiedStaticMethodName(__func__, ExtendedKalman));
    }

    if (pKalman == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to instantiate/setup a new instance!\n",
               getQualifiedStaticMethodName(__func__, ExtendedKalman));
    }

    return pKalman;
}

/**
 * Get adaptive reverse prediction norm residual threshold
 */
double ExtendedKalman::getAdaptiveNormResidualThreshold(void) const
{
    return m_lambda;
}

/**
 * Get adaptive process window size
 */
double ExtendedKalman::getAdaptiveWindow(void) const
{
    return m_gamma;
}

/**
 * Get the name of this class
 */
std::string ExtendedKalman::getClassName(void) const
{
    return "ExtendedKalman";
}

/**
 * Get the factory name of this constructible
 */
std::string ExtendedKalman::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Initialization function
 * @param dt the sampling interval (s)
 */
bool ExtendedKalman::initialize(double dt)
{
    m_dt = dt;
    m_bInitialized = (m_pAppliedEstimationFilter != nullptr);
    if (m_bInitialized)
    {
        m_bInitialized = m_pAppliedEstimationFilter->initialize();
        if (!m_bInitialized)
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Applied estimation filter failed to initialize!\n",
                   getQualifiedMethodName(__func__));
        }
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Applied estimation filter is null! A selection must "
               "be made before the object can be initialized.\n",
               getQualifiedMethodName(__func__));
    }

    return m_bInitialized;
}

/**
 * Perform a measurement update
 * @param stateMeasurement a reference to a state vector object
 */
bool ExtendedKalman::measurementUpdate(StateVector &stateMeasurement)
{
    auto &&z = stateMeasurement;
    bool bSuccess = (m_bInitialized && m_pAppliedEstimationFilter != nullptr && !z.empty());
    if (bSuccess)
    {
        auto &&I = Matrix2d::identity(m_xh.rows());
//      Matrix2d P(m_P); // error covariance prior to measurement update
        m_tempP = m_P; // error covariance prior to measurement update
//      Matrix2d x(m_xh); // state estimate prior to measurement update
        m_temp_x = m_xh; // state estimate prior to measurement update

        // Compute the measurement Jacobian
//      Matrix2d H(pAppliedEstimationFilter->measurementJacobian(x))
        auto &&H = m_pAppliedEstimationFilter->measurementJacobian(m_temp_x);
//      const Matrix2d &HT = H.calcTranspose();
//      Matrix2d S(H * P * HT + m_R); // residual covariance
        m_tempS = H * m_tempP;
        m_tempS.postMultiplyTranspose(H, m_tempS);
        m_tempS += m_R;

//      Matrix2d K(P * HT * S.calcInverse()); // Kalman gain
        Matrix2d K(m_tempP);
        K.postMultiplyTranspose(H, K);
        K *= m_tempS.calcInverse();

//      const Matrix2d &yh = pAppliedEstimationFilter->measurementModel(x);
        auto &&yh = m_pAppliedEstimationFilter->measurementModel(m_temp_x);

        // measurement residual
        auto &&v = m_pAppliedEstimationFilter->computeMeasurementResidual(yh, z);

//      m_xh = x + K * v; // state estimate update
        m_xh = K * v;
        m_xh += m_temp_x;

        if (m_lambda > 0.0)
        {
            K = m_tempP;
//          computeReversePrediction(H, S, v, z);
            computeReversePrediction(H,
                                     m_tempS,
                                     v,
                                     z);

//          K = P * HT * S.calcInverse(); // Kalman gain
            K.postMultiplyTranspose(H, K);
            K *= m_tempS.calcInverse();

//          m_xh = x + K * v; // state estimate update
            m_xh = K * v;
            m_xh += m_temp_x;
        }

//      m_P = (I - K * H) * P; // error covariance update
        m_P = (I - K * H) * m_tempP; // error covariance update

        // adaptive estimation of process noise
//      adaptProcessCovariance(x, P);
        adaptProcessCovariance(m_temp_x,
                               m_tempP);
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
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool ExtendedKalman::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = Kalman::readFromXML(pNode);
    if (bSuccess)
    {
        // read the adaptive norm residual ratio threshold
        auto *pAdaptiveNormResidualRatioThresholdNode = pNode->first_node("adaptiveNormResidualRatioThreshold");
        if (pAdaptiveNormResidualRatioThresholdNode != nullptr)
            setAdaptiveNormResidualThreshold(std::stod(pAdaptiveNormResidualRatioThresholdNode->value()));

        // read the adaptive process noise window size
        auto *pAdaptiveProcessNoiseWindowSizeNode = pNode->first_node("adaptiveProcessNoiseWindowSize");
        if (pAdaptiveProcessNoiseWindowSizeNode != nullptr)
            bSuccess = setAdaptiveWindow(std::stod(pAdaptiveProcessNoiseWindowSizeNode->value()));
    }

    return bSuccess;
}
#endif
/**
 * Set adaptive reverse prediction norm residual threshold
 * @param lambda the norm residual ratio threshold (a value > 0 enables adaptive reverse prediction)
 */
void ExtendedKalman::setAdaptiveNormResidualThreshold(double lambda)
{
    m_lambda = lambda > 0.0 ? lambda : 0.0;
}

/**
 * Set adaptive process window size
 */
bool ExtendedKalman::setAdaptiveWindow(double window)
{
    bool bSuccess = (window >= 0.0);
    if (bSuccess)
        m_gamma = window;
    else
    {
        m_gamma = 0.0;

        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Invalid adaptive window size specified in configuration, resetting to " +
               std::to_string(m_gamma) + "!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Setup function
 */
bool ExtendedKalman::setup(void)
{
    bool bSuccess = Kalman::setup();
    if (bSuccess)
    {
        m_registry["adaptiveNormResidualRatioThreshold"] = m_lambda;
        m_registry["adaptiveProcessNoiseWindowSize"] = m_gamma;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void ExtendedKalman::swap(ExtendedKalman &kalman)
{
    Kalman::swap(kalman);

    std::swap(m_gamma, kalman.m_gamma);
    std::swap(m_lambda, kalman.m_lambda);

    m_temp_q.swap(kalman.m_temp_q);
    m_temp_vSIv.swap(kalman.m_temp_vSIv);
    m_temp_wSIw.swap(kalman.m_temp_wSIw);
    m_temp_x.swap(kalman.m_temp_x);
    m_tempP.swap(kalman.m_tempP);
    m_tempQ.swap(kalman.m_tempQ);
    m_tempS.swap(kalman.m_tempS);
}

/**
 * Perform a time update
 * @param u the control input
 */
void ExtendedKalman::timeUpdate(const Matrix2d &u)
{
    if (m_bInitialized && m_pAppliedEstimationFilter != nullptr)
    {
        auto &&A = m_pAppliedEstimationFilter->dynamicsJacobian(m_dt, m_xh);
        m_xh = m_pAppliedEstimationFilter->dynamicsModel(m_dt, m_xh, u); // project state ahead
//      m_P = A * m_P * A.calcTranspose() + m_Q; // project the error covariance ahead
        m_P = A * m_P;
        m_P.postMultiplyTranspose(A, m_P);
        m_P += m_Q;
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
}

}

}

}

}
