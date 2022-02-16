#ifndef RADAR_TRACK_ESTIMATION_FILTER_INITIALIZER_H
#define RADAR_TRACK_ESTIMATION_FILTER_INITIALIZER_H

#include "cloneable.h"
#include "estimationFilterUser.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace applied
{

// forward declarations
class RadarMeasurement;

/**
 * This class implements an estimation filter user class for radar track estimation
 */
class RadarTrackEstimationFilterUser
: public attributes::interfaces::Cloneable<RadarTrackEstimationFilterUser>,
  public EstimationFilterUser,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<RadarTrackEstimationFilterUser>
{
public:

    /**
     * Using declarations
     */
    using EstimationFilterUser::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM RadarTrackEstimationFilterUser(void);

    /**
     * Constructor
     * @param pEstimationFilter               a pointer to the estimation filter associated with this object
     * @param pMeasurementAggregationStrategy a pointer to the measurement aggregation strategy
     */
    EXPORT_STEM RadarTrackEstimationFilterUser(EstimationFilter *pEstimationFilter,
                                               MeasurementAggregationStrategy *
                                               pMeasurementAggregationStrategy = nullptr);

    /**
     * Copy constructor
     */
    EXPORT_STEM RadarTrackEstimationFilterUser(const RadarTrackEstimationFilterUser &estimationFilterUser);

    /**
     * Move constructor
     */
    EXPORT_STEM RadarTrackEstimationFilterUser(RadarTrackEstimationFilterUser &&estimationFilterUser);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~RadarTrackEstimationFilterUser(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RadarTrackEstimationFilterUser &
    operator = (const RadarTrackEstimationFilterUser &estimationFilterUser);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RadarTrackEstimationFilterUser &
    operator = (RadarTrackEstimationFilterUser &&estimationFilterUser);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual RadarTrackEstimationFilterUser *clone(void) const override;

    /**
     * create() function
     * @param pEstimationFilter               a pointer to the estimation filter associated with this object
     * @param pMeasurementAggregationStrategy a pointer to the measurement aggregation strategy
     */
    static EXPORT_STEM RadarTrackEstimationFilterUser *
    create(EstimationFilter *pEstimationFilter,
           MeasurementAggregationStrategy *pMeasurementAggregationStrategy = nullptr);

    /**
     * Function to estimate the initial state from this object's measurements
     */
    EXPORT_STEM virtual bool estimateInitialState(StateVector &tateEstimate) override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get a pointer to the frame of reference associated with the measurement at the specified time
     */
    EXPORT_STEM virtual ReferenceFrame *getMeasurementFrame(double time) const override;

    /**
     * Get a pointer to the frame of reference associated with the state estimate at the specified time
     */
    EXPORT_STEM virtual ReferenceFrame *getStateEstimateFrame(double time) const override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(RadarTrackEstimationFilterUser &estimationFilterUser) override final;
};

}

}

}

}

#endif
