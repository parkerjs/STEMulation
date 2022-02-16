#ifndef BANG_BANG_SERVO_MECHANICAL_GIMBAL_H
#define BANG_BANG_SERVO_MECHANICAL_GIMBAL_H

#include "servoMechanicalGimbal.h"

namespace math
{

namespace control_systems
{

namespace applied
{

/**
 * This class models a bang-bang servo-mechanical gimbal system using a second-order state-space model
 */
class BangBangServoMechanicalGimbal
: virtual private attributes::abstract::Reflective,
  public ServoMechanicalGimbal,
  public attributes::interfaces::Swappable<BangBangServoMechanicalGimbal>
{
public:

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;
    using ServoMechanicalGimbal::swap;

    /**
     * Constructor
     * @param angleUnits the angle units associated with this object's angular quantities (Degrees or Radians)
     * @param omega      the natural frequency in radians
     * @param zeta       the damping ratio
     */
    EXPORT_STEM BangBangServoMechanicalGimbal(const AngleUnitType &angleUnits = AngleUnitType::Degrees,
                                              double omega = 1.0,
                                              double zeta = 1.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM BangBangServoMechanicalGimbal(const BangBangServoMechanicalGimbal &servoMechanicalGimbal);

    /**
     * Move constructor
     */
    EXPORT_STEM BangBangServoMechanicalGimbal(BangBangServoMechanicalGimbal &&servoMechanicalGimbal);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~BangBangServoMechanicalGimbal(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM BangBangServoMechanicalGimbal &
    operator = (const BangBangServoMechanicalGimbal &servoMechanicalGimbal);

    /**
     * Move assignment operator
     */
    EXPORT_STEM BangBangServoMechanicalGimbal &
    operator = (BangBangServoMechanicalGimbal &&servoMechanicalGimbal);

    /**
     * Create() function
     */
    static EXPORT_STEM BangBangServoMechanicalGimbal *create(void);

    /**
     * Function to evaluate the dynamics model which represents this control system
     * @param t  time at which the dynamics model is evaluated
     * @param x  the state vector
     * @param xd the state derivative vector
     */
    EXPORT_STEM virtual void dynamicsModel(double t,
                                           const StateVector &x,
                                           StateVector &xd) override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get the angular threshold between bang-bang and linear operation
     */
    EXPORT_STEM virtual double getThreshold(void) const final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set the angular threshold between bang-bang and linear operation
     */
    EXPORT_STEM virtual void setThreshold(double threshold) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(BangBangServoMechanicalGimbal &servoMechanicalGimbal) override final;

protected:

    /**
     * angular threshold between bang-bang and linear operation
     */
    double m_threshold;
};

}

}

}

#endif
