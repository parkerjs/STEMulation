#include "angularSubtense.h"
#include "math_constants.h"
#include "phase_unwrapper.h"

// using namespace declarations
using namespace math::control_systems::blocks;
using namespace math::trigonometric;

namespace math
{

namespace geometric
{

/**
 * Constructor
 * @param angleUnits the angle units associated with this object (Degrees or Radians)
 */
AngularSubtense::AngularSubtense(const AngleUnitType &angleUnits)
: m_angles(0.0, 0.0),
  m_angleUnits(angleUnits)
{

}

/**
 * Constructor
 * @param alpha      the first angle in the subtense
 * @param beta       the second angle in the subtense
 * @param angleUnits the angle units associated with the input angles (Degrees or Radians)
 */
AngularSubtense::AngularSubtense(double alpha,
                                 double beta,
                                 const AngleUnitType &angleUnits)
: m_angles(alpha, beta),
  m_angleUnits(angleUnits)
{

}

/**
 * Constructor
 * @param subtense   the pair of angles which define the subtense
 * @param angleUnits the angle units associated with the input angles (Degrees or Radians)
 */
AngularSubtense::AngularSubtense(const std::pair<double, double> &subtense,
                                 const AngleUnitType &angleUnits)
: m_angles(subtense.first, subtense.second),
  m_angleUnits(angleUnits)
{

}

/**
 * Copy constructor
 */
AngularSubtense::AngularSubtense(const AngularSubtense &subtense)
{
    operator = (subtense);
}

/**
 * Move constructor
 */
AngularSubtense::AngularSubtense(AngularSubtense &&subtense)
{
    operator = (std::move(subtense));
}

/**
 * Destructor
 */
AngularSubtense::~AngularSubtense(void)
{

}

/**
 * Copy assignment operator
 */
AngularSubtense &AngularSubtense::operator = (const AngularSubtense &subtense)
{
    if (&subtense != this)
    {
        m_angleUnits = subtense.m_angleUnits;
        m_angles = subtense.m_angles;
    }

    return *this;
}

/**
 * Move assignment operator
 */
AngularSubtense &AngularSubtense::operator = (AngularSubtense &&subtense)
{
    if (&subtense != this)
    {
        m_angleUnits = std::move(subtense.m_angleUnits);
        m_angles = std::move(subtense.m_angles);
    }

    return *this;
}

/**
 * Determine if the input angle is within the angular subtense
 */
bool AngularSubtense::angleWithinSubtense(double angle) const
{
    return angleWithinSubtense(angle,
                               m_angles.first,
                               m_angles.second,
                               m_angleUnits);
}

/**
 * Determine if the input angle is within the angular subtense defined by angles alpha and beta; see class
 * description for explanation regarding how alpha and beta are used to define the angular subtense
 * @param angle      the angle for which the test will be performed
 * @param alpha      the first angle in the subtense
 * @param beta       the second angle in the subtense
 * @param angleUnits the angle units associated with this object (Degrees or Radians)
 */
bool AngularSubtense::angleWithinSubtense(double angle,
                                          double alpha,
                                          double beta,
                                          const AngleUnitType &angleUnits)
{
    bool bWithinSubtense = false;
    auto delta = calcDeltaFromCenter(angle, alpha, beta, angleUnits);
    auto width = calcWidth(alpha, beta, angleUnits);
    if (std::fabs(delta) <= 0.5 * width)
        bWithinSubtense = true;

    return bWithinSubtense;
}

/**
 * Calculate the center of the subtense
 */
double AngularSubtense::calcCenter(void) const
{
    return calcCenter(m_angles.first,
                      m_angles.second,
                      m_angleUnits);
}

/**
 * Calculate the center of the subtense defined by angles alpha and beta; see class description for explanation
 * regarding how alpha and beta are used to define the angular subtense
 * @param alpha      the first angle in the subtense
 * @param beta       the second angle in the subtense
 * @param angleUnits the angle units associated with this object (Degrees or Radians)
 */
double AngularSubtense::calcCenter(double alpha,
                                   double beta,
                                   const AngleUnitType &angleUnits)
{
    auto width = calcWidth(alpha, beta, angleUnits);
    double cnv = 1.0;
    if (angleUnits == AngleUnitType::Radians)
        cnv = DEGREES_TO_RADIANS;

    return std::fmod(beta - 0.5 * width, 360.0 * cnv);
}

/**
 * Calculate the delta of an angle relative to the center of the subtense
 */
double AngularSubtense::calcDeltaFromCenter(double angle) const
{
    return calcDeltaFromCenter(angle,
                               m_angles.first,
                               m_angles.second,
                               m_angleUnits);
}

/**
 * Calculate the delta of an angle relative to the center of the subtense defined by angles alpha and beta; see
 * class description for explanation regarding how alpha and beta are used to define the angular subtense
 * @param angle      the angle for which the delta will be calculate
 * @param alpha      the first angle in the subtense
 * @param beta       the second angle in the subtense
 * @param angleUnits the angle units associated with this object (Degrees or Radians)
 */
double AngularSubtense::calcDeltaFromCenter(double angle,
                                            double alpha,
                                            double beta,
                                            const AngleUnitType &angleUnits)
{
    auto center = calcCenter(alpha, beta, angleUnits);
    double cnv = 1.0;
    if (angleUnits == AngleUnitType::Radians)
        cnv = DEGREES_TO_RADIANS;

    return PhaseUnwrapper<double>::apply(angle - center, cnv  * -180.0, cnv * 180.0);
}

/**
 * Calculate the with of the angular subtense
 */
double AngularSubtense::calcWidth(void) const
{
    return calcWidth(m_angles.first,
                     m_angles.second,
                     m_angleUnits);
}

/**
 * Calculate the with of the angular subtense defined by angles alpha and beta; see class description for
 * explanation regarding how alpha and beta are used to define the angular subtense
 * @param alpha      the first angle in the subtense
 * @param beta       the second angle in the subtense
 * @param angleUnits the angle units associated with this object (Degrees or Radians)
 */
double AngularSubtense::calcWidth(double alpha,
                                  double beta,
                                  const AngleUnitType &angleUnits)
{
    double cnv = 1.0;
    if (angleUnits == AngleUnitType::Radians)
        cnv = DEGREES_TO_RADIANS;

    auto cycle = cnv * 360.0;
    auto width = beta - alpha;
    if (width < 0.0)
    {
        width = PhaseUnwrapper<double>::apply(width, 0.0, cycle) + cycle * std::floor(-width / cycle);
    }

    return width;
}

/**
 * Get the pair of angles which define the subtense
 */
std::pair<double, double> &AngularSubtense::getAngles(void)
{
    return m_angles;
}

/**
 * Get the pair of angles which define the subtense
 */
std::pair<double, double> AngularSubtense::getAngles(void) const
{
    return m_angles;
}

/**
 * Get the angle units associated with this object (Degrees or Radians)
 */
AngleUnitType AngularSubtense::getAngleUnits(void) const
{
    return m_angleUnits;
}

/**
 * Get the name of this class
 */
std::string AngularSubtense::getClassName(void) const
{
    return "AngularSubtense";
}

/**
 * Return whether or not this object's subtense is defined in a clockwise fashion
 */
bool AngularSubtense::isClockwise(void) const
{
    return isClockwise(m_angles.first, m_angles.second);
}

/**
 * Return whether or not this object's subtense is defined in a clockwise fashion; see class description for
 * explanation regarding how alpha and beta are used to define the angular subtense
 * @param alpha the first angle in the subtense
 * @param beta  the second angle in the subtense
 */
bool AngularSubtense::isClockwise(double alpha,
                                  double beta)
{
    return alpha < beta;
}

/**
 * Return whether or not this object's subtense is defined in a clockwise fashion
 */
bool AngularSubtense::isCounterClockwise(void) const
{
    return !isClockwise();
}

/**
 * Return whether or not this object's subtense is defined in a clockwise fashion; see class description for
 * explanation regarding how alpha and beta are used to define the angular subtense
 * @param alpha the first angle in the subtense
 * @param beta  the second angle in the subtense
 */
bool AngularSubtense::isCounterClockwise(double alpha,
                                         double beta)
{
    return !isClockwise(alpha, beta);
}

/**
 * Function to limit the input angle to within the bounds of this angular subtense
 */
double AngularSubtense::limit(double angle) const
{
    return limit(angle, m_angles.first, m_angles.second, m_angleUnits);
}

/**
 * Function to limit the input angle to within the bounds of this angular subtense
 * @param angle      the input to which the limit will be applied
 * @param alpha      the first angle in the subtense
 * @param beta       the second angle in the subtense
 * @param angleUnits the angle units associated with this object (Degrees or Radians)
 */
double AngularSubtense::limit(double angle,
                              double alpha,
                              double beta,
                              const AngleUnitType &angleUnits)
{
    auto deltaFromCenter = calcDeltaFromCenter(angle, alpha, beta, angleUnits);
    auto width = calcWidth(alpha, beta, angleUnits);
    if (std::fabs(deltaFromCenter > 0.5 * width))
        angle = (deltaFromCenter > 0.0) ? beta : alpha;

    return angle;
}

/**
 * Set the first angle defining the subtense
 */
void AngularSubtense::setAlpha(double alpha)
{
    m_angles.first = alpha;
}

/**
 * Set the pair of angles which define the subtense
 */
void AngularSubtense::setAngles(const std::pair<double, double> &angles)
{
    m_angles = angles;
}

/**
 * Set the angle units associated with this object (Degrees or Radians)
 */
void AngularSubtense::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
}

/**
 * Set the second angle defining the subtense
 */
void AngularSubtense::setBeta(double beta)
{
    m_angles.second = beta;
}

}

}
