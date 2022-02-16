#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "interpolatedFrameState.h"
#include "projectedFrameState.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include <cmath>
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
static FactoryRegistrar<FrameState>
factory(factoryName, (ProjectedFrameState * (*)(const std::string &))&ProjectedFrameState::create);

/**
 * Constructor
 * @param name       the name of this reference frame state
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
ProjectedFrameState::ProjectedFrameState(const std::string &name,
                                         const AngleUnitType &angleUnits)
: FrameState(name,
             angleUnits)
{

}

/**
 * Copy constructor
 */
ProjectedFrameState::ProjectedFrameState(const ProjectedFrameState &state)
{
    operator = (state);
}

/**
 * Move constructor
 */
ProjectedFrameState::ProjectedFrameState(ProjectedFrameState &&state)
{
    operator = (std::move(state));
}

/**
 * Destructor
 */
ProjectedFrameState::~ProjectedFrameState(void)
{

}

/**
 * Copy assignment operator
 */
ProjectedFrameState &ProjectedFrameState::operator = (const ProjectedFrameState &state)
{
    if (&state != this)
    {
        FrameState::operator = (state);

        m_acceleration = state.m_acceleration;
        m_orientation = state.m_orientation;
        m_origin = state.m_origin;
        m_rotationalAccelerations = state.m_rotationalAccelerations;
        m_rotationalRates = state.m_rotationalRates;
        m_velocity = state.m_velocity;
    }

    return *this;
}

/**
 * Move assignment operator
 */
ProjectedFrameState &ProjectedFrameState::operator = (ProjectedFrameState &&state)
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
bool ProjectedFrameState::operator == (const FrameState &state) const
{
    return operator == (dynamic_cast<const ProjectedFrameState &>(state));
}

/**
 * Equality operator
 */
bool ProjectedFrameState::operator == (const ProjectedFrameState &state) const
{
    bool bEqual = FrameState::operator == (state);
    if (bEqual)
    {
        bEqual = (m_acceleration == state.m_acceleration &&
                  m_orientation == state.m_orientation &&
                  m_origin == state.m_origin &&
                  m_rotationalAccelerations == state.m_rotationalAccelerations &&
                  m_rotationalRates == state.m_rotationalRates &&
                  m_velocity == state.m_velocity);
    }

    return bEqual;
}

/**
 * Assign the values of this frame state from another frame state
 */
bool ProjectedFrameState::assign(const FrameState *pState)
{
    auto *pProjectedFrameState = dynamic_cast<const ProjectedFrameState *>(pState);
    if (pProjectedFrameState != nullptr)
        return assign(pProjectedFrameState);

    auto *pInterpolatedFrameStae = dynamic_cast<const InterpolatedFrameState *>(pState);
    if (pInterpolatedFrameStae != nullptr)
        return assign(pInterpolatedFrameStae);

    return false;
}

/**
 * Assign the values of this frame state from another frame state
 */
bool ProjectedFrameState::assign(const InterpolatedFrameState *pState)
{
    bool bSuccess = FrameState::assign(pState);
    if (bSuccess)
    {
        setAcceleration(pState->getAcceleration());
        setOrientation(pState->getOrientation());
        setOrigin(pState->getOrigin());
        setRotationalAccelerations(pState->getRotationalAccelerations());
        setRotationalRates(pState->getRotationalRates());
        setVelocity(pState->getVelocity());
    }

    return bSuccess;
}

/**
 * Assign the values of this frame state from another frame state
 */
bool ProjectedFrameState::assign(const ProjectedFrameState *pState)
{
    bool bSuccess = FrameState::assign(pState);
    if (bSuccess)
    {
        operator = (*pState);
    }

    return bSuccess;
}

/**
 * clone() function
 */
ProjectedFrameState *ProjectedFrameState::clone(void) const
{
    return new ProjectedFrameState(*this);
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void ProjectedFrameState::convertAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
    m_orientation.convertAngleUnits(m_angleUnits);
    m_rotationalAccelerations.convertAngleUnits(m_angleUnits);
    m_rotationalRates.convertAngleUnits(m_angleUnits);
}

/**
 * create() function
 * @param name the name of this reference frame state
 */
ProjectedFrameState *ProjectedFrameState::create(const std::string &name)
{
    return create(name, AngleUnitType::Degrees);
}

/**
 * create() function
 * @param name       the name of this reference frame state
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
ProjectedFrameState *ProjectedFrameState::create(const std::string &name,
                                                 const AngleUnitType &angleUnits)
{
    ProjectedFrameState *pFrameState = nullptr;
    if (!name.empty())
        pFrameState = new ProjectedFrameState(name, angleUnits);

    return pFrameState;
}

/**
 * Function to deserialize this object's data
 */
std::istream &ProjectedFrameState::deserialize(std::istream &stream)
{
    FrameState::deserialize(stream);

    if (stream)
    {
        m_acceleration.deserialize(stream);
        m_orientation.deserialize(stream);
        m_origin.deserialize(stream);
        m_rotationalAccelerations.deserialize(stream);
        m_rotationalRates.deserialize(stream);
        m_velocity.deserialize(stream);
    }

    return stream;
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 */
Vector3d &ProjectedFrameState::getAcceleration(void)
{
    return m_acceleration;
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 */
Vector3d ProjectedFrameState::getAcceleration(void) const
{
    return m_acceleration;
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 */
void ProjectedFrameState::getAcceleration(double acceleration[3]) const
{
    m_acceleration.get(acceleration);
}

/**
 * Get the name of this class
 */
std::string ProjectedFrameState::getClassName(void) const
{
    return "ProjectedFrameState";
}

/**
 * Get the factory name of this constructible
 */
std::string ProjectedFrameState::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get the orientation angles of this frame with respect to its parent at the specified time
 */
void ProjectedFrameState::getOrientation(double &roll,
                                         double &pitch,
                                         double &yaw,
                                         double t) const
{
    pitch = m_orientation.getPitch();
    roll = m_orientation.getRoll();
    yaw = m_orientation.getYaw();

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        pitch += (m_rotationalRates.getPitch() + 0.5 * m_rotationalAccelerations.getPitch() * dt) * dt;
        roll += (m_rotationalRates.getRoll() + 0.5 * m_rotationalAccelerations.getRoll() * dt) * dt;
        yaw += (m_rotationalRates.getYaw() + 0.5 * m_rotationalAccelerations.getYaw() * dt) * dt;
    }
}

/**
 * Get the orientation angles of this frame with respect to its parent
 */
Eulers &ProjectedFrameState::getOrientation(void)
{
    return m_orientation;
}

/**
 * Get the orientation angles of this frame with respect to its parent
 */
Eulers ProjectedFrameState::getOrientation(void) const
{
    return m_orientation;
}

/**
 * Get the position of the origin of this frame with respect to its parent at the specified time
 */
void ProjectedFrameState::getOrigin(double origin[3],
                                    double t) const
{
    m_origin.get(origin);

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        origin[0] += (m_velocity[0] + 0.5 * m_acceleration[0] * dt) * dt;
        origin[1] += (m_velocity[1] + 0.5 * m_acceleration[1] * dt) * dt;
        origin[2] += (m_velocity[2] + 0.5 * m_acceleration[2] * dt) * dt;
    }
}

/**
 * Get the position of the origin of this frame with respect to its parent
 */
Vector3d &ProjectedFrameState::getOrigin(void)
{
    return m_origin;
}

/**
 * Get the position of the origin of this frame with respect to its parent
 */
Vector3d ProjectedFrameState::getOrigin(void) const
{
    return m_origin;
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time to which the return value will be projected
 */
double ProjectedFrameState::getPitch(const AngleUnitType &angleUnits,
                                     double t) const
{
    auto pitch = m_orientation.getPitch(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto pitchAcc = m_rotationalAccelerations.getPitch(angleUnits);
        auto pitchRate = m_rotationalRates.getPitch(angleUnits);
        if (pitchAcc != 0.0 || pitchRate != 0.0)
            pitch += (pitchRate + 0.5 * pitchAcc * dt) * dt;
    }

    return pitch;
}

/**
 * Get this object's pitch acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double ProjectedFrameState::getPitchAcceleration(const AngleUnitType &angleUnits) const
{
    return m_rotationalAccelerations.getPitch(angleUnits);
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time to which the return value will be projected
 */
double ProjectedFrameState::getPitchRate(const AngleUnitType &angleUnits,
                                         double t) const
{
    auto pitchRate = m_rotationalRates.getPitch(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto pitchAcc = m_rotationalAccelerations.getPitch(angleUnits);
        if (pitchAcc != 0.0)
            pitchRate += pitchAcc * dt;
    }

    return pitchRate;
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time to which the return value will be projected
 */
double ProjectedFrameState::getRoll(const AngleUnitType &angleUnits,
                                    double t) const
{
    auto roll = m_orientation.getRoll(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto rollAcc = m_rotationalAccelerations.getRoll(angleUnits);
        auto rollRate = m_rotationalRates.getRoll(angleUnits);
        if (rollAcc != 0.0 || rollRate != 0.0)
            roll += (rollRate + 0.5 * rollAcc * dt) * dt;
    }

    return roll;
}

/**
 * Get this object's roll acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double ProjectedFrameState::getRollAcceleration(const AngleUnitType &angleUnits) const
{
    return m_rotationalAccelerations.getRoll(angleUnits);
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time to which the return value will be projected
 */
double ProjectedFrameState::getRollRate(const AngleUnitType &angleUnits,
                                        double t) const
{
    auto rollRate = m_rotationalRates.getRoll(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto rollAcc = m_rotationalAccelerations.getRoll(angleUnits);
        if (rollAcc != 0.0)
            rollRate += rollAcc * dt;
    }

    return rollRate;
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
void ProjectedFrameState::getRotationalAccelerations(double &rollAccel,
                                                     double &pitchAccel,
                                                     double &yawAccel) const
{
    pitchAccel = m_rotationalAccelerations.getPitch();
    rollAccel = m_rotationalAccelerations.getRoll();
    yawAccel = m_rotationalAccelerations.getYaw();
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
Eulers &ProjectedFrameState::getRotationalAccelerations(void)
{
    return m_rotationalAccelerations;
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
Eulers ProjectedFrameState::getRotationalAccelerations(void) const
{
    return m_rotationalAccelerations;
}

/**
 * Get the rotational rates of this frame with respect to its parent at the specified time
 */
void ProjectedFrameState::getRotationalRates(double &rollRate,
                                             double &pitchRate,
                                             double &yawRate,
                                             double t) const
{
    pitchRate = m_rotationalRates.getPitch();
    rollRate = m_rotationalRates.getRoll();
    yawRate = m_rotationalRates.getYaw();

    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        pitchRate += m_rotationalAccelerations.getPitch() * dt;
        rollRate += m_rotationalAccelerations.getRoll() * dt;
        yawRate += m_rotationalAccelerations.getYaw() * dt;
    }
}

/**
 * Get the rotational rates of this frame with respect to its parent
 */
Eulers &ProjectedFrameState::getRotationalRates(void)
{
    return m_rotationalRates;
}

/**
 * Get the rotational rates of this frame with respect to its parent
 */
Eulers ProjectedFrameState::getRotationalRates(void) const
{
    return m_rotationalRates;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent at the specified time
 */
void ProjectedFrameState::getVelocity(double velocity[3],
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
 * Get the velocity of the origin of this frame with respect to its parent
 */
Vector3d &ProjectedFrameState::getVelocity(void)
{
    return m_velocity;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent
 */
Vector3d ProjectedFrameState::getVelocity(void) const
{
    return m_velocity;
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time to which the return value will be projected
 */
double ProjectedFrameState::getYaw(const AngleUnitType &angleUnits,
                                   double t) const
{
    auto yaw = m_orientation.getYaw(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto yawAcc = m_rotationalAccelerations.getYaw(angleUnits);
        auto yawRate = m_rotationalRates.getYaw(angleUnits);
        if (yawAcc != 0.0 || yawRate != 0.0)
            yaw += (yawRate + 0.5 * yawAcc * dt) * dt;
    }

    return yaw;
}

/**
 * Get this object's yaw acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double ProjectedFrameState::getYawAcceleration(const AngleUnitType &angleUnits) const
{
    return m_rotationalAccelerations.getYaw(angleUnits);
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time to which the return value will be projected
 */
double ProjectedFrameState::getYawRate(const AngleUnitType &angleUnits,
                                       double t) const
{
    auto yawRate = m_rotationalRates.getYaw(angleUnits);
    auto dt = t - m_t0;
    if (dt != 0.0)
    {
        auto yawAcc = m_rotationalAccelerations.getYaw(angleUnits);
        if (yawAcc != 0.0)
            yawRate += yawAcc * dt;
    }

    return yawRate;
}

/**
 * Initialization function
 */
bool ProjectedFrameState::initialize(void)
{
    bool bSuccess = FrameState::initialize();
    if (bSuccess)
    {
        m_orientation.set(0.0, 0.0, 0.0);
        m_origin.set(0.0, 0.0, 0.0);

        initializeTimeDerivatives();
    }

    return bSuccess;
}

/**
 * Initialization function to reset time-derivatives to zero
 */
void ProjectedFrameState::initializeTimeDerivatives(void)
{
    m_acceleration.set(0.0, 0.0, 0.0);
    m_rotationalAccelerations.set(0.0, 0.0, 0.0);
    m_rotationalRates.set(0.0, 0.0, 0.0);
    m_velocity.set(0.0, 0.0, 0.0);
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool ProjectedFrameState::isEqual(const FrameState &state,
                                  double tol) const
{
    return isEqual(dynamic_cast<const ProjectedFrameState &>(state));
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool ProjectedFrameState::isEqual(const ProjectedFrameState &state,
                                  double tol) const
{
    bool bEqual = FrameState::isEqual(state, tol);
    if (bEqual)
        bEqual = isSpatiallyEqual(state, tol);

    return bEqual;
}

/**
 * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
 * (tests all but time)
 */
bool ProjectedFrameState::isSpatiallyEqual(const FrameState &state,
                                           double tol) const
{
    return isSpatiallyEqual(dynamic_cast<const ProjectedFrameState &>(state));
}

/**
 * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
 * (tests all but time)
 */
bool ProjectedFrameState::isSpatiallyEqual(const ProjectedFrameState &state,
                                           double tol) const
{
    bool bEqual = (memcmp((void *)this, (void *)&state, sizeof(ProjectedFrameState)) == 0);
    if (!bEqual)
    {
        bEqual = (m_acceleration.isEqual(state.m_acceleration, tol) &&
                  m_orientation.isEqual(state.m_orientation, tol) &&
                  m_origin.isEqual(state.m_origin, tol) &&
                  m_rotationalAccelerations.isEqual(state.m_rotationalAccelerations, tol) &&
                  m_rotationalRates.isEqual(state.m_rotationalRates, tol) &&
                  m_velocity.isEqual(state.m_velocity, tol));
    }

    return bEqual;
}

/**
 * Function to print the contents of this reference frame state
 */
std::ostream &ProjectedFrameState::print(std::ostream &stream) const
{
    FrameState::print(stream);

    if (stream)
    {
        stream << " Origin (x, y, z): " << m_origin << std::endl
               << " Velocity (xd, yd, zd): " << m_velocity << std::endl
               << " Acceleration (xdd, ydd, zdd): " << m_acceleration << std::endl
               << " Orientation (roll, pitch, yaw): " << m_orientation << std::endl
               << " Rotational rates (roll, pitch, yaw): " << m_rotationalRates << std::endl
               << " Rotational accelerations (roll, pitch, yaw): " << m_rotationalAccelerations;
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool ProjectedFrameState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = FrameState::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pOriginNode = pNode->first_node("origin");
        if (pOriginNode != nullptr)
            m_origin.readFromXML(pOriginNode);

        auto *pVelocityNode = pNode->first_node("velocity");
        if (pVelocityNode != nullptr)
            m_velocity.readFromXML(pVelocityNode);

        auto *pAccelerationNode = pNode->first_node("acceleration");
        if (pAccelerationNode != nullptr)
            m_acceleration.readFromXML(pAccelerationNode);

        auto *pOrientationNode = pNode->first_node("orientation");
        if (pOrientationNode != nullptr)
            m_orientation.readFromXML(pOrientationNode);
        else
            m_orientation.set(0.0, 0.0, 0.0);

        auto *pRotationalRatesNode = pNode->first_node("rotationalRates");
        if (pRotationalRatesNode != nullptr)
            m_rotationalRates.readFromXML(pRotationalRatesNode);

        auto *pRotationalAccelerationsNode = pNode->first_node("rotationalAccelerations");
        if (pRotationalAccelerationsNode != nullptr)
            m_rotationalAccelerations.readFromXML(pRotationalAccelerationsNode);
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &ProjectedFrameState::serialize(std::ostream &stream) const
{
    FrameState::serialize(stream);

    if (stream)
    {
        m_acceleration.serialize(stream);
        m_orientation.serialize(stream);
        m_origin.serialize(stream);
        m_rotationalAccelerations.serialize(stream);
        m_rotationalRates.serialize(stream);
        m_velocity.serialize(stream);
    }

    return stream;
}

/**
 * Set the acceleration of the origin of this frame with respect to its parent
 */
void ProjectedFrameState::setAcceleration(double xAcceleration,
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
void ProjectedFrameState::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
    m_orientation.setAngleUnits(m_angleUnits);
    m_rotationalAccelerations.setAngleUnits(m_angleUnits);
    m_rotationalRates.setAngleUnits(m_angleUnits);
}

/**
 * Set the orientation angles of this frame with respect to its parent
 */
void ProjectedFrameState::setOrientation(const Eulers &orientation)
{
    setOrientation(orientation[EulerAxisType::Roll],
                   orientation[EulerAxisType::Pitch],
                   orientation[EulerAxisType::Yaw]);

    m_orientation.convertAngleUnits(m_angleUnits);
}

/**
 * Set the orientation angles of this frame with respect to its parent
 */
void ProjectedFrameState::setOrientation(double roll,
                                         double pitch,
                                         double yaw)
{
    m_orientation.setPitch(pitch);
    m_orientation.setRoll(roll);
    m_orientation.setYaw(yaw);
}

/**
 * Set the position of the origin of this frame with respect to its parent
 */
void ProjectedFrameState::setOrigin(double xPosition,
                                    double yPosition,
                                    double zPosition)
{
    m_origin.setX(xPosition);
    m_origin.setY(yPosition);
    m_origin.setZ(zPosition);
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setPitch(double pitch,
                                   const AngleUnitType &angleUnits)
{
    m_orientation.setPitch(pitch, angleUnits);
}

/**
 * Set this object's pitch acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setPitchAcceleration(double pitchAcceleration,
                                               const AngleUnitType &angleUnits)
{
    m_rotationalAccelerations.setPitch(pitchAcceleration, angleUnits);
}

/**
 * Set this object's pitch rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setPitchRate(double pitchRate,
                                       const AngleUnitType &angleUnits)
{
    m_rotationalRates.setPitch(pitchRate, angleUnits);
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setRoll(double roll,
                                  const AngleUnitType &angleUnits)
{
    m_orientation.setRoll(roll, angleUnits);
}

/**
 * Set this object's roll acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setRollAcceleration(double rollAcceleration,
                                              const AngleUnitType &angleUnits)
{
    m_rotationalAccelerations.setRoll(rollAcceleration, angleUnits);
}

/**
 * Set this object's roll rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setRollRate(double rollRate,
                                      const AngleUnitType &angleUnits)
{
    m_rotationalRates.setRoll(rollRate, angleUnits);
}

/**
 * Set the rotational accelerations of this frame with respect to its parent
 */
void ProjectedFrameState::setRotationalAccelerations(const Eulers &rotationalAccelerations)
{
    setRotationalAccelerations(rotationalAccelerations[EulerAccelerationAxisType::Roll],
                               rotationalAccelerations[EulerAccelerationAxisType::Pitch],
                               rotationalAccelerations[EulerAccelerationAxisType::Yaw]);

    m_rotationalAccelerations.convertAngleUnits(m_angleUnits);
}

/**
 * Set the rotational accelerations of this frame with respect to its parent
 */
void ProjectedFrameState::setRotationalAccelerations(double rollAcceleration,
                                                     double pitchAcceleration,
                                                     double yawAcceleration)
{
    m_rotationalAccelerations[EulerAccelerationAxisType::Pitch] = pitchAcceleration;
    m_rotationalAccelerations[EulerAccelerationAxisType::Roll] = rollAcceleration;
    m_rotationalAccelerations[EulerAccelerationAxisType::Yaw] = yawAcceleration;
}

/**
 * Set the rotational rates of this frame with respect to its parent
 */
void ProjectedFrameState::setRotationalRates(const Eulers &rotationalRates)
{
    setRotationalRates(rotationalRates[EulerRateAxisType::Roll],
                       rotationalRates[EulerRateAxisType::Pitch],
                       rotationalRates[EulerRateAxisType::Yaw]);

    m_rotationalRates.convertAngleUnits(m_angleUnits);
}

/**
 * Set the rotational rates of this frame with respect to its parent
 */
void ProjectedFrameState::setRotationalRates(double rollRate,
                                             double pitchRate,
                                             double yawRate)
{
    m_rotationalRates[EulerRateAxisType::Pitch] = pitchRate;
    m_rotationalRates[EulerRateAxisType::Roll] = rollRate;
    m_rotationalRates[EulerRateAxisType::Yaw] = yawRate;
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 */
void ProjectedFrameState::setVelocity(double xVelocity,
                                      double yVelocity,
                                      double zVelocity)
{
    m_velocity.setX(xVelocity);
    m_velocity.setY(yVelocity);
    m_velocity.setZ(zVelocity);
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setYaw(double yaw,
                                 const AngleUnitType &angleUnits)
{
    m_orientation.setYaw(yaw, angleUnits);
}

/**
 * Set this object's yaw acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setYawAcceleration(double yawAcceleration,
                                             const AngleUnitType &angleUnits)
{
    m_rotationalAccelerations.setYaw(yawAcceleration, angleUnits);
}

/**
 * Set this object's yaw rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void ProjectedFrameState::setYawRate(double yawRate,
                                     const AngleUnitType &angleUnits)
{
    m_rotationalRates.setYaw(yawRate, angleUnits);
}

/**
 * Swap function
 */
void ProjectedFrameState::swap(ProjectedFrameState &state)
{
    FrameState::swap(state);

    m_acceleration.swap(state.m_acceleration);
    m_orientation.swap(state.m_orientation);
    m_origin.swap(state.m_origin);
    m_rotationalAccelerations.swap(state.m_rotationalAccelerations);
    m_rotationalRates.swap(state.m_rotationalRates);
    m_velocity.swap(state.m_velocity);
}

/**
 * Update this reference frame state to the specified time. This function will update all quantities within the
 * frame state that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param timeReference enumeration specifying whether the update time is an absolute time or a delta time
 *                      (with respect to current time)
 */
void ProjectedFrameState::update(double t,
                                 const TimeReferenceType &timeReference)
{
    if (timeReference == TimeReferenceType::Delta)
        t += m_t0;

    // order matters
    if (t != m_t0)
    {
        getOrigin(m_origin, t);
        getVelocity(m_velocity, t);
        getOrientation(m_orientation, t);
        getRotationalRates(m_rotationalRates, t);
        setTime(t);
    }
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool ProjectedFrameState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = FrameState::writeToXML(pNode);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess && m_origin != 0.0)
        {
            auto *pOriginNode = pDocument->allocate_node(node_element, "origin");
            bSuccess = m_origin.writeToXML(pOriginNode);
            pNode->append_node(pOriginNode);
        }

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

        if (bSuccess && m_orientation != 0.0)
        {
            auto *pOrientationNode = pDocument->allocate_node(node_element, "orientation");
            bSuccess = m_orientation.writeToXML(pOrientationNode);
            pNode->append_node(pOrientationNode);
        }

        if (bSuccess && m_rotationalRates != 0.0)
        {
            auto *pRotationalRatesNode = pDocument->allocate_node(node_element, "rotationalRates");
            bSuccess = m_rotationalRates.writeToXML(pRotationalRatesNode);
            pNode->append_node(pRotationalRatesNode);
        }

        if (bSuccess && m_rotationalAccelerations != 0.0)
        {
            auto *pRotationalAccelerationsNode = pDocument->allocate_node(node_element,
                                                                          "rotationalAccelerations");
            bSuccess = m_rotationalAccelerations.writeToXML(pRotationalAccelerationsNode);
            pNode->append_node(pRotationalAccelerationsNode);
        }
    }

    return bSuccess;
}
#endif
}

}
