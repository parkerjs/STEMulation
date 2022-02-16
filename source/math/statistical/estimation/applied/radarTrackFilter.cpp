#include "estimationFilter.h"
#include "matrix2d.h"
#include "radar_measurement_type.h"
#include "radarTrackEstimationFilterUser.h"
#include "radarTrackFilter.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateMap.h"
#include "stateVector.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "RadarTrackFilter";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::control_systems;
using namespace math::linear_algebra::matrix;
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

namespace applied
{

// register factories...
static FactoryRegistrar<AppliedEstimationFilter>
factory(factoryName, &RadarTrackFilter::create);

/**
 * Constructor
 */
RadarTrackFilter::RadarTrackFilter(void)
{

}

/**
 * Constructor
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
RadarTrackFilter::RadarTrackFilter(EstimationFilter *pEstimationFilter)
: AppliedEstimationFilter(pEstimationFilter),
  m_rho(0.0)
{
    m_maneuverVariance[0] = 0.0;
    m_maneuverVariance[1] = 0.0;
    m_maneuverVariance[2] = 0.0;
}

/**
 * Copy constructor
 */
RadarTrackFilter::RadarTrackFilter(RadarTrackFilter &trackFilter)
{
    operator = (trackFilter);
}

/**
 * Move constructor
 */
RadarTrackFilter::RadarTrackFilter(RadarTrackFilter &&trackFilter)
{
    operator = (std::move(trackFilter));
}

/**
 * Destructor
 */
RadarTrackFilter::~RadarTrackFilter(void)
{

}

/**
 * Copy assignment operator
 */
RadarTrackFilter &RadarTrackFilter::operator = (RadarTrackFilter &trackFilter)
{
    if (&trackFilter != this)
    {
        AppliedEstimationFilter::operator = (trackFilter);

        m_maneuverVariance[0] = trackFilter.m_maneuverVariance[0];
        m_maneuverVariance[1] = trackFilter.m_maneuverVariance[1];
        m_maneuverVariance[2] = trackFilter.m_maneuverVariance[2];
        m_rho = trackFilter.m_rho;
    }

    return *this;
}

/**
 * Move assignment operator
 */
RadarTrackFilter &RadarTrackFilter::operator = (RadarTrackFilter &&trackFilter)
{
    if (&trackFilter != this)
    {
        trackFilter.swap(*this);
    }

    return *this;
}

/**
 * clone() function
 */
RadarTrackFilter *RadarTrackFilter::clone(void)
{
    auto *pTrackFilter = new RadarTrackFilter(*this);
    if (!pTrackFilter->setup())
    {
        delete pTrackFilter;
        pTrackFilter = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Cloned an instance of \"" + pTrackFilter->getClassName() + "\".\n",
               getQualifiedMethodName(__func__));
    }

    if (pTrackFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to clone/setup a new instance!\n",
               getQualifiedMethodName(__func__));
    }

    return pTrackFilter;
}

/**
 * Compute error covariance
 * @param xh                             the state estimate vector
 * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
 * @param dt                             the sampling interval (s)
 */
Matrix2d RadarTrackFilter::computeErrorCovariance(const Matrix2d &xh,
                                                  const StateMap *pMeasurementStandardDeviations,
                                                  double dt)
{
    double sigAz = 0.0, sigR = 0.0, sigRd = 0.0, sigZe = 0.0;
    if (pMeasurementStandardDeviations != nullptr)
    {
        pMeasurementStandardDeviations->get(RadarMeasurementType::azimuth(), sigAz);
        pMeasurementStandardDeviations->get(RadarMeasurementType::range(), sigR);
        pMeasurementStandardDeviations->get(RadarMeasurementType::rangeRate(), sigRd);

        if (pMeasurementStandardDeviations->get(RadarMeasurementType::zenith(), sigZe))
            pMeasurementStandardDeviations->get(RadarMeasurementType::zenith(), sigZe);
        else // zenith not available, perhaps elevation is available
        {
            // NOTE: no conversion to zenith is necessary if the standard error is given in elevation
            pMeasurementStandardDeviations->get(RadarMeasurementType::elevation(), sigZe);
        }
    }

    auto sigAzd  = sigAz / dt;
    auto sigAzdd = sigAzd / dt;
    auto sigZed  = sigZe / dt;
    auto sigZedd = sigZed / dt;
    auto sigRdd  = sigRd / dt;

    auto cosSigAz = std::cos(sigAz);
    auto cosSigZe = std::cos(sigZe);
    auto sinSigAz = std::sin(sigAz);
    auto sinSigZe = std::sin(sigZe);

    auto rSinZe = sigR * sinSigZe;
    auto x = rSinZe * cosSigAz;
    auto y = rSinZe * sinSigAz;
    auto z = sigR * cosSigZe;

    auto rdSinZe = sigRd * sinSigZe;
    auto rZedCosZe = z * sigZed;
    auto rAzdSinZe = rSinZe * sigAzd;
    auto xd = (rdSinZe + rZedCosZe) * cosSigAz - rAzdSinZe * sinSigAz;
    auto yd = (rdSinZe + rZedCosZe) * sinSigAz + rAzdSinZe * cosSigAz;
    auto zd = sigRd * cosSigZe - sigR * sigZed * sinSigZe;

    auto rAzd = sigR * sigAzd;
    auto rAzdSq = rAzd * sigAzd;
    auto rZedSq = sigR * sigZed * sigZed;
    auto rdd_rZedSq_rAzdSqSinZe = (sigRdd - rAzdSq - rZedSq) * sinSigZe;
    auto twoRdZed_rZedd = 2 * sigRd * sigZed + sigR * sigZedd;
    auto twoRdZed_rZeddCosZe = twoRdZed_rZedd * cosSigZe;
    auto twoRdAzd_rAzddSinZe = (2 * sigRd * sigAzd + sigR * sigAzdd) * sinSigZe;
    auto twoR_ZedAzdCosZe = 2 * rAzd * sigZed * cosSigZe;
    auto xdd = (rdd_rZedSq_rAzdSqSinZe + twoRdZed_rZeddCosZe) * cosSigAz
             - (twoRdAzd_rAzddSinZe + twoR_ZedAzdCosZe) * sinSigAz;
    auto ydd = (rdd_rZedSq_rAzdSqSinZe + twoRdZed_rZeddCosZe) * sinSigAz
             + (twoRdAzd_rAzddSinZe + twoR_ZedAzdCosZe) * cosSigAz;
    auto zdd = (sigRdd - rZedSq) * cosSigZe - twoRdZed_rZedd * sinSigZe;

    Matrix2d P(9, 9);
    P(0, 0) = x * x;
    P(1, 1) = y * y;
    P(2, 2) = z * z;
    P(3, 3) = xd * xd;
    P(4, 4) = yd * yd;
    P(5, 5) = zd * zd;
    P(6, 6) = xdd * xdd;
    P(7, 7) = ydd * ydd;
    P(8, 8) = zdd * zdd;

    return P;
}

/**
 * Compute measurement covariance
 * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
 * @param dt                             the sampling interval (s)
 */
Matrix2d RadarTrackFilter::computeMeasurementCovariance(const StateMap *pMeasurementStandardDeviations,
                                                        double dt)
{
    double sigAz = 0.0, sigR = 0.0, sigRd = 0.0, sigZe = 0.0;
    if (pMeasurementStandardDeviations != nullptr)
    {
        pMeasurementStandardDeviations->get(RadarMeasurementType::azimuth(), sigAz);
        pMeasurementStandardDeviations->get(RadarMeasurementType::range(), sigR);
        pMeasurementStandardDeviations->get(RadarMeasurementType::rangeRate(), sigRd);
        pMeasurementStandardDeviations->get(RadarMeasurementType::zenith(), sigZe);
    }

    Matrix2d R(4, 4);

    R(0, 0) = sigAz * sigAz;
    R(1, 1) = sigZe * sigZe;
    R(2, 2) = sigR * sigR;
    R(2, 3) = m_rho * sigR * sigRd;
    R(3, 3) = sigRd * sigRd;
    R(3, 2) = m_rho * sigR * sigRd;

    return R;
}

/**
 * Process the measurement residual
 * @param yh the state estimate prior to measurement update
 * @param z  the state measurement vector
 */
Matrix2d RadarTrackFilter::computeMeasurementResidual(const Matrix2d &yh,
                                                      const Matrix2d &z)
{
    return z - yh;
}

/**
 * Compute process covariance
 * @param dt the sampling interval (s)
 */
Matrix2d RadarTrackFilter::computeProcessCovariance(double dt)
{
    auto dt2 = dt * dt;
    auto dt3 = dt * dt2;
    auto dt4 = dt * dt3;
    auto dt5 = dt * dt4;

    auto wx = m_maneuverVariance[0];
    auto wy = m_maneuverVariance[1];
    auto wz = m_maneuverVariance[2];

    Matrix2d Q(9, 9);
    Q(0, 0) = wx * dt5 / 20.0;
    Q(0, 3) = Q(3, 0) = wx * dt4 / 8.0;
    Q(0, 6) = Q(6, 0) = wx * dt3 / 6.0;
    Q(1, 1) = wy * dt5 / 20.0;
    Q(1, 4) = Q(4, 1) = wy * dt4 / 8.0;
    Q(1, 7) = Q(7, 1) = wy * dt3 / 6.0;
    Q(2, 2) = wz * dt5 / 20.0;
    Q(2, 5) = Q(5, 2) = wz * dt4 / 8.0;
    Q(2, 8) = Q(8, 2) = wz * dt3 / 6.0;

    Q(3, 3) = wx * dt3 / 3.0;
    Q(3, 6) = Q(6, 3) = wx * dt2 / 2.0;
    Q(4, 4) = wy * dt3 / 3.0;
    Q(4, 7) = Q(7, 4) = wy * dt2 / 2.0;
    Q(5, 5) = wz * dt3 / 3.0;
    Q(5, 8) = Q(8, 5) = wz * dt2 / 2.0;

    Q(6, 6) = wx * dt;
    Q(7, 7) = wy * dt;
    Q(8, 8) = wz * dt;

    return Q;
}

/**
 * Function to convert a map of state measurements to a vector of state measurements
 * @param stateMeasurement the input map of state measurements
 * @param stateVector      upon success, contains the information converted from the input map of state
 *                         measurements
 */
void RadarTrackFilter::convertStateMeasurementToStateVector(const StateMap &stateMeasurement,
                                                            StateVector &measurementVector)
{
    measurementVector.resize(4);
    stateMeasurement.get(RadarMeasurementType::azimuth(), measurementVector[0]);
    stateMeasurement.get(RadarMeasurementType::range(), measurementVector[2]);
    stateMeasurement.get(RadarMeasurementType::rangeRate(), measurementVector[3]);

    if (stateMeasurement.get(RadarMeasurementType::zenith(), measurementVector[1]))
        stateMeasurement.get(RadarMeasurementType::zenith(), measurementVector[1]);
    else // zenith not available, perhaps elevation is available
        stateMeasurement.get(RadarMeasurementType::elevation(), measurementVector[1]);
}

/**
 * create() function
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
RadarTrackFilter *RadarTrackFilter::create(EstimationFilter *pEstimationFilter)
{
    RadarTrackFilter *pTrackFilter = nullptr;
    if (pEstimationFilter != nullptr)
    {
        pTrackFilter = new RadarTrackFilter(pEstimationFilter);
        if (!pTrackFilter->setup())
        {
            delete pTrackFilter;
            pTrackFilter = nullptr;
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Debug,
                   "Created an instance of \"" + pTrackFilter->getClassName() + "\".\n",
                   getQualifiedStaticMethodName(__func__, RadarTrackFilter));
        }
    }
    else if (pEstimationFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Estimation filter is null!\n",
               getQualifiedStaticMethodName(__func__, RadarTrackFilter));
    }

    if (pTrackFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to instantiate/setup a new instance!\n",
               getQualifiedStaticMethodName(__func__, RadarTrackFilter));
    }

    return pTrackFilter;
}

/**
 * Evaluate the dynamics Jacobian
 * @param dt the sampling interval (s)
 * @param x  the state vector
 */
Matrix2d RadarTrackFilter::dynamicsJacobian(double dt,
                                            const Matrix2d &x) const
{
    Matrix2d F(9, 9);

    F(0, 0) = 1.0, F(0, 3) = dt; F(0, 6) = 0.5 * dt * dt;
    F(1, 1) = 1.0, F(1, 4) = dt; F(1, 7) = 0.5 * dt * dt;
    F(2, 2) = 1.0, F(2, 5) = dt; F(2, 8) = 0.5 * dt * dt;
    F(3, 3) = 1.0, F(3, 6) = dt;
    F(4, 4) = 1.0, F(4, 7) = dt;
    F(5, 5) = 1.0, F(5, 8) = dt;
    F(6, 6) = 1.0;
    F(7, 7) = 1.0;
    F(8, 8) = 1.0;

    return F;
}

/**
 * Evaluate the dynamics model
 * @param dt the sampling interval (s)
 * @param x  the state vector
 * @param u  an input vector
 */
Matrix2d RadarTrackFilter::dynamicsModel(double dt,
                                         const Matrix2d &x,
                                         const Matrix2d &u)
{
    auto &&F = dynamicsJacobian(dt, x);
    auto &&xh = F * x;

    if (m_pEstimationFilter != nullptr && !u.empty())
    {
        Matrix2d Q;
        m_pEstimationFilter->getMatrix("processCovariance", Q);
        xh += Q * u;
    }

    return xh;
}

/**
 * Get the name of this class
 */
std::string RadarTrackFilter::getClassName(void) const
{
    return "RadarTrackFilter";
}

/**
 * Get the factory name of this constructible
 */
std::string RadarTrackFilter::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get the maneuver variance
 */
void RadarTrackFilter::getManeuverVariance(double maneuverVariance[3]) const
{
    maneuverVariance[0] = m_maneuverVariance[0];
    maneuverVariance[1] = m_maneuverVariance[1];
    maneuverVariance[2] = m_maneuverVariance[2];
}

/**
 * Get the level of correlation between range and range rate
 */
double RadarTrackFilter::getRangeRateMeasurementCorrelationCoefficient(void) const
{
    return m_rho;
}

/**
 * Initialization function
 */
bool RadarTrackFilter::initialize(void)
{
    auto *pEstimationFilterUser = getEstimationFilterUser();
    bool bSuccess = (m_pEstimationFilter != nullptr &&
                     pEstimationFilterUser != nullptr);
    if (bSuccess)
    {
        StateVector xh;
        auto *pMeasurementStandardDeviations = pEstimationFilterUser->getMeasurementStandardDeviations();
        bSuccess = pEstimationFilterUser->estimateInitialState(xh);
        if (bSuccess)
            bSuccess = (pMeasurementStandardDeviations != nullptr);

        if (bSuccess)
        {
            // set the initialize state
            m_pEstimationFilter->setMatrix("stateEstimate", xh);

            // initialize measurement matrix
            if (m_pEstimationFilter->getVariableRegistry().contains("measurement"))
            {
                auto &&H = measurementJacobian(xh);
                m_pEstimationFilter->setMatrix("measurement", H);
            }

            // get the estimation filter update rate
            auto dt = m_pEstimationFilter->getUpdateRate();

            // initialize error covariance matrix
            if (m_pEstimationFilter->getVariableRegistry().contains("errorCovariance"))
            {
                auto &&P = computeErrorCovariance(xh, pMeasurementStandardDeviations, dt);
                m_pEstimationFilter->setMatrix("errorCovariance", P);
            }

            // initialize the process noise covariance matrix
            if (m_pEstimationFilter->getVariableRegistry().contains("processCovariance"))
            {
                auto &&Q = computeProcessCovariance(dt);
                m_pEstimationFilter->setMatrix("processCovariance", Q);
            }

            // initialize the measurement noise covariance matrix
            if (m_pEstimationFilter->getVariableRegistry().contains("measurementCovariance"))
            {
                auto &&R = computeMeasurementCovariance(pMeasurementStandardDeviations, dt);
                m_pEstimationFilter->setMatrix("measurementCovariance", R);
            }
        }
    }
    else if (m_pEstimationFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Estimation filter is null!\n",
               getQualifiedMethodName(__func__));
    }
    else if (pEstimationFilterUser == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Estimation filter user is null!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Evaluate the measurement Jacobian
 * @param x the state vector
 */
Matrix2d RadarTrackFilter::measurementJacobian(const Matrix2d &x) const
{
    Matrix2d H(4, 9); // Jacobian matrix to be populated

    // 1st row: partial derivatives of azimuth with respect to x, y, z, xd, yd, zd, xdd, ydd, zdd
    // 2nd row: partial derivatives of zenith with respect to x, y, z, xd, yd, zd, xdd, ydd, zdd
    // 3rd row: partial derivatives of range with respect to x, y, z, xd, yd, zd, xdd, ydd, zdd
    // 4th row: partial derivatives of azimuth rate with respect to x, y, z, xd, yd, zd, xdd, ydd, zdd

    auto rxySq = x[0] * x[0] + x[1] * x[1];
    auto rSq = rxySq + x[2] * x[2];
    auto r = sqrt(rSq);
    auto rd = (x[0] * x[3] + x[1] * x[4] + x[2] * x[5]) / r;

    if (rSq > 0)
    {
        H(0, 0) = -x[1] / rxySq;
        H(0, 1) =  x[0] / rxySq;
    }

    auto d = rSq * sqrt(rSq - x[2] * x[2]);
    if (d > 0)
    {
        H(1, 0) = x[0] * x[2] / d;
        H(1, 1) = x[1] * x[2] / d;
        H(1, 2) = -d / rSq / rSq;
    }

    if (r > 0)
    {
        H(2, 0) = x[0] / r;
        H(2, 1) = x[1] / r;
        H(2, 2) = x[2] / r;

        H(3, 0) = (r * x[3] - rd * x[0]) / rSq;
        H(3, 1) = (r * x[4] - rd * x[1]) / rSq;
        H(3, 2) = (r * x[5] - rd * x[2]) / rSq;
        H(3, 3) = H(2, 0);
        H(3, 4) = H(2, 1);
        H(3, 5) = H(2, 2);
    }

    return H;
}

/**
 * Evaluate the measurement model
 * @param x the state vector
 * @param u an input vector
 */
Matrix2d RadarTrackFilter::measurementModel(const Matrix2d &x,
                                            const Matrix2d &u)
{
    auto r = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
    auto rd = (x[0] * x[3] + x[1] * x[4] + x[2] * x[5]) / r;

    Matrix2d h(4, 1);

    // azimuth, zenith, range, azimuth rate, zenith rate, range rate
    h[0] = std::atan2(x[1], x[0]);
    h[1] = std::acos(x[2] / r);
    h[2] = r;
    h[3] = rd;

    if (m_pEstimationFilter != nullptr && !u.empty())
    {
        Matrix2d R;
        m_pEstimationFilter->getMatrix("measurementCovariance", R);
        h += R * u;
    }

    return h;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool RadarTrackFilter::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = AppliedEstimationFilter::readFromXML(pNode);
    if (bSuccess)
    {
        // read the range-range rate measurement correlation coefficient
        auto *pRangeRateMeasurementCorrelationCoefficientNode = pNode->
              first_node("rangeRateMeasurementCorrelationCoefficient");
        if (pRangeRateMeasurementCorrelationCoefficientNode != nullptr)
        {
            bSuccess = setRangeRateMeasurementCorrelationCoefficient(
                       std::stod(pRangeRateMeasurementCorrelationCoefficientNode->value()));
        }

        // read x maneuver variance
        auto *pX_ManeuverVarianceNode = pNode->first_node("xManeuverVariance");
        if (pX_ManeuverVarianceNode != nullptr)
            m_maneuverVariance[0] = std::stod(pX_ManeuverVarianceNode->value());

        // read y maneuver variance
        auto *pY_ManeuverVarianceNode = pNode->first_node("yManeuverVariance");
        if (pY_ManeuverVarianceNode != nullptr)
            m_maneuverVariance[1] = std::stod(pY_ManeuverVarianceNode->value());

        // read x maneuver variance
        auto *pZ_ManeuverVarianceNode = pNode->first_node("zManeuverVariance");
        if (pZ_ManeuverVarianceNode != nullptr)
            m_maneuverVariance[2] = std::stod(pZ_ManeuverVarianceNode->value());
    }

    return bSuccess;
}
#endif
/**
 * Set the maneuver variance
 */
void RadarTrackFilter::setManeuverVariance(double maneuverVariance[3])
{
    m_maneuverVariance[0] = maneuverVariance[0];
    m_maneuverVariance[1] = maneuverVariance[1];
    m_maneuverVariance[2] = maneuverVariance[2];
}

/**
 * Set the level (0.0 to 1.0) of correlation between range and range rate
 */
bool RadarTrackFilter::setRangeRateMeasurementCorrelationCoefficient(double rho)
{
    bool bSuccess = (rho >= 0.0 && rho <= 1.0);
    if (bSuccess)
    {
        m_rho = rho;
    }
    else
    {
        m_rho = 0.0;

        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Invalid range-range rate measurement correlation coefficient, resetting to " +
               std::to_string(m_rho) + "!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Setup function
 */
bool RadarTrackFilter::setup(void)
{
    bool bSuccess = AppliedEstimationFilter::setup();
    if (bSuccess)
    {
        m_registry["rangeRateMeasurementCorrelationCoefficient"] = m_rho;
        m_registry["xManeuverVariance"] = m_maneuverVariance[0];
        m_registry["yManeuverVariance"] = m_maneuverVariance[1];
        m_registry["zManeuverVariance"] = m_maneuverVariance[2];
    }

    return bSuccess;
}

/**
 * Swap function
 */
void RadarTrackFilter::swap(RadarTrackFilter &trackFilter)
{
    AppliedEstimationFilter::swap(trackFilter);

    std::swap(m_maneuverVariance[0], trackFilter.m_maneuverVariance[0]);
    std::swap(m_maneuverVariance[1], trackFilter.m_maneuverVariance[1]);
    std::swap(m_maneuverVariance[2], trackFilter.m_maneuverVariance[2]);
    std::swap(m_rho, trackFilter.m_rho);
}

}

}

}

}
