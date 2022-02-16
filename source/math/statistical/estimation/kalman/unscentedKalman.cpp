#include "appliedEstimationFilter.h"
#include "cholesky.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateMap.h"
#include "stateVector.h"
#include "unscentedKalman.h"
#include <iomanip>

// file-scoped variables
static constexpr char factoryName[] = "UnscentedKalman";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix::decomposition;
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
factory(factoryName, &UnscentedKalman::create);

/**
 * Constructor
 */
UnscentedKalman::UnscentedKalman(void)
: m_alpha(1.e-1),
  m_beta(2.0),
  m_kappa(0.0)
{

}

/**
 * Copy constructor
 */
UnscentedKalman::UnscentedKalman(const UnscentedKalman &kalman)
{
    operator = (kalman);
}

/**
 * Move constructor
 */
UnscentedKalman::UnscentedKalman(UnscentedKalman &&kalman)
{
    operator = (std::move(kalman));
}

/**
 * Destructor
 */
UnscentedKalman::~UnscentedKalman(void)
{

}

/**
 * Copy assignment operator
 */
UnscentedKalman &UnscentedKalman::operator = (const UnscentedKalman &kalman)
{
    if (&kalman != this)
    {
        Kalman::operator = (kalman);

        m_alpha = kalman.m_alpha;
        m_beta = kalman.m_beta;
        m_chiX = kalman.m_chiX;
        m_kappa = kalman.m_kappa;
        m_K = kalman.m_K;
        m_Pxy = kalman.m_Pxy;
        m_Pyy = kalman.m_Pyy;
        m_S = kalman.m_S;
        m_Wm = kalman.m_Wm;
        m_Wc = kalman.m_Wc;
        m_yh = kalman.m_yh;
    }

    return *this;
}

/**
 * Move assignment operator
 */
UnscentedKalman &UnscentedKalman::operator = (UnscentedKalman &&kalman)
{
    if (&kalman != this)
    {
        kalman.swap(*this);
    }

    return *this;
}

/**
 * Compute the sigma points
 */
void UnscentedKalman::calcSigmaPoints(void)
{
    auto &&L = m_xh.size();
    auto &&lambda = m_alpha * m_alpha * (L + m_kappa) - L;

    // compute the matrix square root of (L + lambda) * m_P
    m_S = m_P;
    m_S.scale(L + lambda);
    Cholesky_Factor<Matrix2d> cholesky;
    cholesky.factor(m_S);

    // compute the 2L + 1 sigma point column vectors
    for (std::size_t i = 0; i < L; ++i)
    {
        m_chiX(i, 0) = m_xh[i];
        for (std::size_t j = 0; j < L; ++j)
        {
            auto &&S_ij = m_S(i, j);
            m_chiX(i, j + 1) = m_chiX(i, 0) + S_ij;
            m_chiX(i, j + L + 1) = m_chiX(i, 0) - S_ij;
        }
    }
}

/**
 * Compute the sigma points
 */
void UnscentedKalman::calcWeights(void)
{
    auto &&L = m_xh.size();
    auto &&n = 1 + (L << 1);
    auto &&lambda = m_alpha * m_alpha * (L + m_kappa) - L;

    m_Wc.resize(n, 1);
    m_Wm.resize(n, 1);

    m_Wm[0] = lambda / (L + lambda);
    m_Wc[0] = m_Wm[0] + 1 - m_alpha * m_alpha + m_beta;

    for (std::size_t i = 1; i < n; ++i)
    {
        m_Wc[i] = 0.5 / (L + lambda);
        m_Wm[i] = m_Wc[i];
    }
}

/**
 * clone() function
 */
UnscentedKalman *UnscentedKalman::clone(void) const
{
    auto *pKalman = new UnscentedKalman(*this);
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
 * Configure function (using input file or input string)
 */
bool UnscentedKalman::configure(tTokenMap &tokenMap)
{
    bool bSuccess = Kalman::configure(tokenMap);
    if (bSuccess)
    {
        setAlpha(m_alpha);
    }

    return bSuccess;
}

/**
 * create() function
 */
UnscentedKalman *UnscentedKalman::create(void)
{
    auto *pKalman = new UnscentedKalman();
    if (!pKalman->setup())
    {
        delete pKalman;
        pKalman = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Created an instance of \"" + pKalman->getClassName() + "\".\n",
               getQualifiedStaticMethodName(__func__, UnscentedKalman));
    }

    if (pKalman == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to instantiate/setup a new instance!\n",
               getQualifiedStaticMethodName(__func__, UnscentedKalman));
    }

    return pKalman;
}

/**
 * Get alpha parameter
 */
double UnscentedKalman::getAlpha(void) const
{
    return m_alpha;
}

/**
 * Get beta parameter
 */
double UnscentedKalman::getBeta(void) const
{
    return m_beta;
}

/**
 * Get the name of this class
 */
std::string UnscentedKalman::getClassName(void) const
{
    return "UnscentedKalman";
}

/**
 * Get the factory name of this constructible
 */
std::string UnscentedKalman::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get kappa parameter
 */
double UnscentedKalman::getKappa(void) const
{
    return m_kappa;
}

/**
 * Initialization function
 * @param dt the sampling interval (s)
 */
bool UnscentedKalman::initialize(double dt)
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
        else if (!m_xh.empty())
        {
            auto &&L = m_xh.size();
            auto &&n = m_R.rows();

            // resize observation estimate vector
            m_yh.resize(n, 1);

            // resize sigma vector rows associated with state variables
            m_chiX.resize(L, 1 + (L << 1));

            // resize sigma vectors associated with observation estimate
            m_chiY.resize(n, 1 + (L << 1));

            // resize covariance matrices
            m_P.resize(L, L);
            m_Pxy.resize(L, n);
            m_Pyy.resize(n, n);

            calcWeights();
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "A non-empty initial state estimate vector must be supplied for initialization.\n",
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
bool UnscentedKalman::measurementUpdate(StateVector &stateMeasurement)
{
    auto &&z = stateMeasurement;
    bool bSuccess = (m_bInitialized && m_pAppliedEstimationFilter != nullptr && !z.empty());
    if (bSuccess)
    {
        // initialize Pyy covariance matrix
        for (std::size_t i = 0; i < m_Pyy.size(); ++i)
            m_Pyy[i] = 0.0;

        // compute Pyy covariance
        auto &&L = m_xh.size();
        auto &&n = m_yh.size();
        for (std::size_t i = 0; i < n; ++i)
        {
            for (std::size_t j = 0; j < n; ++j)
            {
                for (std::size_t k = 0; k <= (L << 1); ++k)
                {
                    auto &&dYi = m_chiY(i, k) - m_yh[i];
                    auto &&dYj = m_chiY(j, k) - m_yh[j];
                    m_Pyy(i, j) += m_Wc[k] * dYi * dYj;
                }

                m_Pyy(i, j) += m_R(i, j);
            }
        }

        // initialize Pxy covariance matrix
        for (std::size_t i = 0; i < m_Pxy.size(); ++i)
            m_Pxy[i] = 0.0;

        // compute Pxy covariance
        for (std::size_t i = 0; i < L; ++i)
        {
            for (std::size_t j = 0; j < n; ++j)
            {
                for (std::size_t k = 0; k <= (L << 1); ++k)
                {
                    auto &&dXi = m_chiX(i, k) - m_xh[i];
                    auto &&dYj = m_chiY(j, k) - m_yh[j];
                    m_Pxy(i, j) += m_Wc[k] * dXi * dYj;
                }
            }
        }

        // compute Kalman gain
        m_K = m_Pxy * m_Pyy.calcInverse();

        // update state estimate
        auto &&v = m_pAppliedEstimationFilter->computeMeasurementResidual(m_yh, z); // measurement residual
        m_xh += m_K * v;

        // update error covariance
        m_P -= m_K * m_Pyy.postMultiplyTranspose(m_K);
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
bool UnscentedKalman::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = Kalman::readFromXML(pNode);
    if (bSuccess)
    {
        // read the sigma point spreading factor
        auto *pAlphaNode = pNode->first_node("alpha");
        if (pAlphaNode != nullptr)
        {
            setAlpha(std::stod(pAlphaNode->value()));
        }

        // read the pdf distribution parameter
        auto *pBetaNode = pNode->first_node("beta");
        if (pBetaNode != nullptr)
        {
            setBeta(std::stod(pBetaNode->value()));
        }

        // read the secondary sigma point spreading factor
        auto *pKappaNode = pNode->first_node("kappa");
        if (pKappaNode != nullptr)
        {
            setKappa(std::stod(pKappaNode->value()));
        }
    }

    return bSuccess;
}
#endif
/**
 * Set alpha parameter
 */
void UnscentedKalman::setAlpha(double alpha)
{
    if (alpha < 1.E-4)
    {
        alpha = 1.e-4;
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Unscented filter sigma point spreading factor too low, resetting to " +
               std::to_string(m_alpha) + ".\n",
               getQualifiedMethodName(__func__));
    }

    if (alpha > 1.0)
    {
        alpha = 1.0;
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Unscented filter sigma point spreading factor too high, resetting to " +
               std::to_string(m_alpha) + ".\n",
               getQualifiedMethodName(__func__));
    }

    m_alpha = alpha;
}

/**
 * Set beta parameter
 */
void UnscentedKalman::setBeta(double beta)
{
    m_beta = beta;
}

/**
 * Set kappa parameter
 */
void UnscentedKalman::setKappa(double kappa)
{
    m_kappa = kappa;
}

/**
 * Setup function
 */
bool UnscentedKalman::setup(void)
{
    bool bSuccess = Kalman::setup();
    if (bSuccess)
    {
        m_registry["alpha"] = m_alpha;
        m_registry["beta"] = m_beta;
        m_registry["kappa"] = m_kappa;
        m_registry["observationEstimate"] = m_yh;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void UnscentedKalman::swap(UnscentedKalman &kalman)
{
    Kalman::swap(kalman);

    std::swap(m_alpha, kalman.m_alpha);
    std::swap(m_beta, kalman.m_beta);
    m_chiX.swap(kalman.m_chiX);
    m_chiY.swap(kalman.m_chiY);
    std::swap(m_kappa, kalman.m_kappa);
    m_K.swap(kalman.m_K);
    m_Pxy.swap(kalman.m_Pxy);
    m_Pyy.swap(kalman.m_Pyy);
    m_S.swap(kalman.m_S);
    m_Wc.swap(kalman.m_Wc);
    m_Wm.swap(kalman.m_Wm);
    m_yh.swap(kalman.m_yh);
}

/**
 * Perform a time update
 * @param u the control input
 */
void UnscentedKalman::timeUpdate(const Matrix2d &u)
{
    if (m_bInitialized && m_pAppliedEstimationFilter != nullptr)
    {
        // compute the sigma vectors
        calcSigmaPoints();

        // initialize state estimate to zero before finding mean
        auto &&L = m_xh.size();
        for (std::size_t i = 0; i < L; ++i)
            m_xh[i] = 0.0;

        // propagate the sigma vectors through the dynamics model
        std::vector<double> chiX(L, 0.0);
        for (std::size_t i = 0; i <= (L << 1); ++i)
        {
            for (std::size_t j = 0; j < L; ++j)
                chiX[j] = m_chiX(j, i);

            chiX = m_pAppliedEstimationFilter->dynamicsModel(m_dt, chiX, u);

            // compute state estimate
            for (std::size_t j = 0; j < L; ++j)
            {
                m_chiX(j, i) = chiX[j];
                m_xh[j] += m_Wm[i] * m_chiX(j, i);
            }
        }

        // initialize error covariance matrix
        for (std::size_t i = 0; i < m_P.size(); ++i)
            m_P[i] = 0.0;

        // compute error covariance
        for (std::size_t i = 0; i < L; ++i)
        {
            for (std::size_t j = 0; j < L; ++j)
            {
                for (std::size_t k = 0; k <= (L << 1); ++k)
                {
                    auto &&dXi = m_chiX(i, k) - m_xh[i];
                    auto &&dXj = m_chiX(j, k) - m_xh[j];
                    m_P(i, j) += m_Wc[k] * dXi * dXj;
                }

                m_P(i, j) += m_Q(i, j);
            }
        }

        // initialize observation estimate to zero before finding mean
        auto &&n = m_yh.size();
        for (std::size_t j = 0; j < n; ++j)
            m_yh[j] = 0.0;

        // propagate the sigma vectors through the measurement model
        for (std::size_t i = 0; i <= (L << 1); ++i)
        {
            for (std::size_t j = 0; j < L; ++j)
                chiX[j] = m_chiX(j, i);

            auto &&chiY = m_pAppliedEstimationFilter->measurementModel(chiX);

            // compute state estimate
            for (std::size_t j = 0; j < n; ++j)
            {
                m_chiY(j, i) = chiY[j];
                m_yh[j] += m_Wm[i] * m_chiY(j, i);
            }
        }
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
