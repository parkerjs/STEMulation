#ifndef RADAR_TRACK_FILTER_H
#define RADAR_TRACK_FILTER_H

#include "appliedEstimationFilter.h"

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
 * radar azimuth, elevation, range, and range rate measurements
 */
class RadarTrackFilter
: public AppliedEstimationFilter,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<RadarTrackFilter>
{
public:

    /**
     * Using declarations
     */
    using AppliedEstimationFilter::dynamicsModel;
    using AppliedEstimationFilter::measurementModel;
    using AppliedEstimationFilter::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM RadarTrackFilter(void);

    /**
     * Constructor
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM RadarTrackFilter(EstimationFilter *pEstimationFilter);

    /**
     * Copy constructor
     */
    EXPORT_STEM RadarTrackFilter(RadarTrackFilter &trackFilter);

    /**
     * Move constructor
     */
    EXPORT_STEM RadarTrackFilter(RadarTrackFilter &&trackFilter);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~RadarTrackFilter(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RadarTrackFilter &operator = (RadarTrackFilter &trackFilter);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RadarTrackFilter &operator = (RadarTrackFilter &&trackFilter);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual RadarTrackFilter *clone(void) override;

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

protected:

    /**
     * Process the measurement residual
     * @param yh the state estimate prior to measurement update
     * @param z  the state measurement vector
     */
    EXPORT_STEM virtual Matrix2d computeMeasurementResidual(const Matrix2d &yh,
                                                            const Matrix2d &z) override;

public:

    /**
     * Compute process covariance
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual Matrix2d computeProcessCovariance(double dt) override;

    /**
     * Function to convert a map of state measurements to a vector of state measurements
     * @param stateMeasurement the input map of state measurements
     * @param stateVector      upon success, contains the information converted from the input map of state
     *                         measurements
     */
    EXPORT_STEM virtual void convertStateMeasurementToStateVector(const StateMap &stateMeasurement,
                                                                  StateVector &measurementVector) override;

    /**
     * create() function
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    static EXPORT_STEM RadarTrackFilter *create(EstimationFilter *pEstimationFilter);

    /**
     * Evaluate the dynamics Jacobian
     * @param dt the sampling interval (s)
     * @param x  the state vector
     */
    EXPORT_STEM virtual Matrix2d dynamicsJacobian(double dt,
                                                  const Matrix2d &x) const override;

    /**
     * Evaluate the dynamics model
     * @param dt the sampling interval (s)
     * @param x  the state vector
     * @param u  an input vector
     */
    EXPORT_STEM virtual Matrix2d dynamicsModel(double dt,
                                               const Matrix2d &x,
                                               const Matrix2d &u) override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

    /**
     * Get the maneuver variance
     */
    EXPORT_STEM virtual void getManeuverVariance(double maneuverVariance[3]) const final;

    /**
     * Get the level of correlation between range and range rate
     */
    EXPORT_STEM virtual double getRangeRateMeasurementCorrelationCoefficient(void) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

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
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set the maneuver variance
     */
    EXPORT_STEM virtual void setManeuverVariance(double maneuverVariance[3]) final;

    /**
     * Set the level (0.0 to 1.0) of correlation between range and range rate
     */
    EXPORT_STEM virtual bool setRangeRateMeasurementCorrelationCoefficient(double rho) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(RadarTrackFilter &trackFilter) override final;

protected:

    /**
     * maneuver variance vector
     */
    double m_maneuverVariance[3];

    /**
     * the level of correlation between range and range rate
     */
    double m_rho;
};

}

}

}

}

#endif
