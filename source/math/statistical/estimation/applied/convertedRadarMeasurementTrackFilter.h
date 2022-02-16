#ifndef CONVERTED_RADAR_MEASUREMENT_TRACK_FILTER_H
#define CONVERTED_RADAR_MEASUREMENT_TRACK_FILTER_H

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
 * radar azimuth, elevation, range, and range rate measurements; the radar measurements are converted to
 * Cartesian coordinates prior to estimation
 */
class ConvertedRadarMeasurementTrackFilter
: public RadarTrackFilter,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<ConvertedRadarMeasurementTrackFilter>
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
    EXPORT_STEM ConvertedRadarMeasurementTrackFilter(EstimationFilter *pEstimationFilter);

    /**
     * Copy constructor
     */
    EXPORT_STEM ConvertedRadarMeasurementTrackFilter(ConvertedRadarMeasurementTrackFilter &trackFilter);

    /**
     * Move constructor
     */
    EXPORT_STEM ConvertedRadarMeasurementTrackFilter(ConvertedRadarMeasurementTrackFilter &&trackFilter);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~ConvertedRadarMeasurementTrackFilter(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM ConvertedRadarMeasurementTrackFilter &
    operator = (ConvertedRadarMeasurementTrackFilter &trackFilter);

    /**
     * Move assignment operator
     */
    EXPORT_STEM ConvertedRadarMeasurementTrackFilter &
    operator = (ConvertedRadarMeasurementTrackFilter &&trackFilter);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual ConvertedRadarMeasurementTrackFilter *clone(void) override;

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
     * Compute measurement covariance
     * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
     * @param dt                             the sampling interval (s)
     */
    EXPORT_STEM virtual Matrix2d computeMeasurementCovariance(const StateMap *pMeasurementStandardDeviations,
                                                              double dt) override;

    /**
     * create() function
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    static EXPORT_STEM ConvertedRadarMeasurementTrackFilter *
    create(EstimationFilter *pEstimationFilter);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

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
     * Perform pre-processing on measured data prior to filtering
     * @param stateMeasurement a reference to a state vector object
     * @param dt               the sampling interval (s)
     */
    EXPORT_STEM virtual void preProcessMeasurementData(StateVector &stateMeasurement,
                                                       double dt = 0.0) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(ConvertedRadarMeasurementTrackFilter &trackFilter) override final;

protected:

    /**
     * the latest azimuth measurement
     */
    double m_az;

    /**
     * the latest range measurement
     */
    double m_r;

    /**
     * the latest zenith measurement
     */
    double m_ze;
};

}

}

}

}

#endif
