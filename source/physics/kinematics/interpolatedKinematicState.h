#ifndef INTERPOLATED_KINEMATIC_STATE_H
#define INTERPOLATED_KINEMATIC_STATE_H

#include "kinematicState.h"

namespace physics
{

namespace kinematics
{

/**
 * This class stores the kinematic state of a 3-d point/space object as a time history and estimates the state
 * at another time by interpolating the time history
 */
class InterpolatedKinematicState
: public KinematicState,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<InterpolatedKinematicState>
{
public:

    /**
     * Using declarations
     */
    using KinematicState::getEulerRates;
    using KinematicState::getEulers;
    using KinematicState::swap;

private:

    /**
     * Constructor
     */
    EXPORT_STEM InterpolatedKinematicState(void);

    /**
     * Constructor
     * @param angleUnits the units of this object's Euler angles, degrees or Radians
     */
    EXPORT_STEM InterpolatedKinematicState(const AngleUnitType &angleUnits);

    /**
     * Copy constructor
     */
    EXPORT_STEM InterpolatedKinematicState(const InterpolatedKinematicState &state);

    /**
     * Move constructor
     */
    EXPORT_STEM InterpolatedKinematicState(InterpolatedKinematicState &&state);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~InterpolatedKinematicState(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM InterpolatedKinematicState &operator = (const InterpolatedKinematicState &state);

    /**
     * Move assignment operator
     */
    EXPORT_STEM InterpolatedKinematicState &operator = (InterpolatedKinematicState &&state);

public:

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (const KinematicState &state) const override;

    /**
     * Equality operator
     */
    EXPORT_STEM bool operator == (const InterpolatedKinematicState &state) const;

    /**
     * Assign the values of this kinematic state from another kinematic state
     */
    EXPORT_STEM virtual bool assign(const KinematicState *pState) override;

    /**
     * Assign the values of this kinematic state from another kinematic state
     */
    EXPORT_STEM virtual bool assign(const InterpolatedKinematicState *pState) final;

    /**
     * clone() function
     */
    EXPORT_STEM virtual InterpolatedKinematicState *clone(void) const override;

    /**
     * Convert the units of this object's angular quantities (Degrees or Radians)
     */
    EXPORT_STEM virtual void convertAngleUnits(const AngleUnitType &angleUnits) override;

    /**
     * create() function
     * @param angleUnits the units of this object's Euler angles, degrees or Radians
     */
    static EXPORT_STEM InterpolatedKinematicState *create(const AngleUnitType &angleUnits =
                                                          AngleUnitType::Degrees);

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) override;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual void getAcceleration(double acceleration[3]) const override;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual Vector3d &getAcceleration(void) override;

    /**
     * Get the state acceleration
     */
    EXPORT_STEM virtual Vector3d getAcceleration(void) const override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual void getEulerAccelerations(double &rollAccel,
                                                   double &pitchAccel,
                                                   double &yawAccel) const override;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual Eulers &getEulerAccelerations(void) override;

    /**
     * Get the Euler accelerations
     */
    EXPORT_STEM virtual Eulers getEulerAccelerations(void) const override;

    /**
     * Get the Euler rates at the specified time
     */
    EXPORT_STEM virtual void getEulerRates(double &rollRate,
                                           double &pitchRate,
                                           double &yawRate,
                                           double t) const override;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual Eulers &getEulerRates(void) override;

    /**
     * Get the Euler rates
     */
    EXPORT_STEM virtual Eulers getEulerRates(void) const override;

    /**
     * Get the Euler angles at the specified time
     */
    EXPORT_STEM virtual void getEulers(double &roll,
                                       double &pitch,
                                       double &yaw,
                                       double t) const override;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual Eulers &getEulers(void) override;

    /**
     * Get the Euler angles
     */
    EXPORT_STEM virtual Eulers getEulers(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

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
     * Get the state position at the specified time
     */
    EXPORT_STEM virtual void getPosition(double position[3],
                                         double t) const override;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual Vector3d &getPosition(void) override;

    /**
     * Get the state position
     */
    EXPORT_STEM virtual Vector3d getPosition(void) const override;

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
     * Get the state velocity at the specified time
     */
    EXPORT_STEM virtual void getVelocity(double velocity[3],
                                         double t) const override;

    /**
     * Get the state velocity
     */
    EXPORT_STEM virtual Vector3d &getVelocity(void) override;

    /**
     * Get the state velocity
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
     * Determines whether or not two kinematic states are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const KinematicState &state,
                                     double tol = 0.0) const override;

    /**
     * Determines whether or not two kinematic states are equivalent within the specified tolerance
     */
    EXPORT_STEM virtual bool isEqual(const InterpolatedKinematicState &state,
                                     double tol = 0.0) const final;

    /**
     * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
     * (tests all but time)
     */
    EXPORT_STEM virtual bool isSpatiallyEqual(const KinematicState &state,
                                              double tol = 0.0) const override;

    /**
     * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
     * (tests all but time)
     */
    EXPORT_STEM virtual bool isSpatiallyEqual(const InterpolatedKinematicState &state,
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
     * Set the state acceleration
     */
    EXPORT_STEM virtual void setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration) override;

    /**
     * Set angle units (Degrees or Radians)
     */
    EXPORT_STEM virtual void setAngleUnits(const AngleUnitType &angleUnits) override;

    /**
     * Set Euler accelerations used to propagate Euler rates
     */
    EXPORT_STEM virtual void setEulerAccelerations(const Eulers &eulerAccelerations) override;

    /**
     * Set Euler accelerations used to propagate Euler rates
     */
    EXPORT_STEM virtual void setEulerAccelerations(double rollAcceleration,
                                                   double pitchAcceleration,
                                                   double yawAcceleration) override;

    /**
     * Set Euler rates used to propagate Euler angles
     */
    EXPORT_STEM virtual void setEulerRates(const Eulers &eulerRates) override;

    /**
     * Set Euler rates used to propagate Euler angles
     */
    EXPORT_STEM virtual void setEulerRates(double rollRate,
                                           double pitchRate,
                                           double yawRate) override;

    /**
     * Set the Euler angles
     */
    EXPORT_STEM virtual void setEulers(const Eulers &eulers) override;

    /**
     * Set the Euler angles
     */
    EXPORT_STEM virtual void setEulers(double roll,
                                       double pitch,
                                       double yaw) override;

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
     * Set the state initial position
     */
    EXPORT_STEM virtual void setPosition(double xPosition,
                                         double yPosition,
                                         double zPosition) override;

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
     * Set the state initial velocity
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
    EXPORT_STEM virtual void swap(InterpolatedKinematicState &state) override;
#ifdef RAPID_XML
    /**
     * Function to write data to XML
     */
    EXPORT_STEM virtual bool writeToXML(rapidxml::xml_node<char> *pNode) override;
#endif
private:

    /**
     * this state's interpolated acceleration
     */
    Vector3d m_acceleration;

    /**
     * this state's map of accelerations vs time
     */
    std::map<double, Vector3d> m_accelerationMap;

    /**
     * this state's interpolated Euler angle accelerations (degrees/sec/sec [default] or radians/sec/sec)
     */
    Eulers m_eulerAccelerations;

    /**
     * this state's map of Euler accelerations vs time
     */
    std::map<double, Eulers> m_eulerAccelerationsMap;

    /**
     * this state's interpolated Euler angle rates (degrees/sec [default] or radians/sec)
     */
    Eulers m_eulerRates;

    /**
     * this state's map of Euler rates vs time
     */
    std::map<double, Eulers> m_eulerRatesMap;

    /**
     * this state's interpolated Euler angles (degrees [default] or radians)
     */
    Eulers m_eulers;

    /**
     * this state's map of Euler angles vs time
     */
    std::map<double, Eulers> m_eulersMap;

    /**
     * this state's interpolated position
     */
    Vector3d m_position;

    /**
     * this state's map of position vs time
     */
    std::map<double, Vector3d> m_positionMap;

    /**
     * this state's interpolated velocity
     */
    Vector3d m_velocity;

    /**
     * this state's map of velocity vs time
     */
    std::map<double, Vector3d> m_velocityMap;
};

}

}

#endif
