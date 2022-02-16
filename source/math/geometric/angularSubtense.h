#ifndef ANGULAR_SUBTENSE_H
#define ANGULAR_SUBTENSE_H

#include "angle_unit_type.h"
#include "export_library.h"
#include "reflective.h"

namespace math
{

namespace geometric
{

/**
 * This class represents an angular subtense, which is geometrically defined as follows: if the first angle
 * (alpha) is less than the second angle (beta), the subtense is traced in a counter-clockwise fashion moving
 * from the first angle (alpha) to the second angle (beta); otherwise, the subtense is traced by moving
 * clockwise from second angle (beta) to the first angle (alpha)
 */
class AngularSubtense
: virtual private attributes::abstract::Reflective
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;

    /**
     * Constructor
     * @param angleUnits the angle units associated with this object (Degrees or Radians)
     */
    EXPORT_STEM AngularSubtense(const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Constructor
     * @param alpha      the first angle in the subtense
     * @param beta       the second angle in the subtense
     * @param angleUnits the angle units associated with the input angles (Degrees or Radians)
     */
    EXPORT_STEM AngularSubtense(double alpha,
                                double beta,
                                const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Constructor
     * @param subtense   the pair of angles which define the subtense
     * @param angleUnits the angle units associated with the input angles (Degrees or Radians)
     */
    EXPORT_STEM AngularSubtense(const std::pair<double, double> &subtense,
                                const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Copy constructor
     */
    EXPORT_STEM AngularSubtense(const AngularSubtense &subtense);

    /**
     * Move constructor
     */
    EXPORT_STEM AngularSubtense(AngularSubtense &&subtense);

    /**
     * Destructor
     */
    EXPORT_STEM ~AngularSubtense(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM AngularSubtense &operator = (const AngularSubtense &subtense);

    /**
     * Move assignment operator
     */
    EXPORT_STEM AngularSubtense &operator = (AngularSubtense &&subtense);

    /**
     * Determine if the input angle is within the angular subtense
     */
    EXPORT_STEM virtual bool angleWithinSubtense(double angle) const final;

    /**
     * Determine if the input angle is within the angular subtense defined by angles alpha and beta; see class
     * description for explanation regarding how alpha and beta are used to define the angular subtense
     * @param angle      the angle for which the test will be performed
     * @param alpha      the first angle in the subtense
     * @param beta       the second angle in the subtense
     * @param angleUnits the angle units associated with this object (Degrees or Radians)
     */
    static EXPORT_STEM bool angleWithinSubtense(double angle,
                                                double alpha,
                                                double beta,
                                                const AngleUnitType &angleUnits);

    /**
     * Calculate the center of the subtense
     */
    EXPORT_STEM virtual double calcCenter(void) const final;

    /**
     * Calculate the center of the subtense defined by angles alpha and beta; see class description for
     * explanation regarding how alpha and beta are used to define the angular subtense
     * @param alpha      the first angle in the subtense
     * @param beta       the second angle in the subtense
     * @param angleUnits the angle units associated with this object (Degrees or Radians)
     */
    static EXPORT_STEM double calcCenter(double alpha,
                                         double beta,
                                         const AngleUnitType &angleUnits);

    /**
     * Calculate the delta of an angle relative to the center of the subtense
     */
    EXPORT_STEM virtual double calcDeltaFromCenter(double angle) const final;

    /**
     * Calculate the delta of an angle relative to the center of the subtense defined by angles alpha and beta;
     * see class description for explanation regarding how alpha and beta are used to define the angular
     * subtense
     * @param angle      the angle for which the delta will be calculate
     * @param alpha      the first angle in the subtense
     * @param beta       the second angle in the subtense
     * @param angleUnits the angle units associated with this object (Degrees or Radians)
     */
    static EXPORT_STEM double calcDeltaFromCenter(double angle,
                                                  double alpha,
                                                  double beta,
                                                  const AngleUnitType &angleUnits);

    /**
     * Calculate the with of the angular subtense
     */
    EXPORT_STEM virtual double calcWidth(void) const final;

    /**
     * Calculate the with of the angular subtense defined by angles alpha and beta; see class description for
     * explanation regarding how alpha and beta are used to define the angular subtense
     * @param alpha      the first angle in the subtense
     * @param beta       the second angle in the subtense
     * @param angleUnits the angle units associated with this object (Degrees or Radians)
     */
    static EXPORT_STEM double calcWidth(double alpha,
                                        double beta,
                                        const AngleUnitType &angleUnits);

    /**
     * Get the pair of angles which define the subtense
     */
    EXPORT_STEM virtual std::pair<double, double> &getAngles(void) final;

    /**
     * Get the pair of angles which define the subtense
     */
    EXPORT_STEM virtual std::pair<double, double> getAngles(void) const final;

    /**
     * Get the angle units associated with this object (Degrees or Radians)
     */
    EXPORT_STEM virtual AngleUnitType getAngleUnits(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Return whether or not this object's subtense is defined in a clockwise fashion
     */
    EXPORT_STEM virtual bool isClockwise(void) const final;

    /**
     * Return whether or not this object's subtense is defined in a clockwise fashion; see class description
     * for explanation regarding how alpha and beta are used to define the angular subtense
     * @param alpha the first angle in the subtense
     * @param beta  the second angle in the subtense
     */
    static EXPORT_STEM bool isClockwise(double alpha,
                                        double beta);

    /**
     * Return whether or not this object's subtense is defined in a clockwise fashion
     */
    EXPORT_STEM virtual bool isCounterClockwise(void) const final;

    /**
     * Return whether or not this object's subtense is defined in a clockwise fashion; see class description
     * for explanation regarding how alpha and beta are used to define the angular subtense
     * @param alpha the first angle in the subtense
     * @param beta  the second angle in the subtense
     */
    static EXPORT_STEM bool isCounterClockwise(double alpha,
                                               double beta);

    /**
     * Function to limit the input angle to within the bounds of this angular subtense
     */
    EXPORT_STEM virtual double limit(double angle) const final;

    /**
     * Function to limit the input angle to within the bounds of this angular subtense
     * @param angle      the input to which the limit will be applied
     * @param alpha      the first angle in the subtense
     * @param beta       the second angle in the subtense
     * @param angleUnits the angle units associated with this object (Degrees or Radians)
     */
    static EXPORT_STEM double limit(double angle,
                                    double alpha,
                                    double beta,
                                    const AngleUnitType &angleUnits);

    /**
     * Set the first angle defining the subtense
     */
    EXPORT_STEM virtual void setAlpha(double alpha) final;

    /**
     * Set the pair of angles which define the subtense
     */
    EXPORT_STEM virtual void setAngles(const std::pair<double, double> &angles) final;

    /**
     * Set the angle units associated with this object (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) final;

    /**
     * Set the second angle defining the subtense
     */
    EXPORT_STEM virtual void setBeta(double beta) final;

private:

    /**
     * the pair of angles which define the subtense
     */
    std::pair<double, double> m_angles;

    /**
     * enumeration specifies whether angles are in degrees or radians
     */
    AngleUnitType m_angleUnits;
};

}

}

#endif
