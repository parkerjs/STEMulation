#include "latestMeasurementStrategy.h"
#include "measurementAggregationStrategy.h"
#include "radarMeasurement.h"
#include "radarTrackEstimationFilterUser.h"
#include "referenceFrame.h"
#include "stateVector.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "RadarTrackEstimationFilterUser";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::control_systems;
using namespace physics::kinematics;
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
static FactoryRegistrar<EstimationFilterUser>
factory(factoryName, &RadarTrackEstimationFilterUser::create);

/**
 * Constructor
 */
RadarTrackEstimationFilterUser::RadarTrackEstimationFilterUser(void)
{

}

/**
 * Constructor
 * @param pEstimationFilter               a pointer to the estimation filter associated with this object
 * @param pMeasurementAggregationStrategy a pointer to the measurement aggregation strategy
 */
RadarTrackEstimationFilterUser::
RadarTrackEstimationFilterUser(EstimationFilter *pEstimationFilter,
                               MeasurementAggregationStrategy *pMeasurementAggregationStrategy)
: EstimationFilterUser(pEstimationFilter,
                       pMeasurementAggregationStrategy)
{

}

/**
 * Copy constructor
 */
RadarTrackEstimationFilterUser::
RadarTrackEstimationFilterUser(const RadarTrackEstimationFilterUser &estimationFilterUser)
{
    operator = (estimationFilterUser);
}

/**
 * Move constructor
 */
RadarTrackEstimationFilterUser::
RadarTrackEstimationFilterUser(RadarTrackEstimationFilterUser &&estimationFilterUser)
{
    operator = (std::move(estimationFilterUser));
}

/**
 * Destructor
 */
RadarTrackEstimationFilterUser::~RadarTrackEstimationFilterUser(void)
{

}

/**
 * Copy assignment operator
 */
RadarTrackEstimationFilterUser &RadarTrackEstimationFilterUser::
operator = (const RadarTrackEstimationFilterUser &estimationFilterUser)
{
    if (&estimationFilterUser != this)
    {
        EstimationFilterUser::operator = (estimationFilterUser);

        // TODO: add members here
    }

    return *this;
}

/**
 * Move assignment operator
 */
RadarTrackEstimationFilterUser &RadarTrackEstimationFilterUser::
operator = (RadarTrackEstimationFilterUser &&estimationFilterUser)
{
    if (&estimationFilterUser != this)
    {
        estimationFilterUser.swap(*this);
    }

    return *this;
}

/**
 * clone() function
 */
RadarTrackEstimationFilterUser *RadarTrackEstimationFilterUser::clone(void) const
{
    return new RadarTrackEstimationFilterUser(*this);
}

/**
 * create() function
 * @param pEstimationFilter               a pointer to the estimation filter associated with this object
 * @param pMeasurementAggregationStrategy a pointer to the measurement aggregation strategy
 */
RadarTrackEstimationFilterUser *
RadarTrackEstimationFilterUser::create(EstimationFilter *pEstimationFilter,
                                       MeasurementAggregationStrategy *pMeasurementAggregationStrategy)
{
    RadarTrackEstimationFilterUser *pEstimationFilterUser = nullptr;
    if (pEstimationFilter != nullptr)
    {
        pEstimationFilterUser = new RadarTrackEstimationFilterUser(pEstimationFilter,
                                                                   pMeasurementAggregationStrategy);
    }

    return pEstimationFilterUser;
}

/**
 * Function to estimate the initial state from this object's measurements
 */
bool RadarTrackEstimationFilterUser::estimateInitialState(StateVector &stateEstimate)
{
    bool bSuccess = (m_pMeasurementAggregationStrategy != nullptr);
    if (bSuccess)
    {
        RadarMeasurement measurement;
        auto &&stateMeasurements = getStateMeasurements();
        bSuccess = m_pMeasurementAggregationStrategy->aggregate(stateMeasurements,
                                                                measurement);
        if (bSuccess)
        {
            double az = measurement.getAzimuth();
            double r = measurement.getRange();
            double rd = measurement.getRangeRate();
            double ze = measurement.getZenith();

            auto cosAz = std::cos(az);
            auto sinAz = std::sin(az);
            auto cosZe = std::cos(ze);
            auto sinZe = std::sin(ze);

            auto rSinZe = r * sinZe;
            auto x = rSinZe * cosAz;
            auto y = rSinZe * sinAz;
            auto z = r * cosZe;

            // resize the state
            stateEstimate.resize(9);

            // position estimates
            stateEstimate[0] = x;
            stateEstimate[1] = y;
            stateEstimate[2] = z;

            // compute velocity estimates
            auto azd = 0.0, zed = 0.0;

            // velocity estimates
            auto rdSinZe = rd * sinZe;
            auto rZedCosZe = z * zed;
            auto rAzdSinZe = rSinZe * azd;
            auto xd = (rdSinZe + rZedCosZe) * cosAz - rAzdSinZe * sinAz;
            auto yd = (rdSinZe + rZedCosZe) * sinAz + rAzdSinZe * cosAz;
            auto zd = rd * cosZe - r * zed * sinZe;

            stateEstimate[3] = xd;
            stateEstimate[4] = yd;
            stateEstimate[5] = zd;

            // acceleration estimates
            stateEstimate[6] = 0.0;
            stateEstimate[7] = 0.0;
            stateEstimate[8] = 0.0;
        }
    }
    else if (m_pMeasurementAggregationStrategy == nullptr)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Measurement aggregation strategy is null!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string RadarTrackEstimationFilterUser::getClassName(void) const
{
    return "RadarTrackEstimationFilterUser";
}

/**
 * Get the factory name of this constructible
 */
std::string RadarTrackEstimationFilterUser::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get a pointer to the frame of reference associated with the measurement at the specified time
 */
ReferenceFrame *RadarTrackEstimationFilterUser::getMeasurementFrame(double time) const
{
    return nullptr; // not used
}

/**
 * Get a pointer to the frame of reference associated with the state estimate at the specified time
 */
ReferenceFrame *RadarTrackEstimationFilterUser::getStateEstimateFrame(double time) const
{
    return nullptr; // not used
}

/**
 * Swap function
 */
void RadarTrackEstimationFilterUser::
swap(RadarTrackEstimationFilterUser &estimationFilterUser)
{
    EstimationFilterUser::swap(estimationFilterUser);

    // TODO: swap members here
}

}

}

}

}
