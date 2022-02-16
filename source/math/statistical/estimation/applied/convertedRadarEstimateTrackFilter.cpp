#include "convertedRadarEstimateTrackFilter.h"
#include "estimationFilter.h"
#include "radar_measurement_type.h"
#include "stateMap.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "ConvertedRadarEstimateTrackFilter";

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
ConvertedRadarEstimateTrackFilter::
ConvertedRadarEstimateTrackFilter(EstimationFilter *pEstimationFilter)
: RadarTrackFilter(pEstimationFilter)
{

}

/**
 * Copy constructor
 */
ConvertedRadarEstimateTrackFilter::
ConvertedRadarEstimateTrackFilter(ConvertedRadarEstimateTrackFilter &trackFilter)
{
    operator = (trackFilter);
}

/**
 * Move constructor
 */
ConvertedRadarEstimateTrackFilter::
ConvertedRadarEstimateTrackFilter(ConvertedRadarEstimateTrackFilter &&trackFilter)
{
    operator = (std::move(trackFilter));
}

/**
 * Destructor
 */
ConvertedRadarEstimateTrackFilter::~ConvertedRadarEstimateTrackFilter(void)
{

}

/**
 * Copy assignment operator
 */
ConvertedRadarEstimateTrackFilter &
ConvertedRadarEstimateTrackFilter::operator = (ConvertedRadarEstimateTrackFilter &trackFilter)
{
    if (&trackFilter != this)
    {
        RadarTrackFilter::operator = (trackFilter);

        // TODO: add members here
    }

    return *this;
}

/**
 * Move assignment operator
 */
ConvertedRadarEstimateTrackFilter &
ConvertedRadarEstimateTrackFilter::operator = (ConvertedRadarEstimateTrackFilter &&trackFilter)
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
ConvertedRadarEstimateTrackFilter *ConvertedRadarEstimateTrackFilter::clone(void)
{
    auto *pTrackFilter = new ConvertedRadarEstimateTrackFilter(*this);
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
 * Compute error covariance Compute error covariance
 * @param xh                             the state estimate vector
 * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
 * @param dt                             the sampling interval (s)
 */
Matrix2d ConvertedRadarEstimateTrackFilter::
computeErrorCovariance(const Matrix2d &xh,
                       const StateMap *pMeasurementStandardDeviations,
                       double dt)
{
    double sig_az = 0.0, sig_r = 0.0, sig_rr = 0.0, sig_ze = 0.0;
    if (pMeasurementStandardDeviations != nullptr)
    {
        pMeasurementStandardDeviations->get(RadarMeasurementType::azimuth(), sig_az);
        pMeasurementStandardDeviations->get(RadarMeasurementType::range(), sig_r);
        pMeasurementStandardDeviations->get(RadarMeasurementType::rangeRate(), sig_rr);
        pMeasurementStandardDeviations->get(RadarMeasurementType::zenith(), sig_ze);
    }

    auto sig_azd = sig_az / dt;
    auto sig_azdd = sig_azd / dt;
    auto sig_zed = sig_ze / dt;
    auto sig_zedd = sig_zed / dt;
    auto sig_rrd = sig_rr / dt;

    Matrix2d P(9, 9);
    P(0, 0) = sig_az * sig_az;
    P(1, 1) = sig_ze * sig_ze;
    P(2, 2) = sig_r * sig_r;
    P(3, 3) = sig_azd * sig_azd;
    P(4, 4) = sig_zed * sig_zed;
    P(5, 5) = sig_rr * sig_rr;
    P(6, 6) = sig_azdd * sig_azdd;
    P(7, 7) = sig_zedd * sig_zedd;
    P(8, 8) = sig_rrd * sig_rrd;

    return P;
}

/**
 * create() function
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
ConvertedRadarEstimateTrackFilter *
ConvertedRadarEstimateTrackFilter::create(EstimationFilter *pEstimationFilter)
{
    ConvertedRadarEstimateTrackFilter *pTrackFilter = nullptr;
    if (pEstimationFilter != nullptr)
    {
        pTrackFilter = new ConvertedRadarEstimateTrackFilter(pEstimationFilter);
        if (!pTrackFilter->setup())
        {
            delete pTrackFilter;
            pTrackFilter = nullptr;
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Debug,
                   "Created an instance of \"" + pTrackFilter->getClassName() + "\".\n",
                   getQualifiedStaticMethodName(__func__, ConvertedRadarEstimateTrackFilter));
        }
    }
    else if (pEstimationFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Estimation filter is null!\n",
               getQualifiedStaticMethodName(__func__, ConvertedRadarEstimateTrackFilter));
    }

    if (pTrackFilter == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to instantiate/setup a new instance!\n",
               getQualifiedStaticMethodName(__func__, ConvertedRadarEstimateTrackFilter));
    }

    return pTrackFilter;
}

/**
 * Get the name of this class
 */
std::string ConvertedRadarEstimateTrackFilter::getClassName(void) const
{
    return "ConvertedRadarEstimateTrackFilter";
}

/**
 * Get the factory name of this constructible
 */
std::string ConvertedRadarEstimateTrackFilter::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Evaluate the measurement Jacobian
 * @param x the state vector
 */
Matrix2d ConvertedRadarEstimateTrackFilter::measurementJacobian(const Matrix2d &x) const
{
    Matrix2d H(4, 9); // Jacobian matrix to be populated

    // 1st row: partial derivatives of azimuth with respect to az, ze, r, azd, zed, rd, azdd, zedd, rdd
    // 2nd row: partial derivatives of zenith with respect to az, ze, r, azd, zed, rd, azdd, zedd, rdd
    // 3rd row: partial derivatives of range with respect to az, ze, r, azd, zed, rd, azdd, zedd, rdd
    // 4th row: partial derivatives of range rate with respect to az, ze, r, azd, zed, rd, azdd, zedd, rdd

    H(0, 0) = 1.0;
    H(1, 1) = 1.0;
    H(2, 2) = 1.0;
    H(3, 5) = 1.0;

    return H;
}

/**
 * Evaluate the measurement model
 * @param x the state vector
 * @param u an input vector
 */
Matrix2d ConvertedRadarEstimateTrackFilter::measurementModel(const Matrix2d &x,
                                                             const Matrix2d &u)
{
    Matrix2d h(4, 1);

    h[0] = x[0];
    h[1] = x[1];
    h[2] = x[2];
    h[3] = x[5];

    return h;
}

/**
 * Perform post-processing on filtered data
 * @param xh the state estimate vector
 * @param dt the sampling interval (s)
 */
void ConvertedRadarEstimateTrackFilter::postProcessFilteredData(Matrix2d &xh,
                                                                double dt)
{
    enum { PosX, PosY, PosZ, VelX, VelY, VelZ, AccX, AccY, AccZ };

    auto az   = xh[0];
    auto azd  = xh[3];
    auto azdd = xh[6];
    auto ze   = xh[1];
    auto zed  = xh[4];
    auto zedd = xh[7];
    auto r    = xh[2];
    auto rd   = xh[5];
    auto rdd  = xh[8];

    auto cosAz = cos(az);
    auto sinAz = sin(az);
    auto cosZe = cos(ze);
    auto sinZe = sin(ze);

    auto rSinZe = r * sinZe;
    auto x = rSinZe * cosAz;
    auto y = rSinZe * sinAz;
    auto z = r * cosZe;

    auto rdSinZe = rd * sinZe;
    auto rZedCosZe = z * zed;
    auto rAzdSinZe = rSinZe * azd;
    auto xd = (rdSinZe + rZedCosZe) * cosAz - rAzdSinZe * sinAz;
    auto yd = (rdSinZe + rZedCosZe) * sinAz + rAzdSinZe * cosAz;
    auto zd = rd * cosZe - r * zed * sinZe;

    auto rAzd = r * azd;
    auto rAzdSq = rAzd * azd;
    auto rZedSq = r * zed * zed;
    auto rdd_rZedSq_rAzdSqSinZe = (rdd - rAzdSq - rZedSq) * sinZe;
    auto twoRdZed_rZedd = 2 * rd * zed + r * zedd;
    auto twoRdZed_rZeddCosZe = twoRdZed_rZedd * cosZe;
    auto twoRdAzd_rAzddSinZe = (2 * rd * azd + r * azdd) * sinZe;
    auto twoR_ZedAzdCosZe = 2 * rAzd * zed * cosZe;
    auto xdd = (rdd_rZedSq_rAzdSqSinZe + twoRdZed_rZeddCosZe) * cosAz
             - (twoRdAzd_rAzddSinZe + twoR_ZedAzdCosZe) * sinAz;
    auto ydd = (rdd_rZedSq_rAzdSqSinZe + twoRdZed_rZeddCosZe) * sinAz
             + (twoRdAzd_rAzddSinZe + twoR_ZedAzdCosZe) * cosAz;
    auto zdd = (rdd - rZedSq) * cosZe - twoRdZed_rZedd * sinZe;

    xh[PosX] = x;
    xh[PosY] = y;
    xh[PosZ] = z;
    xh[VelX] = xd;
    xh[VelY] = yd;
    xh[VelZ] = zd;
    xh[AccX] = xdd;
    xh[AccY] = ydd;
    xh[AccZ] = zdd;
}

/**
 * Swap function
 */
void ConvertedRadarEstimateTrackFilter::swap(ConvertedRadarEstimateTrackFilter &trackFilter)
{
    RadarTrackFilter::swap(trackFilter);

    // TODO: swap members here
}

}

}

}

}
