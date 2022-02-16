#include "math_constants.h"
#include "radar_measurement_type.h"
#include "radarMeasurement.h"

// using namespace declarations
using namespace math::trigonometric;
using namespace utilities;

namespace math
{

namespace statistical
{

namespace estimation
{

namespace applied
{

/**
 * Constructor
 * @param time the time at which this state is defined
 */
RadarMeasurement::RadarMeasurement(double time)
: StateMap(time),
  m_angleUnits(AngleUnitType::Degrees)
{
    m_stateMap[RadarMeasurementType::azimuth()] = 0.0;
    m_stateMap[RadarMeasurementType::range()] = 0.0;
    m_stateMap[RadarMeasurementType::rangeRate()] = 0.0;
    m_stateMap[RadarMeasurementType::zenith()] = 0.0;
}

/**
 * Constructor
 * @param angleUnits the units of this object's angles, Degrees or Radians
 * @param time       the time at which this state is defined
 */
RadarMeasurement::RadarMeasurement(const AngleUnitType &angleUnits,
                                   double time)
: StateMap(time),
  m_angleUnits(angleUnits)
{

}

/**
 * Copy constructor
 */
RadarMeasurement::RadarMeasurement(const RadarMeasurement &stateVector)
{
    operator = (stateVector);
}

/**
 * Move constructor
 */
RadarMeasurement::RadarMeasurement(RadarMeasurement &&stateVector)
{
    operator = (std::move(stateVector));
}

/**
 * Destructor
 */
RadarMeasurement::~RadarMeasurement(void)
{

}

/**
 * Copy assignment operator
 */
RadarMeasurement &
RadarMeasurement::operator = (const RadarMeasurement &stateVector)
{
    if (&stateVector != this)
    {
        Loggable<std::string, std::ostream>::operator = (stateVector);
        StateMap::operator = (stateVector);

        m_angleUnits = stateVector.m_angleUnits;
    }

    return *this;
}

/**
 * Move assignment operator
 */
RadarMeasurement &
RadarMeasurement::operator = (RadarMeasurement &&stateVector)
{
    if (&stateVector != this)
    {
        stateVector.swap(*this);
    }

    return *this;
}

/**
 * clone() function
 */
RadarMeasurement *RadarMeasurement::clone(void) const
{
    return new RadarMeasurement(*this);
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void RadarMeasurement::convertAngleUnits(const AngleUnitType &angleUnits)
{
    if (m_angleUnits != angleUnits)
    {
        double cnv = 1.0;
        if (angleUnits == AngleUnitType::Degrees && m_angleUnits == AngleUnitType::Radians)
            cnv = RADIANS_TO_DEGREES;
        else if (angleUnits == AngleUnitType::Radians && m_angleUnits == AngleUnitType::Degrees)
            cnv = DEGREES_TO_RADIANS;

        if (cnv != 1.0)
        {
            m_stateMap[RadarMeasurementType::azimuth()] *= cnv;
            m_stateMap[RadarMeasurementType::zenith()] *= cnv;
        }

        m_angleUnits = angleUnits;
    }
}

/**
 * Get angle units (Degrees or Radians)
 */
AngleUnitType RadarMeasurement::getAngleUnits(void) const
{
    return m_angleUnits;
}

/**
 * Get the measurement azimuth
 */
double RadarMeasurement::getAzimuth(void)
{
    return m_stateMap[RadarMeasurementType::azimuth()];
}

/**
 * Get the name of this class
 */
std::string RadarMeasurement::getClassName(void) const
{
    return "RadarMeasurement";
}

/**
 * Get the measurement elevation
 */
double RadarMeasurement::getElevation(void)
{
    auto angleShift = PI_OVER_TWO;
    if (m_angleUnits == AngleUnitType::Degrees)
        angleShift *= RADIANS_TO_DEGREES;

    return angleShift - m_stateMap[RadarMeasurementType::zenith()];
}

/**
 * Get the measurement range
 */
double RadarMeasurement::getRange(void)
{
    return m_stateMap[RadarMeasurementType::range()];
}

/**
 * Get the measurement range rate
 */
double RadarMeasurement::getRangeRate(void)
{
    return m_stateMap[RadarMeasurementType::rangeRate()];
}

/**
 * Get the measurement zenith
 */
double RadarMeasurement::getZenith(void)
{
    return m_stateMap[RadarMeasurementType::zenith()];
}

/**
 * Initialization function
 */
bool RadarMeasurement::initialize(void)
{
    bool bSuccess = StateMap::initialize();
    if (bSuccess)
    {
        setAzimuth(0.0);
        setRange(0.0);
        setRangeRate(0.0);
        setZenith(0.0);
    }

    return bSuccess;
}

/**
 * Set angle units (Degrees or Radians)
 */
void RadarMeasurement::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
}

/**
 * Set the measurement azimuth
 */
void RadarMeasurement::setAzimuth(double azimuth)
{
    setAzimuth(azimuth, m_angleUnits);
}

/**
 * Set the measurement azimuth
 */
void RadarMeasurement::setAzimuth(double azimuth,
                                  const AngleUnitType &angleUnits)
{
    double cnv = 1.0;
    if (angleUnits == AngleUnitType::Degrees && m_angleUnits == AngleUnitType::Radians)
        cnv = RADIANS_TO_DEGREES;
    else if (angleUnits == AngleUnitType::Radians && m_angleUnits == AngleUnitType::Degrees)
        cnv = DEGREES_TO_RADIANS;

    m_stateMap[RadarMeasurementType::azimuth()] = cnv * azimuth;
}

/**
 * Set the measurement elevation
 */
void RadarMeasurement::setElevation(double elevation)
{
    setElevation(elevation, m_angleUnits);
}

/**
 * Set the measurement elevation
 * @param elevation  the measurement elevation angle
 * @param angleUnits the units of this object's angles, Degrees or Radians
 */
void RadarMeasurement::setElevation(double elevation,
                                    const AngleUnitType &angleUnits)
{
    auto angleShift = PI_OVER_TWO;
    if (m_angleUnits == AngleUnitType::Degrees)
        angleShift *= RADIANS_TO_DEGREES;

    double cnv = 1.0;
    if (angleUnits == AngleUnitType::Degrees && m_angleUnits == AngleUnitType::Radians)
        cnv = RADIANS_TO_DEGREES;
    else if (angleUnits == AngleUnitType::Radians && m_angleUnits == AngleUnitType::Degrees)
        cnv = DEGREES_TO_RADIANS;

    setZenith(angleShift - cnv * elevation);
}

/**
 * Set the measurement range
 */
void RadarMeasurement::setRange(double range)
{
    m_stateMap[RadarMeasurementType::range()] = range;
}

/**
 * Set the measurement range rate
 */
void RadarMeasurement::setRangeRate(double rangeRate)
{
    m_stateMap[RadarMeasurementType::rangeRate()] = rangeRate;
}

/**
 * Set the measurement zenith
 */
void RadarMeasurement::setZenith(double zenith)
{
    m_stateMap[RadarMeasurementType::zenith()] = zenith;
}

/**
 * Swap function
 */
void RadarMeasurement::swap(RadarMeasurement &stateVector)
{
    Loggable<std::string, std::ostream>::swap(stateVector);
    StateMap::swap(stateVector);

    std::swap(m_angleUnits, stateVector.m_angleUnits);
}

}

}

}

}
