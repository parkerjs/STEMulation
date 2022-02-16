#ifndef RADAR_MEASUREMENT_H
#define RADAR_MEASUREMENT_H

#include "angle_unit_type.h"
#include "loggable.h"
#include "stateMap.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace applied
{

/**
 * This class represents a radar state measurement comprised of azimuth, range, range rate and zenith
 */
class RadarMeasurement
: public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public control_systems::StateMap,
  public attributes::interfaces::Swappable<RadarMeasurement>
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;
    using StateMap::swap;

    /**
     * Constructor
     * @param time the time at which this state is defined
     */
    EXPORT_STEM RadarMeasurement(double time = 0.0);

    /**
     * Constructor
     * @param angleUnits the units of this object's angles, Degrees or Radians
     * @param time       the time at which this state is defined
     */
    EXPORT_STEM RadarMeasurement(const AngleUnitType &angleUnits,
                                 double time = 0.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM RadarMeasurement(const RadarMeasurement &stateVector);

    /**
     * Move constructor
     */
    EXPORT_STEM RadarMeasurement(RadarMeasurement &&stateVector);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~RadarMeasurement(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RadarMeasurement &operator = (const RadarMeasurement &stateVector);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RadarMeasurement &operator = (RadarMeasurement &&stateVector);

    /**
     * clone() function
     */
    EXPORT_STEM virtual RadarMeasurement *clone(void) const override;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) final;

    /**
     * Get angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual AngleUnitType getAngleUnits(void) const final;

    /**
     * Get the measurement azimuth
     */
    EXPORT_STEM virtual double getAzimuth(void) final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the measurement elevation
     */
    EXPORT_STEM virtual double getElevation(void) final;

    /**
     * Get the measurement range
     */
    EXPORT_STEM virtual double getRange(void) final;

    /**
     * Get the measurement range rate
     */
    EXPORT_STEM virtual double getRangeRate(void) final;

    /**
     * Get the measurement zenith
     */
    EXPORT_STEM virtual double getZenith(void) final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) final;

    /**
     * Set the measurement azimuth
     */
    EXPORT_STEM virtual void setAzimuth(double azimuth) final;

    /**
     * Set the measurement azimuth
     */
    EXPORT_STEM virtual void setAzimuth(double azimuth,
                                        const AngleUnitType &angleUnits) final;

    /**
     * Set the measurement elevation
     */
    EXPORT_STEM virtual void setElevation(double elevation) final;

    /**
     * Set the measurement elevation
     * @param elevation  the measurement elevation angle
     * @param angleUnits the units of this object's angles, Degrees or Radians
     */
    EXPORT_STEM virtual void setElevation(double elevation,
                                          const AngleUnitType &angleUnits) final;

    /**
     * Set the measurement range
     */
    EXPORT_STEM virtual void setRange(double range) final;

    /**
     * Set the measurement range rate
     */
    EXPORT_STEM virtual void setRangeRate(double rangeRate) final;

    /**
     * Set the measurement zenith
     */
    EXPORT_STEM virtual void setZenith(double zenith) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(RadarMeasurement &stateVector) override final;

protected:

    /**
     * enumeration specifies whether angles are in degrees or radians
     */
    AngleUnitType m_angleUnits;
};

}

}

}

}

#endif
