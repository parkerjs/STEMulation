#ifndef INTERPOLATED_FRAME_STATE_H
#define INTERPOLATED_FRAME_STATE_H

#include "frameState.h"

namespace physics
{

namespace kinematics
{

// forward declarations
class ProjectedFrameState;

/**
 * This class stores the kinematic state of a coordinate reference frame and estimates the state at another
 * time by interpolating the time history
 */
class InterpolatedFrameState
: public FrameState,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<InterpolatedFrameState>
{
public:

    /**
     * Using declarations
     */
    using FrameState::getOrientation;
    using FrameState::getRotationalRates;
    using FrameState::setAcceleration;
    using FrameState::setOrigin;
    using FrameState::setVelocity;
    using FrameState::swap;

private:

    /**
     * Constructor
     * @param name       the name of this reference frame state
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM InterpolatedFrameState(const std::string &name = DEFAULT_FRAME_STATE,
                                       const AngleUnitType &angleUnits = AngleUnitType::Degrees);

    /**
     * Copy constructor
     */
    EXPORT_STEM InterpolatedFrameState(const InterpolatedFrameState &state);

    /**
     * Move constructor
     */
    EXPORT_STEM InterpolatedFrameState(InterpolatedFrameState &&state);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~InterpolatedFrameState(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM InterpolatedFrameState &operator = (const InterpolatedFrameState &state);

    /**
     * Move assignment operator
     */
    EXPORT_STEM InterpolatedFrameState &operator = (InterpolatedFrameState &&state);

public:

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (const FrameState &state) const override;

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (const InterpolatedFrameState &state) const;

    /**
     * Assign the values of this frame state from another frame state
     */
    EXPORT_STEM virtual bool assign(const FrameState *pState) override;

    /**
     * Assign the values of this frame state from another frame state
     */
    EXPORT_STEM virtual bool assign(const InterpolatedFrameState *pState) final;

    /**
     * Assign the values of this frame state from another frame state
     */
    EXPORT_STEM virtual bool assign(const ProjectedFrameState *pState) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual InterpolatedFrameState *clone(void) const override;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) override;

    /**
     * create() function
     * @param name the name of this reference frame state
     */
    static EXPORT_STEM InterpolatedFrameState *create(const std::string &name = DEFAULT_FRAME_STATE);

    /**
     * create() function
     * @param name       the name of this reference frame state
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    static EXPORT_STEM InterpolatedFrameState *create(const std::string &name,
                                                      const AngleUnitType &angleUnits);

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getAcceleration(double acceleration[3]) const override;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d &getAcceleration(void) override;

    /**
     * Get the acceleration of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d getAcceleration(void) const override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get the orientation angles of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getOrientation(double &roll,
                                            double &pitch,
                                            double &yaw,
                                            double t) const override;

    /**
     * Get the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers &getOrientation(void) override;

    /**
     * Get the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers getOrientation(void) const override;

    /**
     * Get the position of the origin of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getOrigin(double origin[3],
                                       double t) const override;

    /**
     * Get the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d &getOrigin(void) override;

    /**
     * Get the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d getOrigin(void) const override;

    /**
     * Get this object's pitch angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitch(const AngleUnitType &angleUnits,
                                        double t) const override;

    /**
     * Get this object's pitch acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getPitchAcceleration(const AngleUnitType &angleUnits) const override;

    /**
     * Get this object's pitch rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getPitchRate(const AngleUnitType &angleUnits,
                                            double t) const override;

    /**
     * Get this object's roll angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRoll(const AngleUnitType &angleUnits,
                                       double t) const override;

    /**
     * Get this object's roll acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getRollAcceleration(const AngleUnitType &angleUnits) const override;

    /**
     * Get this object's roll rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getRollRate(const AngleUnitType &angleUnits,
                                           double t) const override;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void getRotationalAccelerations(double &rollAccel,
                                                        double &pitchAccel,
                                                        double &yawAccel) const override;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers &getRotationalAccelerations(void) override;

    /**
     * Get the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers getRotationalAccelerations(void) const override;

    /**
     * Get the rotational rates of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getRotationalRates(double &rollRate,
                                                double &pitchRate,
                                                double &yawRate,
                                                double t) const override;

    /**
     * Get the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers &getRotationalRates(void) override;

    /**
     * Get the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual Eulers getRotationalRates(void) const override;

    /**
     * Get the velocity of the origin of this frame with respect to its parent at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3],
                                         double t) const override;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d &getVelocity(void) override;

    /**
     * Get the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual Vector3d getVelocity(void) const override;

    /**
     * Get this object's yaw angle
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYaw(const AngleUnitType &angleUnits,
                                      double t) const override;

    /**
     * Get this object's yaw acceleration
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     */
    EXPORT_STEM virtual double getYawAcceleration(const AngleUnitType &angleUnits) const override;

    /**
     * Get this object's yaw rate
     * @param angleUnits the desired angle units of the return value, Degrees or Radians
     * @param t          the time at which the return value will be computed
     */
    EXPORT_STEM virtual double getYawRate(const AngleUnitType &angleUnits,
                                          double t) const override;

private:

    /**
     * Function to perform map interpolation
     */
    template<typename Result, typename Vector>
    static bool interpolate(double t,
                            Result &&result,
                            const std::map<double, Vector> &map)
    {
        bool bSuccess = (!map.empty());
        if (bSuccess)
        {
            // if beyond the limits, return the first/last element
            if (t < map.cbegin()->first)
                map.cbegin()->second.get(result);
            else if (t > map.crbegin()->first)
                map.crbegin()->second.get(result);
            else
            {
                auto &&itLower = map.lower_bound(t);
                if (itLower == map.cbegin())
                    --itLower;

                auto &&itUpper = map.upper_bound(t);
                for (std::size_t i = 0; i < 3; ++i)
                {
                    result[i] = itLower->second[i] + (itUpper->second[i] - itLower->second[i]) * t -
                                itLower->first / (itUpper->first - itLower->first);
                }
            }
        }

        return bSuccess;
    }

public:

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;

    /**
     * Initialization function to reset time-derivatives to zero
     */
    EXPORT_STEM virtual void initializeTimeDerivatives(void) override;

    /**
     * Determines whether or not two frame states are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const FrameState &state,
                                     double tol = 0.0) const override;

    /**
     * Determines whether or not two frame states are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const InterpolatedFrameState &state,
                                     double tol = 0.0) const final;

    /**
     * Determines whether or not two frame states are spatially equivalent within the specified tolerance
     * (tests all but time)
     */
    EXPORT_STEM virtual bool isSpatiallyEqual(const FrameState &state,
                                              double tol = 0.0) const override;

    /**
     * Determines whether or not two frame states are spatially equivalent within the specified tolerance
     * (tests all but time)
     */
    EXPORT_STEM virtual bool isSpatiallyEqual(const InterpolatedFrameState &state,
                                              double tol = 0.0) const final;

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
    EXPORT_STEM virtual void setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration) override;

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) override;

    /**
     * Set the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrientation(const Eulers &orientation) override;

    /**
     * Set the orientation angles of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrientation(double roll,
                                            double pitch,
                                            double yaw) override;

    /**
     * Set the position of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setOrigin(double xPosition,
                                       double yPosition,
                                       double zPosition) override;

    /**
     * Set this object's pitch angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitch(double pitch,
                                      const AngleUnitType &angleUnits) override;

    /**
     * Set this object's pitch acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitchAcceleration(double pitchAcceleration,
                                                  const AngleUnitType &angleUnits) override;

    /**
     * Set this object's pitch rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setPitchRate(double pitchRate,
                                          const AngleUnitType &angleUnits) override;

    /**
     * Set this object's roll angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRoll(double roll,
                                     const AngleUnitType &angleUnits) override;

    /**
     * Set this object's roll acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRollAcceleration(double rollAcceleration,
                                                 const AngleUnitType &angleUnits) override;

    /**
     * Set this object's roll rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setRollRate(double rollRate,
                                         const AngleUnitType &angleUnits) override;

    /**
     * Set the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalAccelerations(const Eulers &rotationalAccelerations) override;

    /**
     * Set the rotational accelerations of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalAccelerations(double rollAcceleration,
                                                        double pitchAcceleration,
                                                        double yawAcceleration) override;

    /**
     * Set the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalRates(const Eulers &rotationalRates) override;

    /**
     * Set the rotational rates of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setRotationalRates(double rollRate,
                                                double pitchRate,
                                                double yawRate) override;

    /**
     * Set the time at which this frame is currently defined
     */
    EXPORT_STEM virtual void setTime(double t0) override;

    /**
     * Set the velocity of the origin of this frame with respect to its parent
     */
    EXPORT_STEM virtual void setVelocity(double xVelocity,
                                         double yVelocity,
                                         double zVelocity) override;

    /**
     * Set this object's yaw angle
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYaw(double yaw,
                                    const AngleUnitType &angleUnits) override;

    /**
     * Set this object's yaw acceleration
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYawAcceleration(double yawAcceleration,
                                                const AngleUnitType &angleUnits) override;

    /**
     * Set this object's yaw rate
     * @param angleUnits the angle units of the input argument, Degrees or Radians
     */
    EXPORT_STEM virtual void setYawRate(double yawRate,
                                        const AngleUnitType &angleUnits) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(InterpolatedFrameState &state) override final;

    /**
     * Update this reference frame state to the specified time. This function will update all quantities within
     * the frame state that vary with time, including the time tag.
     * @param t             the time value (s)
     * @param timeReference enumeration specifying the update time is an absolute time or a delta time (with
     *                      respect to current time)
     */
    EXPORT_STEM virtual void update(double t,
                                    const TimeReferenceType &timeReference =
                                    TimeReferenceType::Absolute) override;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * acceleration of the origin of this frame with respect to its parent
     */
    Vector3d m_acceleration;

    /**
     * map of acceleration of the origin of this frame with respect to its parent vs time
     */
    std::map<double, Vector3d> m_accelerationMap;

    /**
     * orientation angles of this frame with respect to its parent
     */
    Eulers m_orientation;

    /**
     * map of orientation angles of this frame with respect to its parent vs time
     */
    std::map<double, Eulers> m_orientationMap;

    /**
     * position of the origin of this frame with respect to its parent
     */
    Vector3d m_origin;

    /**
     * map of position of the origin of this frame with respect to its parent vs time
     */
    std::map<double, Vector3d> m_originMap;

    /**
     * rotational accelerations of this frame with respect to its parent
     */
    Eulers m_rotationalAccelerations;

    /**
     * map of rotational accelerations of this frame with respect to its parent vs time
     */
    std::map<double, Eulers> m_rotationalAccelerationsMap;

    /**
     * rotational rates of this frame with respect to its parent
     */
    Eulers m_rotationalRates;

    /**
     * map of rotational rates of this frame with respect to its parent vs time
     */
    std::map<double, Eulers> m_rotationalRatesMap;

    /**
     * velocity of the origin of this frame with respect to its parent
     */
    Vector3d m_velocity;

    /**
     * map of velocity of the origin of this frame with respect to its parent vs time
     */
    std::map<double, Vector3d> m_velocityMap;
};

}

}

#endif
