#ifndef KINEMATIC_STATE_H
#define KINEMATIC_STATE_H

#include "cloneable.h"
#include "eulers.h"
#include "factory_constructible.h"
#include "initializable.h"
#include "output_streamable.h"
#include <cmath>

// forward declarations
namespace math { namespace trigonometric { struct AngleUnitType; } }

namespace physics
{

namespace kinematics
{

/**
 * This class provides an abstract base class for subclasses that store and manipulate the kinematic state of a
 * 3-d point/space object
 */
class KinematicState
: public attributes::interfaces::Cloneable<KinematicState>,
  public attributes::abstract::FactoryConstructible<KinematicState>,
  public attributes::interfaces::Initializable,
  public attributes::concrete::OutputStreamable<KinematicState>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<KinematicState>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<KinematicState>
#endif
{
public:

    /**
     * Type alias declarations
     */
    using AngleUnitType = math::trigonometric::AngleUnitType;
    using Eulers = math::geometric::orientation::Eulers;
    using Vector3d = math::linear_algebra::vector::Vector3d;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM KinematicState(void);

    /**
     * Constructor
     * @param angleUnits the units of this object's Euler angles, degrees or Radians
     */
    EXPORT_STEM KinematicState(const AngleUnitType &angleUnits);

    /**
     * Copy constructor
     */
    EXPORT_STEM KinematicState(const KinematicState &state);

    /**
     * Move constructor
     */
    EXPORT_STEM KinematicState(KinematicState &&state);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~KinematicState(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM KinematicState &operator = (const KinematicState &state);

    /**
     * Move assignment operator
     */
    EXPORT_STEM KinematicState &operator = (KinematicState &&state);

public:

    /**
     * Equality operator
     */
    EXPORT_STEM virtual bool operator == (const KinematicState &state) const;

    /**
     * Inequality operator
     */
    EXPORT_STEM virtual bool operator != (const KinematicState &state) const final;

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const KinematicState &state);

    /**
     * Assign the values of this kinematic state from another kinematic state
     */
    EXPORT_STEM virtual bool assign(const KinematicState *pState) = 0;

    /**
     * clone() function
     */
    EXPORT_STEM virtual KinematicState *clone(void) const override = 0;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) = 0;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double acceleration[3]) const = 0;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual Vector3d &getAcceleration(void) = 0;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual Vector3d getAcceleration(void) const = 0;

    /**
     * Get angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual AngleUnitType getAngleUnits(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual void getEulerAccelerations(double &rollAccel,
                                                   double &pitchAccel,
                                                   double &yawAccel) const = 0;

    /**
     * Get the Euler accelerations (roll, pitch, yaw)
     */
    EXPORT_STEM virtual void getEulerAccelerations(double eulerAccelerations[3]) const final;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual Eulers &getEulerAccelerations(void) = 0;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual Eulers getEulerAccelerations(void) const = 0;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual void getEulerRates(double &rollRate,
                                           double &pitchRate,
                                           double &yawRate) const final;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual void getEulerRates(double &rollRate,
                                           double &pitchRate,
                                           double &yawRate,
                                           double t) const = 0;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual void getEulerRates(double eulerRates[3]) const final;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual void getEulerRates(double eulerRates[3],
                                           double t) const final;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual Eulers &getEulerRates(void) = 0;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual Eulers getEulerRates(void) const = 0;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual Eulers getEulerRates(double t) const final;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual void getEulers(double &roll,
                                       double &pitch,
                                       double &yaw) const final;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual void getEulers(double &roll,
                                       double &pitch,
                                       double &yaw,
                                       double t) const = 0;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual void getEulers(double eulers[3]) const final;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual void getEulers(double eulers[3],
                                       double t) const final;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual Eulers &getEulers(void) = 0;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual Eulers getEulers(void) const = 0;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual Eulers getEulers(double t) const final;

    /**
     * Get this object's pitch angle
     */
    EXPORT_STEM virtual double getPitch(void) const final;

    /**
     * Get this object's pitch angle
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitch(double t) const final;

    /**
     * Get this object's pitch angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitch(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's pitch angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitch(const AngleUnitType &angleUnits,
                                        double t) const = 0;

    /**
     * Get this object's pitch acceleration
     */
    EXPORT_STEM virtual double getPitchAcceleration(void) const final;

    /**
     * Get this object's pitch acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitchAcceleration(const AngleUnitType &angleUnits) const = 0;

    /**
     * Get this object's pitch rate
     */
    EXPORT_STEM virtual double getPitchRate(void) const final;

    /**
     * Get this object's pitch rate
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitchRate(double t) const final;

    /**
     * Get this object's pitch rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitchRate(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's pitch rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitchRate(const AngleUnitType &angleUnits,
                                            double t) const = 0;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual void getPosition(double position[3]) const final;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double position[3],
                                         double t) const = 0;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual Vector3d &getPosition(void) = 0;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual Vector3d getPosition(void) const = 0;

    /**
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual Vector3d getPosition(double t) const final;

    /**
     * Get this object's roll angle
     */
    EXPORT_STEM virtual double getRoll(void) const final;

    /**
     * Get this object's roll angle
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRoll(double t) const final;

    /**
     * Get this object's roll angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRoll(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's roll angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRoll(const AngleUnitType &angleUnits,
                                       double t) const = 0;

    /**
     * Get this object's roll acceleration
     */
    EXPORT_STEM virtual double getRollAcceleration(void) const final;

    /**
     * Get this object's roll acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRollAcceleration(const AngleUnitType &angleUnits) const = 0;

    /**
     * Get this object's roll rate
     */
    EXPORT_STEM virtual double getRollRate(void) const final;

    /**
     * Get this object's roll rate
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRollRate(double t) const final;

    /**
     * Get this object's roll rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRollRate(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's roll rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRollRate(const AngleUnitType &angleUnits,
                                           double t) const = 0;

    /**
     * Get the time
     */
    EXPORT_STEM virtual double getTime(void) const;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3]) const final;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3],
                                         double t) const = 0;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual Vector3d &getVelocity(void) = 0;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual Vector3d getVelocity(void) const = 0;

    /**
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual Vector3d getVelocity(double t) const final;

    /**
     * Get this object's yaw angle
     */
    EXPORT_STEM virtual double getYaw(void) const final;

    /**
     * Get this object's yaw angle
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYaw(double t) const final;

    /**
     * Get this object's yaw angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYaw(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's yaw angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYaw(const AngleUnitType &angleUnits,
                                      double t) const = 0;

    /**
     * Get this object's yaw acceleration
     */
    EXPORT_STEM virtual double getYawAcceleration(void) const final;

    /**
     * Get this object's yaw acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYawAcceleration(const AngleUnitType &angleUnits) const = 0;

    /**
     * Get this object's yaw rate
     */
    EXPORT_STEM virtual double getYawRate(void) const final;

    /**
     * Get this object's yaw rate
     * @param t the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYawRate(double t) const final;

    /**
     * Get this object's yaw rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYawRate(const AngleUnitType &angleUnits) const final;

    /**
     * Get this object's yaw rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYawRate(const AngleUnitType &angleUnits,
                                          double t) const = 0;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Determines whether or not two kinematic states are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const KinematicState &state,
                                     double tol = 0.0) const;

    /**
     * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
     * (tests all but time)
     */
    EXPORT_STEM virtual bool isSpatiallyEqual(const KinematicState &state,
                                              double tol = 0.0) const = 0;

    /**
     * Function to print the contents of this reference frame state
     */
    EXPORT_STEM virtual std::ostream &print(std::ostream &stream) const override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const override;

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(const Vector3d &acceleration) final;

    /**
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration) = 0;

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) = 0;

    /**
     * Set Euler accelerations used to propagate Euler rates
     */
    EXPORT_STEM virtual void setEulerAccelerations(const Eulers &eulerAccelerations) = 0;

    /**
     * Set Euler accelerations used to propagate Euler rates
     */
    EXPORT_STEM virtual void setEulerAccelerations(double rollAcceleration,
                                                   double pitchAcceleration,
                                                   double yawAcceleration) = 0;

    /**
     * Set Euler rates used to propagate Euler angles
     */
    EXPORT_STEM virtual void setEulerRates(const Eulers &eulerRates) = 0;

    /**
     * Set Euler rates used to propagate Euler angles
     */
    EXPORT_STEM virtual void setEulerRates(double rollRate,
                                           double pitchRate,
                                           double yawRate) = 0;

    /**
     * Set the Euler angles
     */
    EXPORT_STEM virtual void setEulers(const Eulers &eulers) = 0;

    /**
     * Set the Euler angles
     */
    EXPORT_STEM virtual void setEulers(double roll,
                                       double pitch,
                                       double yaw) = 0;

    /**
     * Set this object's pitch angle
     */
    EXPORT_STEM virtual void setPitch(double pitch) final;

    /**
     * Set this object's pitch angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitch(double pitch,
                                      const AngleUnitType &angleUnits) = 0;

    /**
     * Set this object's pitch acceleration
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration) final;

    /**
     * Set this object's pitch acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration,
                                                  const AngleUnitType &angleUnits) = 0;

    /**
     * Set this object's pitch rate
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate) final;

    /**
     * Set this object's pitch rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate,
                                          const AngleUnitType &angleUnits) = 0;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(const Vector3d &position) final;

    /**
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(double xPosition,
                                         double yPosition,
                                         double zPosition) = 0;

    /**
     * Set this object's roll angle
     */
    EXPORT_STEM virtual void setRoll(double roll) final;

    /**
     * Set this object's roll angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRoll(double roll,
                                     const AngleUnitType &angleUnits) = 0;

    /**
     * Set this object's roll acceleration
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration) final;

    /**
     * Set this object's roll acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration,
                                                 const AngleUnitType &angleUnits) = 0;

    /**
     * Set this object's roll rate
     */
    EXPORT_STEM virtual void setRollRate(double rollRate) final;

    /**
     * Set this object's roll rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRollRate(double rollRate,
                                         const AngleUnitType &angleUnits) = 0;

    /**
     * Set the time at which the initial position and velocity are defined for the state
     */
    EXPORT_STEM virtual void setTime(double t0) final;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(const Vector3d &velocity) final;

    /**
     * Set the state initial velocity
     */
    EXPORT_STEM virtual void setVelocity(double xVelocity,
                                         double yVelocity,
                                         double zVelocity) = 0;

    /**
     * Set this object's yaw angle
     */
    EXPORT_STEM virtual void setYaw(double yaw) final;

    /**
     * Set this object's yaw angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYaw(double yaw,
                                    const AngleUnitType &angleUnits) = 0;

    /**
     * Set this object's yaw acceleration
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration) final;

    /**
     * Set this object's yaw acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration,
                                                const AngleUnitType &angleUnits) = 0;

    /**
     * Set this object's yaw rate
     */
    EXPORT_STEM virtual void setYawRate(double yawRate) final;

    /**
     * Set this object's yaw rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYawRate(double yawRate,
                                        const AngleUnitType &angleUnits) = 0;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(KinematicState &state) override;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
protected:

    /**
     * specifies whether angles are in degrees or radians
     */
    AngleUnitType m_angleUnits;

    /**
     * time (s) at which state is currently defined
     */
    double m_t0;
};

}

}

#endif
