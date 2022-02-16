#include "acceleration_axis_type.h"
#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "frameState.h"
#include "position_axis_type.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "velocity_axis_type.h"
#include <cmath>
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
 * @param name       the name of this reference frame state
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
FrameState::FrameState(const std::string &name,
                       const AngleUnitType &angleUnits)
: m_angleUnits(angleUnits),
  m_name(name),
  m_t0(0.0)
{

}

/**
 * Copy constructor
 */
FrameState::FrameState(const FrameState &state)
{
    operator = (state);
}

/**
 * Move constructor
 */
FrameState::FrameState(FrameState &&state)
{
    operator = (std::move(state));
}

/**
 * Destructor
 */
FrameState::~FrameState(void)
{

}

/**
 * Copy assignment operator
 */
FrameState &FrameState::operator = (const FrameState &state)
{
    if (&state != this)
    {
        m_angleUnits = state.m_angleUnits;
        m_name = state.m_name;
        m_t0 = state.m_t0;
    }

    return *this;
}

/**
 * Move assignment operator
 */
FrameState &FrameState::operator = (FrameState &&state)
{
    if (&state != this)
    {
        state.swap(*this);
    }

    return *this;
}

/**
 * Equality operator overload
 */
bool FrameState::operator == (const FrameState &state) const
{
    return isEqual(state, 0.0);
}

/**
 * Equality operator overload
 */
bool FrameState::operator != (const FrameState &state) const
{
    return !operator == (state);
}

/**
 * Output stream operator
 */
std::ostream &operator << (std::ostream &stream,
                           const FrameState &state)
{
    return state.print(stream);
}

/**
 * Assign the values of this frame state from another frame state
 */
bool FrameState::assign(const FrameState *pState)
{
    bool bSuccess = (pState != nullptr);
    if (bSuccess)
    {
        setAngleUnits(pState->getAngleUnits());
        setName(pState->getName());
        setTime(pState->getTime());
    }

    return bSuccess;
}

/**
 * Function to deserialize this object's data
 */
std::istream &FrameState::deserialize(std::istream &stream)
{
    if (stream)
    {
        stream.read((char *)&m_angleUnits, sizeof(int));
        std::getline(stream, m_name, '\0');
        stream.read((char *)&m_t0, sizeof(double));
    }

    return stream;
}

/**
 * Get angle units (Degrees or Radians)
 */
AngleUnitType FrameState::getAngleUnits(void) const
{
    return m_angleUnits;
}

/**
 * Get the name of this class
 */
std::string FrameState::getClassName(void) const
{
    return "FrameState";
}

/**
 * Get the name of this reference frame state
 */
std::string FrameState::getName(void) const
{
    return m_name;
}

/**
 * Get the orientation angles of this frame with respect to its parent
 */
void FrameState::getOrientation(double &roll,
                                double &pitch,
                                double &yaw) const
{
    getOrientation(roll, pitch, yaw, m_t0);
}

/**
 * Get the orientation angles of this frame with respect to its parent
 */
void FrameState::getOrientation(double orientation[3]) const
{
    getOrientation(orientation, m_t0);
}

/**
 * Get the orientation angles of this frame with respect to its parent at the specified time
 */
void FrameState::getOrientation(double orientation[3],
                                double t) const
{
    getOrientation(orientation[EulerAxisType::Roll],
                   orientation[EulerAxisType::Pitch],
                   orientation[EulerAxisType::Yaw], t);
}

/**
 * Get the orientation angles of this frame with respect to its parent at the specified time
 */
Eulers FrameState::getOrientation(double t) const
{
    Eulers orientation(m_angleUnits);

    getOrientation(orientation, t);

    return orientation;
}

/**
 * Get the position of the origin of this frame with respect to its parent at the specified time
 */
void FrameState::getOrigin(double origin[3]) const
{
    getOrigin(origin, m_t0);
}

/**
 * Get the position of the origin of this frame with respect to its parent at the specified time
 */
Vector3d FrameState::getOrigin(double t) const
{
    Vector3d origin;

    getOrigin(origin, t);

    return origin;
}

/**
 * Get this object's pitch angle
 */
double FrameState::getPitch(void) const
{
    return getPitch(m_angleUnits, m_t0);
}

/**
 * Get this object's pitch angle
 * @param t the time at which the return value will be computed
 */
double FrameState::getPitch(double t) const
{
    return getPitch(m_angleUnits, t);
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double FrameState::getPitch(const AngleUnitType &angleUnits) const
{
    return getPitch(angleUnits, m_t0);
}

/**
 * Get this object's pitch acceleration
 */
double FrameState::getPitchAcceleration(void) const
{
    return getPitchAcceleration(m_angleUnits);
}

/**
 * Get this object's pitch rate
 */
double FrameState::getPitchRate(void) const
{
    return getPitchRate(m_angleUnits, m_t0);
}

/**
 * Get this object's pitch rate
 * @param t the time at which the return value will be computed
 */
double FrameState::getPitchRate(double t) const
{
    return getPitchRate(m_angleUnits, t);
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double FrameState::getPitchRate(const AngleUnitType &angleUnits) const
{
    return getPitchRate(angleUnits, m_t0);
}

/**
 * Get this object's roll angle
 */
double FrameState::getRoll(void) const
{
    return getRoll(m_angleUnits, m_t0);
}

/**
 * Get this object's roll angle
 * @param t the time at which the return value will be computed
 */
double FrameState::getRoll(double t) const
{
    return getRoll(m_angleUnits, t);
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double FrameState::getRoll(const AngleUnitType &angleUnits) const
{
    return getRoll(angleUnits, m_t0);
}

/**
 * Get this object's roll acceleration
 */
double FrameState::getRollAcceleration(void) const
{
    return getRollAcceleration(m_angleUnits);
}

/**
 * Get this object's roll rate
 */
double FrameState::getRollRate(void) const
{
    return getRollRate(m_angleUnits, m_t0);
}

/**
 * Get this object's roll rate
 * @param t the time at which the return value will be computed
 */
double FrameState::getRollRate(double t) const
{
    return getRollRate(m_angleUnits, t);
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double FrameState::getRollRate(const AngleUnitType &angleUnits) const
{
    return getRollRate(angleUnits, m_t0);
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
void FrameState::getRotationalAccelerations(double rotationalAccelerations[3]) const
{
    getRotationalAccelerations(rotationalAccelerations[EulerAccelerationAxisType::Roll],
                               rotationalAccelerations[EulerAccelerationAxisType::Pitch],
                               rotationalAccelerations[EulerAccelerationAxisType::Yaw]);
}

/**
 * Get the rotational rates of this frame with respect to its parent
 */
void FrameState::getRotationalRates(double &rollRate,
                                    double &pitchRate,
                                    double &yawRate) const
{
    getRotationalRates(rollRate,
                       pitchRate,
                       yawRate,
                       m_t0);
}

/**
 * Get the rotational rates of this frame with respect to its parent
 */
void FrameState::getRotationalRates(double rotationalRates[3]) const
{
    getRotationalRates(rotationalRates, m_t0);
}

/**
 * Get the rotational rates of this frame with respect to its parent at the specified time
 */
void FrameState::getRotationalRates(double rotationalRates[3],
                                    double t) const
{
    getRotationalRates(rotationalRates[EulerRateAxisType::Roll],
                       rotationalRates[EulerRateAxisType::Pitch],
                       rotationalRates[EulerRateAxisType::Yaw], t);
}

/**
 * Get the rotational rates of this frame with respect to its parent at the specified time
 */
Eulers FrameState::getRotationalRates(double t) const
{
    Eulers rotationalRates(m_angleUnits);

    getRotationalRates(rotationalRates, t);

    return rotationalRates;
}

/**
 * Get the time at which this frame is currently defined
 */
double FrameState::getTime(void) const
{
    return m_t0;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent
 */
void FrameState::getVelocity(double velocity[3]) const
{
    getVelocity(velocity, m_t0);
}

/**
 * Get the velocity of the origin of this frame with respect to its parent at the specified time
 */
Vector3d FrameState::getVelocity(double t) const
{
    Vector3d velocity;

    getVelocity(velocity, t);

    return velocity;
}

/**
 * Get this object's yaw angle
 */
double FrameState::getYaw(void) const
{
    return getYaw(m_angleUnits, m_t0);
}

/**
 * Get this object's yaw angle
 * @param t the time at which the return value will be computed
 */
double FrameState::getYaw(double t) const
{
    return getYaw(m_angleUnits, t);
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double FrameState::getYaw(const AngleUnitType &angleUnits) const
{
    return getYaw(angleUnits, m_t0);
}

/**
 * Get this object's yaw acceleration
 */
double FrameState::getYawAcceleration(void) const
{
    return getYawAcceleration(m_angleUnits);
}

/**
 * Get this object's yaw rate
 */
double FrameState::getYawRate(void) const
{
    return getYawRate(m_angleUnits, m_t0);
}

/**
 * Get this object's yaw rate
 * @param t the time at which the return value will be computed
 */
double FrameState::getYawRate(double t) const
{
    return getYawRate(m_angleUnits, t);
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double FrameState::getYawRate(const AngleUnitType &angleUnits) const
{
    return getYawRate(angleUnits, m_t0);
}

/**
 * Initialization function
 */
bool FrameState::initialize(void)
{
    m_t0 = 0.0;

    initializeTimeDerivatives();

    return true;
}

/**
 * Determines whether or not two frame states are equivalent within the specified tolerance
 */
bool FrameState::isEqual(const FrameState &state,
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
std::ostream &FrameState::print(std::ostream &stream) const
{
    if (stream)
    {
        stream << " Time (s): { " << m_t0 << " }" << std::endl
               << " Angle type: { " << std::string(m_angleUnits) << " }" << std::endl;
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool FrameState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "frameState") == 0);
    if (bSuccess)
    {
        auto *pAngleUnitsNode = pNode->first_node("angleType");
        if (pAngleUnitsNode != nullptr)
            m_angleUnits = pAngleUnitsNode->value();

        auto *pNameNode = pNode->first_node("name");
        if (pNameNode != nullptr)
            m_name = pNameNode->value();
        else
            m_name = DEFAULT_FRAME_STATE;

        auto *pTimeNode = pNode->first_node("time");
        if (pTimeNode != nullptr)
            m_t0 = std::stod(pTimeNode->value());
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &FrameState::serialize(std::ostream &stream) const
{
    if (stream)
    {
        stream.write((const char *)&m_angleUnits, sizeof(int));
        stream << m_name << '\0';
        stream.write((const char *)&m_t0, sizeof(double));
    }

    return stream;
}

/**
 * Set the acceleration of the origin of this frame with respect to its parent
 */
void FrameState::setAcceleration(const Vector3d &acceleration)
{
    setAcceleration(acceleration.getX(),
                    acceleration.getY(),
                    acceleration.getZ());
}

/**
 * Set the name of this reference frame state
 */
void FrameState::setName(const std::string &name)
{
    m_name = name;
}

/**
 * Set the position of the origin of this frame with respect to its parent
 */
void FrameState::setOrigin(const Vector3d &origin)
{
    setOrigin(origin.getX(),
              origin.getY(),
              origin.getZ());
}

/**
 * Set this object's pitch angle
 */
void FrameState::setPitch(double pitch)
{
    setPitch(pitch, m_angleUnits);
}

/**
 * Set this object's pitch acceleration
 */
void FrameState::setPitchAcceleration(double pitchAcceleration)
{
    setPitchAcceleration(pitchAcceleration, m_angleUnits);
}

/**
 * Set this object's pitch rate
 */
void FrameState::setPitchRate(double pitchRate)
{
    setPitchRate(pitchRate, m_angleUnits);
}

/**
 * Set this object's roll angle
 */
void FrameState::setRoll(double roll)
{
    setRoll(roll, m_angleUnits);
}

/**
 * Set this object's roll acceleration
 */
void FrameState::setRollAcceleration(double rollAcceleration)
{
    setRollAcceleration(rollAcceleration, m_angleUnits);
}

/**
 * Set this object's roll rate
 */
void FrameState::setRollRate(double rollRate)
{
    setRollRate(rollRate, m_angleUnits);
}

/**
 * Set the time at which this frame is currently defined
 */
void FrameState::setTime(double t0)
{
    m_t0 = t0;
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 */
void FrameState::setVelocity(const Vector3d &velocity)
{
    setVelocity(velocity.getX(),
                velocity.getY(),
                velocity.getZ());
}

/**
 * Set this object's yaw angle
 */
void FrameState::setYaw(double yaw)
{
    setYaw(yaw, m_angleUnits);
}

/**
 * Set this object's yaw acceleration
 */
void FrameState::setYawAcceleration(double yawAcceleration)
{
    setYawAcceleration(yawAcceleration, m_angleUnits);
}

/**
 * Set this object's yaw rate
 */
void FrameState::setYawRate(double yawRate)
{
    setYawRate(yawRate, m_angleUnits);
}

/**
 * Swap function
 */
void FrameState::swap(FrameState &state)
{
    std::swap(m_angleUnits, state.m_angleUnits);
    std::swap(m_name, state.m_name);
    std::swap(m_t0, state.m_t0);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool FrameState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "frameState") == 0);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto *pAngleTypeNode = pDocument->allocate_node(node_element, "angleType");
            auto *pAngleTypeString = pDocument->allocate_string(std::string(m_angleUnits).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pAngleTypeString);
            pAngleTypeNode->append_node(pDataNode);
            pNode->append_node(pAngleTypeNode);
        }

        if (bSuccess)
        {
            auto *pNameNode = pDocument->allocate_node(node_element, "name");
            auto *pDataNode = pDocument->allocate_node(node_data, m_name.c_str());
            pNameNode->append_node(pDataNode);
            pNode->append_node(pNameNode);
        }

        if (bSuccess)
        {
            auto *pTimeNode = pDocument->allocate_node(node_element, "time");
            auto *pTimeString = pDocument->allocate_string(std::to_string(m_t0).c_str());
            auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
            pTimeNode->append_node(pDataNode);
            pNode->append_node(pTimeNode);
        }
    }

    return bSuccess;
}
#endif
}

}
