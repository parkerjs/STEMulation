#include "convertedRadarMeasurementTrackFilter.h"
#include "estimationFilter.h"
#include "radar_measurement_type.h"
#include "stateMap.h"
#include "stateVector.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "ConvertedRadarMeasurementTrackFilter";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix;
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
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
ConvertedRadarMeasurementTrackFilter::
ConvertedRadarMeasurementTrackFilter(EstimationFilter *pEstimationFilter)
: RadarTrackFilter(pEstimationFilter)
{

}

/**
 * Copy constructor
 */
ConvertedRadarMeasurementTrackFilter::
ConvertedRadarMeasurementTrackFilter(ConvertedRadarMeasurementTrackFilter &trackFilter)
{
    operator = (trackFilter);
}

/**
 * Move constructor
 */
ConvertedRadarMeasurementTrackFilter::
ConvertedRadarMeasurementTrackFilter(ConvertedRadarMeasurementTrackFilter &&trackFilter)
{
    operator = (std::move(trackFilter));
}

/**
 * Destructor
 */
ConvertedRadarMeasurementTrackFilter::~ConvertedRadarMeasurementTrackFilter(void)
{

}

/**
 * Copy assignment operator
 */
ConvertedRadarMeasurementTrackFilter &
ConvertedRadarMeasurementTrackFilter::operator = (ConvertedRadarMeasurementTrackFilter &trackFilter)
{
    if (&trackFilter != this)
    {
        RadarTrackFilter::operator = (trackFilter);

        m_az = trackFilter.m_az;
        m_r = trackFilter.m_r;
        m_ze = trackFilter.m_ze;
    }

    return *this;
}

/**
 * Move assignment operator
 */
ConvertedRadarMeasurementTrackFilter &
ConvertedRadarMeasurementTrackFilter::operator = (ConvertedRadarMeasurementTrackFilter &&trackFilter)
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
ConvertedRadarMeasurementTrackFilter *ConvertedRadarMeasurementTrackFilter::clone(void)
{
    auto *pTrackFilter = new ConvertedRadarMeasurementTrackFilter(*this);
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
Matrix2d ConvertedRadarMeasurementTrackFilter::
computeErrorCovariance(const Matrix2d &xh,
                       const StateMap *pMeasurementStandardDeviations,
                       double dt)
{
    Matrix2d P(9, 9);
    for (int i = 0; i < 9; ++i)
        P(i, i) = xh[i]* xh[i];

    return P;
}

/**
 * Compute measurement covariance
 * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
 * @param dt                             the sampling interval (s)
 */
Matrix2d ConvertedRadarMeasurementTrackFilter::
computeMeasurementCovariance(const StateMap *pMeasurementStandardDeviations,
                             double dt)
{
    auto cosAz = cos(m_az);
    auto cosZe = cos(m_ze);
    auto sinAz = sin(m_az);
    double sinZe = sin(m_ze);

    double sigAz = 0.0, sigR = 0.0, sigZe = 0.0;
    if (pMeasurementStandardDeviations != nullptr)
    {
        pMeasurementStandardDeviations->get(RadarMeasurementType::azimuth(), sigAz);
        pMeasurementStandardDeviations->get(RadarMeasurementType::range(), sigR);
        pMeasurementStandardDeviations->get(RadarMeasurementType::zenith(), sigZe);
    }

    auto R11  = m_r  * m_r  * sigAz * sigAz * sigZe * sigZe * sinAz * sinAz * cosZe * cosZe
              + m_r  * m_r  * sigAz * sigAz                 * sinAz * sinAz * sinZe * sinZe
              + m_r  * m_r  *                 sigZe * sigZe * cosAz * cosAz * cosZe * cosZe
              + sigR * sigR * sigAz * sigAz * sigZe * sigZe * sinAz * sinAz * cosZe * cosZe
              + sigR * sigR * sigAz * sigAz *                 sinAz * sinAz * sinZe * sinZe
              + sigR * sigR *                 sigZe * sigZe * cosAz * cosAz * cosZe * cosZe
              + sigR * sigR *                                 sinAz * sinAz * sinZe * sinZe;

    auto R12  = m_r  * m_r  * sinAz * cosAz *                 sigZe * sigZe * cosZe * cosZe
              - m_r  * m_r  * sinAz * cosAz * sigAz * sigAz * sigZe * sigZe * cosZe * cosZe
              - m_r  * m_r  * sinAz * cosAz * sigAz * sigAz *                 sinZe * sinZe
              + sigR * sigR * sinAz * cosAz *                 sigZe * sigZe * cosZe * cosZe
              - sigR * sigR * sinAz * cosAz * sigAz * sigAz * sigZe * sigZe * cosZe * cosZe
              - sigR * sigR * sinAz * cosAz * sigAz * sigAz *                 sinZe * sinZe
              + sigR * sigR * sinAz * cosAz *                                 sinZe * sinZe;

    auto R13  = cosZe * sinZe * cosAz * (sigR * sigR - m_r * m_r * sigZe * sigZe -
                                         sigR * sigR * sigZe * sigZe);

    auto R22  = m_r  * m_r  * sigAz * sigAz * sigZe * sigZe * cosAz * cosAz * cosZe * cosZe
              + m_r  * m_r  * sigAz * sigAz                 * cosAz * cosAz * sinZe * sinZe
              + m_r  * m_r  *                 sigZe * sigZe * sinAz * sinAz * cosZe * cosZe
              + sigR * sigR * sigAz * sigAz * sigZe * sigZe * cosAz * cosAz * cosZe * cosZe
              + sigR * sigR * sigAz * sigAz *                 cosAz * cosAz * sinZe * sinZe
              + sigR * sigR *                 sigZe * sigZe * sinAz * sinAz * cosZe * cosZe
              + sigR * sigR *                                 sinAz * sinAz * sinZe * sinZe;

    auto R23  = cosZe * sinZe * sinAz * (sigR * sigR - m_r * m_r * sigZe * sigZe -
                                         sigR * sigR * sigZe * sigZe);

    auto R33  = m_r * m_r * sigZe * sigZe * sinZe * sinZe
              + sigR * sigR * (cosZe * cosZe + sigZe * sigZe * sinZe * sinZe);

    Matrix2d R(3, 3);

    R(0, 0) = R11; R(0, 1) = R12; R(0, 2) = R13;
    R(1, 0) = R12; R(1, 1) = R22; R(1, 2) = R23;
    R(2, 0) = R13; R(2, 1) = R23; R(2, 2) = R33;

    return R;
}

/**
 * create() function
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
ConvertedRadarMeasurementTrackFilter *
ConvertedRadarMeasurementTrackFilter::create(EstimationFilter *pEstimationFilter)
{
    ConvertedRadarMeasurementTrackFilter *pTrackFilter = nullptr;
    if (pEstimationFilter != nullptr)
    {
        pTrackFilter = new ConvertedRadarMeasurementTrackFilter(pEstimationFilter);
        if (!pTrackFilter->setup())
        {
            delete pTrackFilter;
            pTrackFilter = nullptr;
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Debug,
                   "Created an instance of \"" + pTrackFilter->getClassName() + "\".\n",
                   getQualifiedStaticMethodName(__func__, ConvertedRadarMeasurementTrackFilter));
        }
    }
    else if (pEstimationFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Estimation filter is null!\n",
               getQualifiedStaticMethodName(__func__, ConvertedRadarMeasurementTrackFilter));
    }

    if (pTrackFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to instantiate/setup a new instance!\n",
               getQualifiedStaticMethodName(__func__, ConvertedRadarMeasurementTrackFilter));
    }

    return pTrackFilter;
}

/**
 * Get the name of this class
 */
std::string ConvertedRadarMeasurementTrackFilter::getClassName(void) const
{
    return "ConvertedRadarMeasurementTrackFilter";
}

/**
 * Get the factory name of this constructible
 */
std::string ConvertedRadarMeasurementTrackFilter::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Evaluate the measurement Jacobian
 * @param x the state vector
 */
Matrix2d ConvertedRadarMeasurementTrackFilter::measurementJacobian(const Matrix2d &x) const
{
    Matrix2d H(3, 9);
    H(0, 0) = 1.0;
    H(1, 1) = 1.0;
    H(2, 2) = 1.0;

    return H;
}

/**
 * Evaluate the measurement model
 * @param x the state vector
 * @param u an input vector
 */
Matrix2d ConvertedRadarMeasurementTrackFilter::measurementModel(const Matrix2d &x,
                                                                const Matrix2d &u)
{
    Matrix2d h(3, 1);
    h[0] = x[0];
    h[1] = x[1];
    h[2] = x[2];

    return h;
}

/**
 * Perform pre-processing on measured data prior to filtering
 * @param stateMeasurement a reference to a state vector object
 * @param dt               the sampling interval (s)
 */
void ConvertedRadarMeasurementTrackFilter::preProcessMeasurementData(StateVector &stateMeasurement,
                                                                     double dt)
{
    m_az = stateMeasurement[0];
    m_ze = stateMeasurement[1];
    m_r  = stateMeasurement[2];

    auto cosAz = cos(m_az);
    auto cosZe = cos(m_ze);
    auto sinAz = sin(m_az);
    auto sinZe = sin(m_ze);
    auto rSinZe = m_r * sinZe;

    stateMeasurement[0] = rSinZe * cosAz;
    stateMeasurement[1] = rSinZe * sinAz;
    stateMeasurement[2] = m_r * cosZe;
}

/**
 * Swap function
 */
void ConvertedRadarMeasurementTrackFilter::swap(ConvertedRadarMeasurementTrackFilter &trackFilter)
{
    RadarTrackFilter::swap(trackFilter);

    std::swap(m_az, trackFilter.m_az);
    std::swap(m_r, trackFilter.m_r);
    std::swap(m_ze, trackFilter.m_ze);
}

}

}

}

}
