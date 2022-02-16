#ifndef FRAME_STATE_H
#define FRAME_STATE_H

#include "cloneable.h"
#include "eulers.h"
#include "factory_constructible.h"
#include "initializable.h"
#include "nameable.h"
#include "output_streamable.h"
#include "time_reference_type.h"

// if a default frame state is not specified in preprocessor configuration, then define the following
#ifndef DEFAULT_FRAME_STATE
#define DEFAULT_FRAME_STATE "default"
#endif

// forward declarations
namespace math { namespace trigonometric { struct AngleUnitType; } }

namespace physics
{

namespace kinematics
{

/**
 * This class provides an abstract base class for subclasses that store and manipulate the kinematic state of a
 * coordinate reference frame
 */
class FrameState
: public attributes::interfaces::Cloneable<FrameState>,
  public attributes::abstract::FactoryConstructible<FrameState>,
  public attributes::interfaces::Initializable,
  public attributes::interfaces::Nameable,
  public attributes::concrete::OutputStreamable<FrameState>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Serializable,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<FrameState>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Writable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<FrameState>
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
     * @param name       the name of this reference frame state
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM FrameState(const std::string &name = DEFAULT_FRAME_STATE,
                           const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Copy constructor
     */
    EXPORT_STEM FrameState(const FrameState &state);

    /**
     * Move constructor
     */
    EXPORT_STEM FrameState(FrameState &&state);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~FrameState(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM FrameState &operator = (const FrameState &state);

    /**
     * Move assignment operator
     */
    EXPORT_STEM FrameState &operator = (FrameState &&state);

public:

    /**
     * Equality operator overload
     */
    EXPORT_STEM virtual bool operator == (const FrameState &state) const;

    /**
     * Equality operator overload
     */
    EXPORT_STEM virtual bool operator != (const FrameState &state) const final;

    /**
     * Output stream operator
     */
    friend EXPORT_STEM std::ostream &operator << (std::ostream &stream,
                                                  const FrameState &state);

    /**
     * Assign the values of this frame state from another frame state
     */
    EXPORT_STEM virtual bool assign(const FrameState *pState);

    /**
     * clone() function
     */
    EXPORT_STEM virtual FrameState *clone(void) const override = 0;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) = 0;

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getAcceleration(double acceleration[3]) const = 0;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d &getAcceleration(void) = 0;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
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
     * Get the name of this reference frame state
     */
    EXPORT_STEM virtual std::string getName(void) const override;

    /**
     * Get the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getOrientation(double &roll,
                                            double &pitch,
                                            double &yaw) const final;

    /**
     * Get the orientation angles of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getOrientation(double &roll,
                                            double &pitch,
                                            double &yaw,
                                            double t) const = 0;

    /**
     * Get the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getOrientation(double orientation[3]) const final;

    /**
     * Get the orientation angles of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getOrientation(double orientation[3],
                                            double t) const final;

    /**
     * Get the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers &getOrientation(void) = 0;

    /**
     * Get the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers getOrientation(void) const = 0;

    /**
     * Get the orientation angles of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual Eulers getOrientation(double t) const final;

    /**
     * Get the position of the origin of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getOrigin(double origin[3]) const final;

    /**
     * Get the position of the origin of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getOrigin(double origin[3],
                                       double t) const = 0;

    /**
     * Get the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d &getOrigin(void) = 0;

    /**
     * Get the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d getOrigin(void) const = 0;

    /**
     * Get the position of the origin of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual Vector3d getOrigin(double t) const final;

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
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getRotationalAccelerations(double &rollAccel,
                                                        double &pitchAccel,
                                                        double &yawAccel) const = 0;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getRotationalAccelerations(double rotationalAccelerations[3]) const final;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers &getRotationalAccelerations(void) = 0;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers getRotationalAccelerations(void) const = 0;

    /**
     * Get the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getRotationalRates(double &rollRate,
                                                double &pitchRate,
                                                double &yawRate) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getRotationalRates(double &rollRate,
                                                double &pitchRate,
                                                double &yawRate,
                                                double t) const = 0;

    /**
     * Get the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getRotationalRates(double rotationalRates[3]) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getRotationalRates(double rotationalRates[3],
                                                double t) const final;

    /**
     * Get the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers &getRotationalRates(void) = 0;

    /**
     * Get the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers getRotationalRates(void) const = 0;

    /**
     * Get the rotational rates of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual Eulers getRotationalRates(double t) const final;

    /**
     * Get the time at which this frame is currently defined
     */
    EXPORT_STEM virtual double getTime(void) const final;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3]) const final;

    /**
     * Get the velocity of the origin of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3],
                                         double t) const = 0;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d &getVelocity(void) = 0;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d getVelocity(void) const = 0;

    /**
     * Get the velocity of the origin of this frame with respect to its parent at the specified time
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
     * Initialization function to reset time-derivatives to zero
     */
    EXPORT_STEM virtual void initializeTimeDerivatives(void) = 0;

    /**
     * Determines whether or not two frame states are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const FrameState &state,
                                     double tol = 0.0) const;

    /**
     * Determines whether or not two frame states are spatially equivalent within the specified tolerance
     * (tests all but time)
     */
    EXPORT_STEM virtual bool isSpatiallyEqual(const FrameState &state,
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
     * Set the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setAcceleration(const Vector3d &acceleration) final;

    /**
     * Set the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration) = 0;

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) = 0;

    /**
     * Set the name of this reference frame state
     */
    EXPORT_STEM virtual void setName(const std::string &name) override;

    /**
     * Set the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrientation(const Eulers &orientation) = 0;

    /**
     * Set the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrientation(double roll,
                                            double pitch,
                                            double yaw) = 0;

    /**
     * Set the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrigin(const Vector3d &origin) final;

    /**
     * Set the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrigin(double xPosition,
                                       double yPosition,
                                       double zPosition) = 0;

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
     * Set the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalAccelerations(const Eulers &rotationalAccelerations) = 0;

    /**
     * Set the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalAccelerations(double rollAcceleration,
                                                        double pitchAcceleration,
                                                        double yawAcceleration) = 0;

    /**
     * Set the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalRates(const Eulers &rotationalRates) = 0;

    /**
     * Set the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalRates(double rollRate,
                                                double pitchRate,
                                                double yawRate) = 0;

    /**
     * Set the time at which this frame is currently defined
     */
    EXPORT_STEM virtual void setTime(double t0);

    /**
     * Set the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setVelocity(const Vector3d &velocity) final;

    /**
     * Set the velocity of the origin of this frame with respect to its parent
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
    EXPORT_STEM virtual void swap(FrameState &state) override final;

    /**
     * Update this reference frame state to the specified time. This function will update all quantities within
     * the frame state that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
     *                      (with respect to current time)
     */
    EXPORT_STEM virtual void update(double t,
                                    const TimeReferenceType &timeReference =
                                    TimeReferenceType::Absolute) = 0;
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
     * name of this reference frame state
     */
    std::string m_name;

    /**
     * time (s) at which this frame is currently defined
     */
    double m_t0;
};

}

}

#endif
