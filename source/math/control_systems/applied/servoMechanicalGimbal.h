#ifndef SERVO_MECHANICAL_GIMBAL_H
#define SERVO_MECHANICAL_GIMBAL_H

#include "angle_unit_type.h"
#include "factory_constructible.h"
#include "secondOrderSystem.h"
#include "swappable.h"
#ifdef RAPID_XML
#include "xml_constructible.h"
#endif

namespace math
{

// forward declarations
namespace geometric { class AngularSubtense; }

namespace control_systems
{

// forward declarations
namespace blocks { template<typename> class Limiter;
                   template<typename> class PhaseUnwrapper; }

namespace applied
{

/**
 * This class models a servo-mechanical gimbal system using a second-order state-space model
 */
class ServoMechanicalGimbal
: public attributes::abstract::FactoryConstructible<ServoMechanicalGimbal>,
  virtual private attributes::abstract::Reflective,
  public SecondOrderSystem,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<ServoMechanicalGimbal>,
  public attributes::concrete::XML_Constructible<ServoMechanicalGimbal, rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<ServoMechanicalGimbal>
#endif
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = trigonometric::AngleUnitType;
    using AngularSubtense = geometric::AngularSubtense;
    using Limiter = blocks::Limiter<double>;
    using PhaseUnwrapper = blocks::PhaseUnwrapper<double>;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;
    using SecondOrderSystem::swap;

    /**
     * Constructor
     * @param angleUnits the angle units associated with this object's angular quantities (Degrees or Radians)
     * @param omega      the natural frequency in radians
     * @param zeta       the damping ratio
     */
    EXPORT_STEM ServoMechanicalGimbal(const AngleUnitType &angleUnits = AngleUnitType::Degrees,
                                      double omega = 1.0,
                                      double zeta = 1.0);

    /**
     * Copy constructor
     */
    EXPORT_STEM ServoMechanicalGimbal(const ServoMechanicalGimbal &servoMechanicalGimbal);

    /**
     * Move constructor
     */
    EXPORT_STEM ServoMechanicalGimbal(ServoMechanicalGimbal &&servoMechanicalGimbal);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~ServoMechanicalGimbal(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM ServoMechanicalGimbal &operator = (const ServoMechanicalGimbal &servoMechanicalGimbal);

    /**
     * Move assignment operator
     */
    EXPORT_STEM ServoMechanicalGimbal &operator = (ServoMechanicalGimbal &&servoMechanicalGimbal);

    /**
     * Calculate the position error relative to this object's command position
     */
    EXPORT_STEM virtual double calcPositionError(void) const final;

    /**
     * Calculate the position error relative to the specified command position
     */
    EXPORT_STEM virtual double calcPositionError(double commandPosition) const final;

    /**
     * Determine if the specified command position is valid
     */
    EXPORT_STEM virtual bool commandPositionIsValid(double commandPosition) const final;

    /**
     * Create() function
     */
    static EXPORT_STEM ServoMechanicalGimbal *create(void);
#ifdef RAPID_XML
    /**
     * Function to construct an instance from an XML node
     * @param pNode a pointer to an XML node
     */
    static EXPORT_STEM ServoMechanicalGimbal *createFromXML(rapidxml::xml_node<char> *pNode);
#endif
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
     * Get this object's acceleration
     */
    EXPORT_STEM virtual double getAcceleration(void) const final;

    /**
     * Get this object's acceleration limit
     */
    EXPORT_STEM virtual double getAccelerationLimit(void) const final;

    /**
     * Get the angle units associated with this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual AngleUnitType getAngleUnits(void) const final;

    /**
     * Function to return this object's field of regard as an angular subtense
     */
    EXPORT_STEM virtual AngularSubtense getFieldOfRegard(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get this object's angular command position
     */
    EXPORT_STEM virtual double getCommandPosition(void) const;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

    /**
     * Get this object's initial angular position
     */
    EXPORT_STEM virtual double getInitialPosition(void) const final;

    /**
     * Get this object's phase un-wrapper
     */
    EXPORT_STEM virtual PhaseUnwrapper *getPhaseUnwrapper(void) const final;

    /**
     * Get this object's angular position
     */
    EXPORT_STEM virtual double getPosition(void) const;

    /**
     * Get this object's angular position limiter
     */
    EXPORT_STEM virtual Limiter *getPositionLimiter(void) const final;

    /**
     * Get this object's angular position tolerance
     */
    EXPORT_STEM virtual double getPositionTolerance(void) const;

    /**
     * Get this object's rate
     */
    EXPORT_STEM virtual double getRate(void) const final;

    /**
     * Get this object's rate limit
     */
    EXPORT_STEM virtual double getRateLimit(void) const final;

    /**
     * Get this object's angular rate tolerance
     */
    EXPORT_STEM virtual double getRateTolerance(void) const final;

    /**
     * Function to determine if the angular position has settled at the current command
     */
    EXPORT_STEM virtual bool hasSettled(void) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Test whether the angular position limit has been reached
     */
    EXPORT_STEM virtual bool limitReached(void) const final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set this object's acceleration limit
     */
    EXPORT_STEM virtual void setAccelerationLimit(double accelerationLimit) final;

    /**
     * Set the angle units associated with this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) final;

    /**
     * Set this object's angular command position; returns true upon success
     */
    EXPORT_STEM virtual bool setCommandPosition(double commandPosition) final;

    /**
     * Set this object's initial angular position; returns true upon success
     */
    EXPORT_STEM virtual bool setInitialPosition(double initialPosition) final;

    /**
     * Set this object's angular position tolerance
     */
    EXPORT_STEM virtual void setPositionTolerance(double tolerance) final;

    /**
     * Set this object's angular position; returns true upon success
     */
    EXPORT_STEM virtual bool setPosition(double position) final;

    /**
     * Set this object's rate limit
     */
    EXPORT_STEM virtual void setRateLimit(double rateLimit) final;

    /**
     * Set this object's angular rate tolerance
     */
    EXPORT_STEM virtual void setRateTolerance(double tolerance) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(ServoMechanicalGimbal &servoMechanicalGimbal) override final;

    /**
     * Update function
     */
    EXPORT_STEM virtual bool update(double time) override;

protected:

    /**
     * this object's acceleration limit
     */
    double m_accelerationLimit;

    /**
     * the current achieved angular acceleration
     */
    double m_achievedAcceleration;

    /**
     * the current achieved angular position
     */
    double m_achievedPosition;

    /**
     * the current achieved angular rate
     */
    double m_achievedRate;

    /**
     * enumeration specifies whether angles are in degrees or radians
     */
    AngleUnitType m_angleUnits;

    /**
     * the current angular commanded position
     */
    double m_commandPosition;

    /**
     * the initial angular position
     */
    double m_initialPosition;

    /**
     * the angular position tolerance
     */
    double m_positionTolerance;

    /**
     * this object's phase un-wrapper
     */
    PhaseUnwrapper *m_pPhaseUnwrapper;

    /**
     * this object's angular position limiter
     */
    Limiter *m_pPositionLimiter;

    /**
     * this object's rate limit
     */
    double m_rateLimit;

    /**
     * the angular rate tolerance
     */
    double m_rateTolerance;
};

}

}

}

#endif
