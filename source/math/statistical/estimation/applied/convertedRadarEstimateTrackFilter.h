#ifndef CONVERTED_RADAR_ESTIMATE_TRACK_FILTER_H
#define CONVERTED_RADAR_ESTIMATE_TRACK_FILTER_H

#include "radarTrackFilter.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace applied
{

/**
 * This class implements a radar track filter which attempts to estimate the kinematic state of an object from
 * radar azimuth, elevation, range, and range rate measurements; the radar estimates are filtered and
 * propagated in radar coordinates and the state estimate is converted to Cartesian coordinates
 */
class ConvertedRadarEstimateTrackFilter
: public RadarTrackFilter,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<ConvertedRadarEstimateTrackFilter>
{
public:

    /**
     * Using declarations
     */
    using RadarTrackFilter::dynamicsModel;
    using RadarTrackFilter::measurementModel;
    using RadarTrackFilter::swap;

protected:

    /**
     * Constructor
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM ConvertedRadarEstimateTrackFilter(EstimationFilter *pEstimationFilter);

    /**
     * Copy constructor
     */
    EXPORT_STEM ConvertedRadarEstimateTrackFilter(ConvertedRadarEstimateTrackFilter &trackFilter);

    /**
     * Move constructor
     */
    EXPORT_STEM ConvertedRadarEstimateTrackFilter(ConvertedRadarEstimateTrackFilter &&trackFilter);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~ConvertedRadarEstimateTrackFilter(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM ConvertedRadarEstimateTrackFilter &
    operator = (ConvertedRadarEstimateTrackFilter &trackFilter);

    /**
     * Move assignment operator
     */
    EXPORT_STEM ConvertedRadarEstimateTrackFilter &
    operator = (ConvertedRadarEstimateTrackFilter &&trackFilter);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual ConvertedRadarEstimateTrackFilter *clone(void) override;

    /**
     * Compute error covariance
     * @param xh                             the state estimate vector
     * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
     * @param dt                             the sampling interval (s)
     */
    EXPORT_STEM virtual Matrix2d computeErrorCovariance(const Matrix2d &xh,
                                                        const StateMap *pMeasurementStandardDeviations,
                                                        double dt) override;

    /**
     * create() function
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    static EXPORT_STEM ConvertedRadarEstimateTrackFilter *create(EstimationFilter *pEstimationFilter);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

protected:

    /**
     * Evaluate the measurement Jacobian
     * @param x the state vector
     */
    EXPORT_STEM virtual Matrix2d measurementJacobian(const Matrix2d &x) const override;

    /**
     * Evaluate the measurement model
     * @param x the state vector
     * @param u an input vector
     */
    EXPORT_STEM virtual Matrix2d measurementModel(const Matrix2d &x,
                                                  const Matrix2d &u) override;

    /**
     * Perform post-processing on filtered data
     * @param xh the state estimate vector
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual void postProcessFilteredData(Matrix2d &xh,
                                                     double dt = 0.0) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(ConvertedRadarEstimateTrackFilter &trackFilter) override final;
};

}

}

}

}

#endif
