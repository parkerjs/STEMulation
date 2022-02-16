#include "angle_unit_type.h"
#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "kinematicState.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include <cstring>
#include <string>
#include <typeinfo>

// using namespace declarations
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

/**
 * Constructor
 */
KinematicState::KinematicState(void)
: KinematicState(AngleUnitType::Degrees)
{

}

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, degrees or Radians
 */
KinematicState::KinematicState(const AngleUnitType &angleUnits)
: m_angleUnits(angleUnits),
  m_t0(0.0)
{

}

/**
 * Copy constructor
 */
KinematicState::KinematicState(const KinematicState &state)
{
    operator = (state);
}

/**
 * Move constructor
 */
KinematicState::KinematicState(KinematicState &&state)
{
    operator = (std::move(state));
}

/**
 * Destructor
 */
KinematicState::~KinematicState(void)
{

}

/**
 * Copy assignment operator
 */
KinematicState &KinematicState::operator = (const KinematicState &state)
{
    if (&state != this)
    {
        m_angleUnits = state.m_angleUnits;
        m_t0 = state.m_t0;
    }

    return *this;
}

/**
 * Move assignment operator
 */
KinematicState &KinematicState::operator = (KinematicState &&state)
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
bool KinematicState::operator == (const KinematicState &state) const
{
    return (m_angleUnits == state.m_angleUnits);
}

/**
 * Inequality operator
 */
bool KinematicState::operator != (const KinematicState &state) const
{
    return !operator == (state);
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const KinematicState &state)
{
    return state.print(stream);
}

/**
 * Function to deserialize this object's data
 */
std::istream &KinematicState::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_angleUnits, sizeof(int));
        stream.read((char *)&m_t0, sizeof(double));
    }

    return stream;
}

/**
 * Get angle units (Degrees or Radians)
 */
AngleUnitType KinematicState::getAngleUnits(void) const
{
    return m_angleUnits;
}

/**
 * Get the name of this class
 */
std::string KinematicState::getClassName(void) const
{
    return "KinematicState";
}

/**
 * Get the Euler accelerations (roll, pitch, yaw)
 */
void KinematicState::getEulerAccelerations(double eulerAccelerations[3]) const
{
    getEulerRates(eulerAccelerations[EulerAccelerationAxisType::Roll],
                  eulerAccelerations[EulerAccelerationAxisType::Pitch],
                  eulerAccelerations[EulerAccelerationAxisType::Yaw]);
}

/**
 * Get the Euler rates
 */
void KinematicState::getEulerRates(double &rollRate,
                                   double &pitchRate,
                                   double &yawRate) const
{
    getEulerRates(rollRate,
                  pitchRate,
                  yawRate,
                  m_t0);
}

/**
 * Get the Euler rates
 */
void KinematicState::getEulerRates(double eulerRates[3]) const
{
    getEulerRates(eulerRates, m_t0);
}

/**
 * Get the Euler rates at the specified time
 */
void KinematicState::getEulerRates(double eulerRates[3],
                                   double t) const
{
    getEulerRates(eulerRates[EulerRateAxisType::Roll],
                  eulerRates[EulerRateAxisType::Pitch],
                  eulerRates[EulerRateAxisType::Yaw], t);
}

/**
 * Get the Euler rates at the specified time
 */
Eulers KinematicState::getEulerRates(double t) const
{
    Eulers eulerRates(m_angleUnits);

    getEulerRates(eulerRates, t);

    return eulerRates;
}

/**
 * Get the Euler angles
 */
void KinematicState::getEulers(double &roll,
                               double &pitch,
                               double &yaw) const
{
    getEulers(roll, pitch, yaw, m_t0);
}

/**
 * Get the Euler angles
 */
void KinematicState::getEulers(double eulers[3]) const
{
    getEulers(eulers, m_t0);
}

/**
 * Get the Euler angles at the specified time
 */
void KinematicState::getEulers(double eulers[3],
                               double t) const
{
    getEulers(eulers[EulerAxisType::Roll],
              eulers[EulerAxisType::Pitch],
              eulers[EulerAxisType::Yaw], t);
}

/**
 * Get the Euler angles at the specified time
 */
Eulers KinematicState::getEulers(double t) const
{
    Eulers eulers(m_angleUnits);

    getEulers(eulers, t);

    return eulers;
}

/**
 * Get this object's pitch angle
 */
double KinematicState::getPitch(void) const
{
    return getPitch(m_angleUnits, m_t0);
}

/**
 * Get this object's pitch angle
 * @param t the time at which the return value will be computed
 */
double KinematicState::getPitch(double t) const
{
    return getPitch(m_angleUnits, t);
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double KinematicState::getPitch(const AngleUnitType &angleUnits) const
{
    return getPitch(angleUnits, m_t0);
}

/**
 * Get this object's pitch acceleration
 */
double KinematicState::getPitchAcceleration(void) const
{
    return getPitchAcceleration(m_angleUnits);
}

/**
 * Get this object's pitch rate
 */
double KinematicState::getPitchRate(void) const
{
    return getPitchRate(m_angleUnits, m_t0);
}

/**
 * Get this object's pitch rate
 * @param t the time at which the return value will be computed
 */
double KinematicState::getPitchRate(double t) const
{
    return getPitchRate(m_angleUnits, t);
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double KinematicState::getPitchRate(const AngleUnitType &angleUnits) const
{
    return getPitchRate(angleUnits, m_t0);
}

/**
 * Get the state position
 */
void KinematicState::getPosition(double position[3]) const
{
    getPosition(position, m_t0);
}

/**
 * Get the state position at the specified time
 */
Vector3d KinematicState::getPosition(double t) const
{
    Vector3d position;

    getPosition(position, t);

    return position;
}

/**
 * Get this object's roll angle
 */
double KinematicState::getRoll(void) const
{
    return getRoll(m_angleUnits, m_t0);
}

/**
 * Get this object's roll angle
 * @param t the time at which the return value will be computed
 */
double KinematicState::getRoll(double t) const
{
    return getRoll(m_angleUnits, t);
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double KinematicState::getRoll(const AngleUnitType &angleUnits) const
{
    return getRoll(angleUnits, m_t0);
}

/**
 * Get this object's roll acceleration
 */
double KinematicState::getRollAcceleration(void) const
{
    return getRollAcceleration(m_angleUnits);
}

/**
 * Get this object's roll rate
 */
double KinematicState::getRollRate(void) const
{
    return getRollRate(m_angleUnits, m_t0);
}

/**
 * Get this object's roll rate
 * @param t the time at which the return value will be computed
 */
double KinematicState::getRollRate(double t) const
{
    return getRollRate(m_angleUnits, t);
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double KinematicState::getRollRate(const AngleUnitType &angleUnits) const
{
    return getRollRate(angleUnits, m_t0);
}

/**
 * Get the time
 */
double KinematicState::getTime(void) const
{
    return m_t0;
}

/**
 * Get the state velocity
 */
void KinematicState::getVelocity(double velocity[3]) const
{
    getVelocity(velocity, m_t0);
}

/**
 * Get the state velocity at the specified time
 */
Vector3d KinematicState::getVelocity(double t) const
{
    Vector3d velocity;

    getVelocity(velocity, t);

    return velocity;
}

/**
 * Get this object's yaw angle
 */
double KinematicState::getYaw(void) const
{
    return getYaw(m_angleUnits, m_t0);
}

/**
 * Get this object's yaw angle
 * @param t the time at which the return value will be computed
 */
double KinematicState::getYaw(double t) const
{
    return getYaw(m_angleUnits, t);
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double KinematicState::getYaw(const AngleUnitType &angleUnits) const
{
    return getYaw(angleUnits, m_t0);
}

/**
 * Get this object's yaw acceleration
 */
double KinematicState::getYawAcceleration(void) const
{
    return getYawAcceleration(m_angleUnits);
}

/**
 * Get this object's yaw rate
 */
double KinematicState::getYawRate(void) const
{
    return getYawRate(m_angleUnits, m_t0);
}

/**
 * Get this object's yaw rate
 * @param t the time at which the return value will be computed
 */
double KinematicState::getYawRate(double t) const
{
    return getYawRate(m_angleUnits, t);
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double KinematicState::getYawRate(const AngleUnitType &angleUnits) const
{
    return getYawRate(angleUnits, m_t0);
}

/**
 * Initialization function
 */
bool KinematicState::initialize(void)
{
    m_t0 = 0.0;

    return true;
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool KinematicState::isEqual(const KinematicState &state,
                             double tol) const
{
    bool bEqual = (&state == this);
    if (!bEqual)
    {
        bEqual = m_angleUnits == state.m_angleUnits &&
                 std::fabs(m_t0 - state.m_t0) <= tol &&
                 isSpatiallyEqual(state);
    }

    return bEqual;
}

/**
 * Function to print the contents of this reference frame state
 */
std::ostream &KinematicState::print(std::ostream &stream) const
{
    if (stream)
    {
        stream << " Time (s): { " << m_t0 << " }" << std::endl
               << " Angle type: { " << std::string(m_angleUnits) << " }";
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool KinematicState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pTimeNode = pNode->first_node("time");
        if (pTimeNode != nullptr)
            setTime(std::stod(pTimeNode->value()));

        auto *pAngleUnitsNode = pNode->first_node("angleType");
        if (pAngleUnitsNode != nullptr)
            setAngleUnits(AngleUnitType(pAngleUnitsNode->value()));
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &KinematicState::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_angleUnits, sizeof(int));
        stream.write((const char *)&m_t0, sizeof(double));
    }

    return stream;
}

/**
 * Set the state acceleration
 */
void KinematicState::setAcceleration(const Vector3d &acceleration)
{
    setAcceleration(acceleration.getX(),
                    acceleration.getY(),
                    acceleration.getZ());
}

/**
 * Set this object's pitch angle
 */
void KinematicState::setPitch(double pitch)
{
    setPitch(pitch, m_angleUnits);
}

/**
 * Set this object's pitch acceleration
 */
void KinematicState::setPitchAcceleration(double pitchAcceleration)
{
    setPitchAcceleration(pitchAcceleration, m_angleUnits);
}

/**
 * Set this object's pitch rate
 */
void KinematicState::setPitchRate(double pitchRate)
{
    setPitchRate(pitchRate, m_angleUnits);
}

/**
 * Set the state initial position
 */
void KinematicState::setPosition(const Vector3d &position)
{
    setPosition(position.getX(),
                position.getY(),
                position.getZ());
}

/**
 * Set this object's roll angle
 */
void KinematicState::setRoll(double roll)
{
    setRoll(roll, m_angleUnits);
}

/**
 * Set this object's roll acceleration
 */
void KinematicState::setRollAcceleration(double rollAcceleration)
{
    setRollAcceleration(rollAcceleration, m_angleUnits);
}

/**
 * Set this object's roll rate
 */
void KinematicState::setRollRate(double rollRate)
{
    setRollRate(rollRate, m_angleUnits);
}

/**
 * Set the time at which the initial position and velocity are defined for the state
 */
void KinematicState::setTime(double t0)
{
    m_t0 = t0;
}

/**
 * Set the state initial velocity
 */
void KinematicState::setVelocity(const Vector3d &velocity)
{
    setVelocity(velocity.getX(),
                velocity.getY(),
                velocity.getZ());
}

/**
 * Set this object's yaw angle
 */
void KinematicState::setYaw(double yaw)
{
    setYaw(yaw, m_angleUnits);
}

/**
 * Set this object's yaw acceleration
 */
void KinematicState::setYawAcceleration(double yawAcceleration)
{
    setYawAcceleration(yawAcceleration, m_angleUnits);
}

/**
 * Set this object's yaw rate
 */
void KinematicState::setYawRate(double yawRate)
{
    setYawRate(yawRate, m_angleUnits);
}

/**
 * Swap function
 */
void KinematicState::swap(KinematicState &state)
{
    std::swap(m_angleUnits, state.m_angleUnits);
    std::swap(m_t0, state.m_t0);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool KinematicState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "kinematicState") == 0);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto *pTimeNode = pDocument->allocate_node(node_element, "time");
            auto *pTimeString = pDocument->allocate_string(std::to_string(m_t0).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
            pTimeNode->append_node(pDataNode);
            pNode->append_node(pTimeNode);
        }

        if (bSuccess)
        {
            auto *pAngleTypeNode = pDocument->allocate_node(node_element, "angleType");
            auto *pAngleTypeString = pDocument->allocate_string(std::string(m_angleUnits).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pAngleTypeString);
            pAngleTypeNode->append_node(pDataNode);
            pNode->append_node(pAngleTypeNode);
        }
    }

    return bSuccess;
}
#endif
}

}
