#include "angle_unit_type.h"
#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "projectedKinematicState.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include <cstring>
#include <string>
#include <typeinfo>

// file-scoped variables
static constexpr char factoryName[] = "Projected";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::geometric::orientation;
using namespace math::linear_algebra::vector;
using namespace math::trigonometric;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace physics
{

namespace kinematics
{

// register factories...
static FactoryRegistrar<KinematicState>
factory(factoryName, &ProjectedKinematicState::create);

/**
 * Constructor
 */
ProjectedKinematicState::ProjectedKinematicState(void)
: ProjectedKinematicState(AngleUnitType::Degrees)
{

}

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, degrees or Radians
 */
ProjectedKinematicState::ProjectedKinematicState(const AngleUnitType &angleUnits)
{
    setAngleUnits(angleUnits);
}

/**
 * Copy constructor
 */
ProjectedKinematicState::ProjectedKinematicState(const ProjectedKinematicState &state)
{
    operator = (state);
}

/**
 * Move constructor
 */
ProjectedKinematicState::ProjectedKinematicState(ProjectedKinematicState &&state)
{
    operator = (std::move(state));
}

/**
 * Destructor
 */
ProjectedKinematicState::~ProjectedKinematicState(void)
{

}

/**
 * Copy assignment operator
 */
ProjectedKinematicState &ProjectedKinematicState::operator = (const ProjectedKinematicState &state)
{
    if (&state != this)
    {
        KinematicState::operator = (state);

        m_acceleration = state.m_acceleration;
        m_eulerAccelerations = state.m_eulerAccelerations;
        m_eulerRates = state.m_eulerRates;
        m_eulers = state.m_eulers;
        m_position = state.m_position;
        m_t0 = state.m_t0;
        m_velocity = state.m_velocity;
    }

    return *this;
}

/**
 * Move assignment operator
 */
ProjectedKinematicState &ProjectedKinematicState::operator = (ProjectedKinematicState &&state)
{
    if (&state != this)
    {
        state.swap(*this);
    }

    return *this;
}

/**
 * Equality operator
 */
bool ProjectedKinematicState::operator == (const KinematicState &state) const
{
    return operator == (dynamic_cast<const ProjectedKinematicState &>(state));
}

/**
 * Equality operator
 */
bool ProjectedKinematicState::operator == (const ProjectedKinematicState &state) const
{
    bool bEqual = KinematicState::operator == (state);
    if (bEqual)
    {
        bEqual = (m_acceleration == state.m_acceleration &&
                  m_eulerAccelerations == state.m_eulerAccelerations &&
                  m_eulerRates == state.m_eulerRates &&
                  m_eulers == state.m_eulers &&
                  m_position == state.m_position &&
                  m_t0 == state.m_t0 &&
                  m_velocity == state.m_velocity);
    }

    return bEqual;
}

/**
 * Assign the values of this kinematic state from another kinematic state
 */
bool ProjectedKinematicState::assign(const KinematicState *pState)
{
    return assign(dynamic_cast<const ProjectedKinematicState *>(pState));
}

/**
 * Assign the values of this kinematic state from another kinematic state
 */
bool ProjectedKinematicState::assign(const ProjectedKinematicState *pState)
{
    bool bSuccess = (pState != nullptr);
    if (bSuccess)
    {
        operator = (*pState);
    }

    return bSuccess;
}

/**
 * clone() function
 */
ProjectedKinematicState *ProjectedKinematicState::clone(void) const
{
    return new ProjectedKinematicState(*this);
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void ProjectedKinematicState::convertAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
    m_eulerAccelerations.convertAngleUnits(m_angleUnits);
    m_eulerRates.convertAngleUnits(m_angleUnits);
    m_eulers.convertAngleUnits(m_angleUnits);
}

/**
 * create() function
 * @param angleUnits the units of this object's Euler angles, degrees or Radians
 */
ProjectedKinematicState *ProjectedKinematicState::create(const AngleUnitType &angleUnits)
{
    return new ProjectedKinematicState(angleUnits);
}

/**
 * Function to deserialize this object's data
 */
std::istream &ProjectedKinematicState::deserialize(std::istream &stream)
{
    KinematicState::deserialize(stream);

    if (stream)
    {
        m_acceleration.deserialize(stream);
        m_eulerAccelerations.deserialize(stream);
        m_eulerRates.deserialize(stream);
        m_eulers.deserialize(stream);
        m_position.deserialize(stream);
        m_velocity.deserialize(stream);
    }

    return stream;
}

/**
 * Get the state acceleration
 */
void ProjectedKinematicState::getAcceleration(double acceleration[3]) const
{
    acceleration[0] = m_acceleration[0];
    acceleration[1] = m_acceleration[1];
    acceleration[2] = m_acceleration[2];
}

/**
 * Get the state acceleration
 */
Vector3d &ProjectedKinematicState::getAcceleration(void)
{
    return m_acceleration;
}

/**
 * Get the state acceleration
 */
Vector3d ProjectedKinematicState::getAcceleration(void) const
{
    return m_acceleration;
}

/**
 * Get the name of this class
 */
std::string ProjectedKinematicState::getClassName(void) const
{
    return "ProjectedKinematicState";
}

/**
 * Get the Euler accelerations
 */
void ProjectedKinematicState::getEulerAccelerations(double &rollAccel,
                                                    double &pitchAccel,
                                                    double &yawAccel) const
{
    pitchAccel = m_eulerAccelerations.getPitch();
    rollAccel = m_eulerAccelerations.getRoll();
    yawAccel = m_eulerAccelerations.getYaw();
}

/**
 * Get the Euler accelerations
 */
Eulers &ProjectedKinematicState::getEulerAccelerations(void)
{
    return m_eulerAccelerations;
}

/**
 * Get the Euler accelerations
 */
Eulers ProjectedKinematicState::getEulerAccelerations(void) const
{
    return m_eulerAccelerations;
}

/**
 * Get the Euler rates at the specified time
 */
void ProjectedKinematicState::getEulerRates(double &rollRate,
                                            double &pitchRate,
                                            double &yawRate,
                                            double t) const
{
    pitchRate = m_eulerRates.getPitch();
    rollRate = m_eulerRates.getRoll();
    yawRate = m_eulerRates.getYaw();

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        pitchRate += m_eulerAccelerations.getPitch() * dt;
        rollRate += m_eulerAccelerations.getRoll() * dt;
        yawRate += m_eulerAccelerations.getYaw() * dt;
    }
}

/**
 * Get the Euler rates
 */
Eulers &ProjectedKinematicState::getEulerRates(void)
{
    return m_eulerRates;
}

/**
 * Get the Euler rates
 */
Eulers ProjectedKinematicState::getEulerRates(void) const
{
    return m_eulerRates;
}

/**
 * Get the Euler angles at the specified time
 */
void ProjectedKinematicState::getEulers(double &roll,
                                        double &pitch,
                                        double &yaw,
                                        double t) const
{
    pitch = m_eulers.getPitch();
    roll = m_eulers.getRoll();
    yaw = m_eulers.getYaw();

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        pitch += (m_eulerRates.getPitch() + 0.5 * m_eulerAccelerations.getPitch() * dt) * dt;
        roll += (m_eulerRates.getRoll() + 0.5 * m_eulerAccelerations.getRoll() * dt) * dt;
        yaw += (m_eulerRates.getYaw() + 0.5 * m_eulerAccelerations.getYaw() * dt) * dt;
    }
}

/**
 * Get the Euler angles
 */
Eulers &ProjectedKinematicState::getEulers(void)
{
    return m_eulers;
}

/**
 * Get the Euler angles
 */
Eulers ProjectedKinematicState::getEulers(void) const
{
    return m_eulers;
}

/**
 * Get the factory name of this constructible
 */
std::string ProjectedKinematicState::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double ProjectedKinematicState::getPitch(const AngleUnitType &angleUnits,
                                         double t) const
{
    auto pitch = m_eulers.getPitch(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto pitchAcc = m_eulerAccelerations.getPitch(angleUnits);
        auto pitchRate = m_eulerRates.getPitch(angleUnits);
        if (pitchAcc != 0.0 || pitchRate != 0.0)
            pitch += (pitchRate + 0.5 * pitchAcc * dt) * dt;
    }

    return pitch;
}

/**
 * Get this object's pitch acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double ProjectedKinematicState::getPitchAcceleration(const AngleUnitType &angleUnits) const
{
    return m_eulerAccelerations.getPitch(angleUnits);
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double ProjectedKinematicState::getPitchRate(const AngleUnitType &angleUnits,
                                             double t) const
{
    auto pitchRate = m_eulerRates.getPitch(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto pitchAcc = m_eulerAccelerations.getPitch(angleUnits);
        if (pitchAcc != 0.0)
            pitchRate += pitchAcc * dt;
    }

    return pitchRate;
}

/**
 * Get the state position at the specified time
 */
void ProjectedKinematicState::getPosition(double position[3],
                                          double t) const
{
    m_position.get(position);

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        position[0] += (m_velocity[0] + 0.5 * m_acceleration[0] * dt) * dt;
        position[1] += (m_velocity[1] + 0.5 * m_acceleration[1] * dt) * dt;
        position[2] += (m_velocity[2] + 0.5 * m_acceleration[2] * dt) * dt;
    }
}

/**
 * Get the state position
 */
Vector3d &ProjectedKinematicState::getPosition(void)
{
    return m_position;
}

/**
 * Get the state position
 */
Vector3d ProjectedKinematicState::getPosition(void) const
{
    return m_position;
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double ProjectedKinematicState::getRoll(const AngleUnitType &angleUnits,
                                        double t) const
{
    auto roll = m_eulers.getRoll(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto rollAcc = m_eulerAccelerations.getRoll(angleUnits);
        auto rollRate = m_eulerRates.getRoll(angleUnits);
        if (rollAcc != 0.0 || rollRate != 0.0)
            roll += (rollRate + 0.5 * rollAcc * dt) * dt;
    }

    return roll;
}

/**
 * Get this object's roll acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double ProjectedKinematicState::getRollAcceleration(const AngleUnitType &angleUnits) const
{
    return m_eulerAccelerations.getRoll(angleUnits);
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double ProjectedKinematicState::getRollRate(const AngleUnitType &angleUnits,
                                            double t) const
{
    auto rollRate = m_eulerRates.getRoll(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto rollAcc = m_eulerAccelerations.getRoll(angleUnits);
        if (rollAcc != 0.0)
            rollRate += rollAcc * dt;
    }

    return rollRate;
}

/**
 * Get the state velocity at the specified time
 */
void ProjectedKinematicState::getVelocity(double velocity[3],
                                          double t) const
{
    m_velocity.get(velocity);

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        velocity[0] += m_acceleration[0] * dt;
        velocity[1] += m_acceleration[1] * dt;
        velocity[2] += m_acceleration[2] * dt;
    }
}

/**
 * Get the state velocity
 */
Vector3d &ProjectedKinematicState::getVelocity(void)
{
    return m_velocity;
}

/**
 * Get the state velocity
 */
Vector3d ProjectedKinematicState::getVelocity(void) const
{
    return m_velocity;
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double ProjectedKinematicState::getYaw(const AngleUnitType &angleUnits,
                                       double t) const
{
    auto yaw = m_eulers.getYaw(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto yawAcc = m_eulerAccelerations.getYaw(angleUnits);
        auto yawRate = m_eulerRates.getYaw(angleUnits);
        if (yawAcc != 0.0 || yawRate != 0.0)
            yaw += (yawRate + 0.5 * yawAcc * dt) * dt;
    }

    return yaw;
}

/**
 * Get this object's yaw acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double ProjectedKinematicState::getYawAcceleration(const AngleUnitType &angleUnits) const
{
    return m_eulerAccelerations.getYaw(angleUnits);
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double ProjectedKinematicState::getYawRate(const AngleUnitType &angleUnits,
                                           double t) const
{
    auto yawRate = m_eulerRates.getYaw(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto yawAcc = m_eulerAccelerations.getYaw(angleUnits);
        if (yawAcc != 0.0)
            yawRate += yawAcc * dt;
    }

    return yawRate;
}

/**
 * Initialization function
 */
bool ProjectedKinematicState::initialize(void)
{
    bool bSuccess = KinematicState::initialize();
    if (bSuccess)
    {
        m_acceleration.set(0.0, 0.0, 0.0);
        m_eulerAccelerations.set(0.0, 0.0, 0.0);
        m_eulerRates.set(0.0, 0.0, 0.0);
        m_eulers.set(0.0, 0.0, 0.0);
        m_position.set(0.0, 0.0, 0.0);
        m_velocity.set(0.0, 0.0, 0.0);
    }

    return bSuccess;
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool ProjectedKinematicState::isEqual(const KinematicState &state,
                                      double tol) const
{
    return isEqual(dynamic_cast<const ProjectedKinematicState &>(state));
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool ProjectedKinematicState::isEqual(const ProjectedKinematicState &state,
                                      double tol) const
{
    bool bEqual = KinematicState::isEqual(state, tol);
    if (bEqual)
        bEqual = isSpatiallyEqual(state, tol);

    return bEqual;
}

/**
 * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
 * (tests all but time)
 */
bool ProjectedKinematicState::isSpatiallyEqual(const KinematicState &state,
                                               double tol) const
{
    return isSpatiallyEqual(dynamic_cast<const ProjectedKinematicState &>(state));
}

/**
 * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
 * (tests all but time)
 */
bool ProjectedKinematicState::isSpatiallyEqual(const ProjectedKinematicState &state,
                                               double tol) const
{
    bool bEqual = (memcmp((void *)this, (void *)&state, sizeof(ProjectedKinematicState)) == 0);
    if (!bEqual)
    {
        bEqual = (m_acceleration.isEqual(state.m_acceleration, tol) &&
                  m_eulerAccelerations.isEqual(state.m_eulerAccelerations, tol) &&
                  m_eulerRates.isEqual(state.m_eulerRates, tol) &&
                  m_eulers.isEqual(state.m_eulers, tol) &&
                  m_position.isEqual(state.m_position, tol) &&
                  m_velocity.isEqual(state.m_velocity, tol));
    }

    return bEqual;
}

/**
 * Function to print the contents of this reference frame state
 */
std::ostream &ProjectedKinematicState::print(std::ostream &stream) const
{
    KinematicState::print(stream);

    if (stream)
    {
        stream << " Position: " << m_position << std::endl
               << " Velocity: " << m_velocity << std::endl
               << " Acceleration: " << m_acceleration << std::endl
               << " Eulers (roll, pitch, yaw): " << m_eulers << std::endl
               << " Euler rates (roll, pitch, yaw): " << m_eulerRates << std::endl
               << " Euler accelerations (roll, pitch, yaw): " << m_eulerAccelerations;
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool ProjectedKinematicState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = KinematicState::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pPositionNode = pNode->first_node("position");
        if (pPositionNode != nullptr)
            m_position.readFromXML(pPositionNode);

        auto *pVelocityNode = pNode->first_node("velocity");
        if (pVelocityNode != nullptr)
            m_velocity.readFromXML(pVelocityNode);

        auto *pAccelerationNode = pNode->first_node("acceleration");
        if (pAccelerationNode != nullptr)
            m_acceleration.readFromXML(pAccelerationNode);

        auto *pEulersNode = pNode->first_node("eulers");
        if (pEulersNode != nullptr)
            m_eulers.readFromXML(pEulersNode);

        auto *pEulerRatesNode = pNode->first_node("eulerRates");
        if (pEulerRatesNode != nullptr)
            m_eulerRates.readFromXML(pEulerRatesNode);

        auto *pEulerAccelerationsNode = pNode->first_node("eulerAccelerations");
        if (pEulerAccelerationsNode != nullptr)
            m_eulerAccelerations.readFromXML(pEulerAccelerationsNode);
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &ProjectedKinematicState::serialize(std::ostream &stream) const
{
    KinematicState::serialize(stream);

    if (stream)
    {
        m_acceleration.serialize(stream);
        m_eulerAccelerations.serialize(stream);
        m_eulerRates.serialize(stream);
        m_eulers.serialize(stream);
        m_position.serialize(stream);
        m_velocity.serialize(stream);
    }

    return stream;
}

/**
 * Set the state acceleration
 */
void ProjectedKinematicState::setAcceleration(double xAcceleration,
                                              double yAcceleration,
                                              double zAcceleration)
{
    m_acceleration.set(xAcceleration,
                       yAcceleration,
                       zAcceleration);
}

/**
 * Set angle units (Degrees or Radians)
 */
void ProjectedKinematicState::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
    m_eulerAccelerations.setAngleUnits(m_angleUnits);
    m_eulerRates.setAngleUnits(m_angleUnits);
    m_eulers.setAngleUnits(m_angleUnits);
}

/**
 * Set Euler accelerations used to propagate Euler rates
 */
void ProjectedKinematicState::setEulerAccelerations(const Eulers &eulerAccelerations)
{
    setEulerAccelerations(eulerAccelerations[EulerAccelerationAxisType::Roll],
                          eulerAccelerations[EulerAccelerationAxisType::Pitch],
                          eulerAccelerations[EulerAccelerationAxisType::Yaw]);

    m_eulerAccelerations.convertAngleUnits(m_angleUnits);
}

/**
 * Set Euler accelerations used to propagate Euler rates
 */
void ProjectedKinematicState::setEulerAccelerations(double rollAcceleration,
                                                    double pitchAcceleration,
                                                    double yawAcceleration)
{
    m_eulerAccelerations.setPitch(pitchAcceleration);
    m_eulerAccelerations.setRoll(rollAcceleration);
    m_eulerAccelerations.setYaw(yawAcceleration);
}

/**
 * Set Euler rates used to propagate Euler angles
 */
void ProjectedKinematicState::setEulerRates(const Eulers &eulerRates)
{
    setEulerRates(eulerRates[EulerRateAxisType::Roll],
                  eulerRates[EulerRateAxisType::Pitch],
                  eulerRates[EulerRateAxisType::Yaw]);

    m_eulerRates.convertAngleUnits(m_angleUnits);
}

/**
 * Set Euler rates used to propagate Euler angles
 */
void ProjectedKinematicState::setEulerRates(double rollRate,
                                            double pitchRate,
                                            double yawRate)
{
    m_eulerRates.setPitch(pitchRate);
    m_eulerRates.setRoll(rollRate);
    m_eulerRates.setYaw(yawRate);
}

/**
 * Set the Euler angles
 */
void ProjectedKinematicState::setEulers(const Eulers &eulers)
{
    setEulers(eulers[EulerAxisType::Roll],
              eulers[EulerAxisType::Pitch],
              eulers[EulerAxisType::Yaw]);

    m_eulers.convertAngleUnits(m_angleUnits);
}

/**
 * Set the Euler angles
 */
void ProjectedKinematicState::setEulers(double roll,
                                        double pitch,
                                        double yaw)
{
    m_eulers.setPitch(pitch);
    m_eulers.setRoll(roll);
    m_eulers.setYaw(yaw);
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setPitch(double pitch,
                                       const AngleUnitType &angleUnits)
{
    m_eulers.setPitch(pitch, angleUnits);
}

/**
 * Set this object's pitch acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setPitchAcceleration(double pitchAcceleration,
                                                   const AngleUnitType &angleUnits)
{
    m_eulerAccelerations.setPitch(pitchAcceleration, angleUnits);
}

/**
 * Set this object's pitch rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setPitchRate(double pitchRate,
                                           const AngleUnitType &angleUnits)
{
    m_eulerRates.setPitch(pitchRate, angleUnits);
}

/**
 * Set the position of the origin of this frame with respect to its parent
 */
void ProjectedKinematicState::setPosition(double xPosition,
                                          double yPosition,
                                          double zPosition)
{
    m_position.set(xPosition,
                   yPosition,
                   zPosition);
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setRoll(double roll,
                                      const AngleUnitType &angleUnits)
{
    m_eulers.setRoll(roll, angleUnits);
}

/**
 * Set this object's roll acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setRollAcceleration(double rollAcceleration,
                                                  const AngleUnitType &angleUnits)
{
    m_eulerAccelerations.setRoll(rollAcceleration, angleUnits);
}

/**
 * Set this object's roll rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setRollRate(double rollRate,
                                          const AngleUnitType &angleUnits)
{
    m_eulerRates.setRoll(rollRate, angleUnits);
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 */
void ProjectedKinematicState::setVelocity(double xVelocity,
                                          double yVelocity,
                                          double zVelocity)
{
    m_velocity.set(xVelocity,
                   yVelocity,
                   zVelocity);
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setYaw(double yaw,
                                     const AngleUnitType &angleUnits)
{
    m_eulers.setYaw(yaw, angleUnits);
}

/**
 * Set this object's yaw acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setYawAcceleration(double yawAcceleration,
                                                 const AngleUnitType &angleUnits)
{
    m_eulerAccelerations.setYaw(yawAcceleration, angleUnits);
}

/**
 * Set this object's yaw rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedKinematicState::setYawRate(double yawRate,
                                         const AngleUnitType &angleUnits)
{
    m_eulerRates.setYaw(yawRate, angleUnits);
}

/**
 * Swap function
 */
void ProjectedKinematicState::swap(ProjectedKinematicState &state)
{
    KinematicState::swap(state);

    m_acceleration.swap(state.m_acceleration);
    m_eulerAccelerations.swap(state.m_eulerAccelerations);
    m_eulerRates.swap(state.m_eulerRates);
    m_eulers.swap(state.m_eulers);
    m_position.swap(state.m_position);
    m_velocity.swap(state.m_velocity);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool ProjectedKinematicState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = KinematicState::writeToXML(pNode);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess && m_position != 0.0)
        {
            auto *pPositionNode = pDocument->allocate_node(node_element, "position");
            bSuccess = m_position.writeToXML(pPositionNode);
            pNode->append_node(pPositionNode);
        }
        else
            m_position.set(0.0, 0.0, 0.0);

        if (bSuccess && m_velocity != 0.0)
        {
            auto *pVelocityNode = pDocument->allocate_node(node_element, "velocity");
            bSuccess = m_velocity.writeToXML(pVelocityNode);
            pNode->append_node(pVelocityNode);
        }

        if (bSuccess && m_acceleration != 0.0)
        {
            auto *pAccelerationNode = pDocument->allocate_node(node_element, "acceleration");
            bSuccess = m_acceleration.writeToXML(pAccelerationNode);
            pNode->append_node(pAccelerationNode);
        }

        if (bSuccess && m_eulers != 0.0)
        {
            auto *pEulersNode = pDocument->allocate_node(node_element, "eulers");
            bSuccess = m_eulers.writeToXML(pEulersNode);
            pNode->append_node(pEulersNode);
        }

        if (bSuccess && m_eulerRates != 0.0)
        {
            auto *pEulerRatesNode = pDocument->allocate_node(node_element, "eulerRates");
            bSuccess = m_eulerRates.writeToXML(pEulerRatesNode);
            pNode->append_node(pEulerRatesNode);
        }

        if (bSuccess && m_eulerAccelerations != 0.0)
        {
            auto *pEulerAccelerationsNode = pDocument->allocate_node(node_element, "eulerAccelerations");
            bSuccess = m_eulerAccelerations.writeToXML(pEulerAccelerationsNode);
            pNode->append_node(pEulerAccelerationsNode);
        }
    }

    return bSuccess;
}
#endif
}

}
