#include "angle_unit_type.h"
#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "interpolatedFrameState.h"
#include "projectedFrameState.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include <cstring>
#include <string>
#include <typeinfo>

// file-scoped variables
static constexpr char factoryName[] = "Interpolated";

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
factory(factoryName, (InterpolatedFrameState * (*)(const std::string &))&InterpolatedFrameState::create);

/**
 * Constructor
 * @param name       the name of this reference frame state
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
InterpolatedFrameState::InterpolatedFrameState(const std::string &name,
                                               const AngleUnitType &angleUnits)
: FrameState(name,
             angleUnits)
{

}

/**
 * Copy constructor
 */
InterpolatedFrameState::InterpolatedFrameState(const InterpolatedFrameState &state)
{
    operator = (state);
}

/**
 * Move constructor
 */
InterpolatedFrameState::InterpolatedFrameState(InterpolatedFrameState &&state)
{
    operator = (std::move(state));
}

/**
 * Destructor
 */
InterpolatedFrameState::~InterpolatedFrameState(void)
{

}

/**
 * Copy assignment operator
 */
InterpolatedFrameState &InterpolatedFrameState::operator = (const InterpolatedFrameState &state)
{
    if (&state != this)
    {
        FrameState::operator = (state);

        m_accelerationMap = state.m_accelerationMap;
        m_orientationMap = state.m_orientationMap;
        m_originMap = state.m_originMap;
        m_rotationalAccelerationsMap = state.m_rotationalAccelerationsMap;
        m_rotationalRatesMap = state.m_rotationalRatesMap;
        m_velocityMap = state.m_velocityMap;
    }

    return *this;
}

/**
 * Move assignment operator
 */
InterpolatedFrameState &InterpolatedFrameState::operator = (InterpolatedFrameState &&state)
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
bool InterpolatedFrameState::operator == (const FrameState &state) const
{
    return operator == (dynamic_cast<const InterpolatedFrameState &>(state));
}

/**
 * Equality operator
 */
bool InterpolatedFrameState::operator == (const InterpolatedFrameState &state) const
{
    bool bEqual = FrameState::operator == (state);
    if (bEqual)
    {
        bEqual = (m_accelerationMap == state.m_accelerationMap &&
                  m_orientationMap == state.m_orientationMap &&
                  m_originMap == state.m_originMap &&
                  m_rotationalAccelerations == state.m_rotationalAccelerations &&
                  m_rotationalRatesMap == state.m_rotationalRatesMap &&
                  m_velocityMap == state.m_velocityMap);
    }

    return bEqual;
}

/**
 * Assign the values of this frame state from another frame state
 */
bool InterpolatedFrameState::assign(const FrameState *pState)
{
    auto *pInterpolatedFrameStae = dynamic_cast<const InterpolatedFrameState *>(pState);
    if (pInterpolatedFrameStae != nullptr)
        return assign(pInterpolatedFrameStae);

    auto *pProjectedFrameState = dynamic_cast<const ProjectedFrameState *>(pState);
    if (pProjectedFrameState != nullptr)
        return assign(pProjectedFrameState);

    return false;
}

/**
 * Assign the values of this frame state from another frame state
 */
bool InterpolatedFrameState::assign(const InterpolatedFrameState *pState)
{
    bool bSuccess = FrameState::assign(pState);
    if (bSuccess)
    {
        operator = (*pState);
    }

    return bSuccess;
}

/**
 * Assign the values of this frame state from another frame state
 */
bool InterpolatedFrameState::assign(const ProjectedFrameState *pState)
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
 * clone() function
 */
InterpolatedFrameState *InterpolatedFrameState::clone(void) const
{
    return new InterpolatedFrameState(*this);
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void InterpolatedFrameState::convertAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;

    std::for_each(m_orientationMap.begin(),
                  m_orientationMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.convertAngleUnits(angleUnits); });

    std::for_each(m_rotationalAccelerationsMap.begin(),
                  m_rotationalAccelerationsMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.convertAngleUnits(angleUnits); });

    std::for_each(m_rotationalRatesMap.begin(),
                  m_rotationalRatesMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.convertAngleUnits(angleUnits); });
}

/**
 * create() function
 * @param name the name of this reference frame state
 */
InterpolatedFrameState *InterpolatedFrameState::create(const std::string &name)
{
    return create(name, AngleUnitType::Degrees);
}

/**
 * create() function
 * @param name       the name of this reference frame state
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
InterpolatedFrameState *InterpolatedFrameState::create(const std::string &name,
                                                       const AngleUnitType &angleUnits)
{
    InterpolatedFrameState *pFrameState = nullptr;
    if (!name.empty())
        pFrameState = new InterpolatedFrameState(name, angleUnits);

    return pFrameState;
}

/**
 * Function to deserialize this object's data
 */
std::istream &InterpolatedFrameState::deserialize(std::istream &stream)
{
    FrameState::deserialize(stream);

    if (stream)
    {
        std::size_t numAccelerations;
        stream.read((char *)&numAccelerations, sizeof(std::size_t));
        m_accelerationMap.clear();
        for (std::size_t i = 0; i < numAccelerations; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_accelerationMap[time].deserialize(stream);
        }

        std::size_t numOrientations;
        stream.read((char *)&numOrientations, sizeof(std::size_t));
        m_orientationMap.clear();
        for (std::size_t i = 0; i < numOrientations; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_orientationMap[time].deserialize(stream);
        }

        std::size_t numOrigins;
        stream.read((char *)&numOrigins, sizeof(std::size_t));
        m_originMap.clear();
        for (std::size_t i = 0; i < numOrigins; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_originMap[time].deserialize(stream);
        }

        std::size_t numRotationalAccelerations;
        stream.read((char *)&numRotationalAccelerations, sizeof(std::size_t));
        m_rotationalAccelerationsMap.clear();
        for (std::size_t i = 0; i < numRotationalAccelerations; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_rotationalAccelerationsMap[time].deserialize(stream);
        }

        std::size_t numRotationalRates;
        stream.read((char *)&numRotationalRates, sizeof(std::size_t));
        m_rotationalRatesMap.clear();
        for (std::size_t i = 0; i < numRotationalRates; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_rotationalRatesMap[time].deserialize(stream);
        }

        std::size_t numVelocities;
        stream.read((char *)&numVelocities, sizeof(std::size_t));
        m_velocityMap.clear();
        for (std::size_t i = 0; i < numVelocities; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_velocityMap[time].deserialize(stream);
        }
    }

    return stream;
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 */
void InterpolatedFrameState::getAcceleration(double acceleration[3]) const
{
    interpolate(m_t0, acceleration, m_accelerationMap);
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 */
Vector3d &InterpolatedFrameState::getAcceleration(void)
{
    m_acceleration.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_acceleration, m_accelerationMap);

    return m_acceleration;
}

/**
 * Get the acceleration of the origin of this frame with respect to its parent
 */
Vector3d InterpolatedFrameState::getAcceleration(void) const
{
    Vector3d acceleration;
    interpolate(m_t0, acceleration, m_accelerationMap);

    return acceleration;
}

/**
 * Get the name of this class
 */
std::string InterpolatedFrameState::getClassName(void) const
{
    return "InterpolatedFrameState";
}

/**
 * Get the factory name of this constructible
 */
std::string InterpolatedFrameState::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get the orientation angles of this frame with respect to its parent at the specified time
 */
void InterpolatedFrameState::getOrientation(double &roll,
                                            double &pitch,
                                            double &yaw,
                                            double t) const
{
    Eulers orientation;
    interpolate(t, orientation, m_orientationMap);

    pitch = orientation.getPitch();
    roll = orientation.getRoll();
    yaw = orientation.getYaw();
}

/**
 * Get the orientation angles of this frame with respect to its parent
 */
Eulers &InterpolatedFrameState::getOrientation(void)
{
    m_orientation.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_orientation, m_orientationMap);

    return m_orientation;
}

/**
 * Get the orientation angles of this frame with respect to its parent
 */
Eulers InterpolatedFrameState::getOrientation(void) const
{
    Eulers orientation;
    interpolate(m_t0, orientation, m_orientationMap);

    return orientation;
}

/**
 * Get the position of the origin of this frame with respect to its parent at the specified time
 */
void InterpolatedFrameState::getOrigin(double origin[3],
                                       double t) const
{
    interpolate(t, origin, m_originMap);
}

/**
 * Get the position of the origin of this frame with respect to its parent
 */
Vector3d &InterpolatedFrameState::getOrigin(void)
{
    m_origin.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_origin, m_originMap);

    return m_origin;
}

/**
 * Get the position of the origin of this frame with respect to its parent
 */
Vector3d InterpolatedFrameState::getOrigin(void) const
{
    Vector3d origin;
    interpolate(m_t0, origin, m_originMap);

    return origin;
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedFrameState::getPitch(const AngleUnitType &angleUnits,
                                        double t) const
{
    auto &&orientation = getOrientation(t);

    orientation.convertAngleUnits(angleUnits);

    return orientation.getPitch();
}

/**
 * Get this object's pitch acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double InterpolatedFrameState::getPitchAcceleration(const AngleUnitType &angleUnits) const
{
    auto &&rotationalAccelerations = getRotationalAccelerations();

    rotationalAccelerations.convertAngleUnits(angleUnits);

    return rotationalAccelerations.getPitch();
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedFrameState::getPitchRate(const AngleUnitType &angleUnits,
                                            double t) const
{
    auto &&rotationalRates = getRotationalRates(t);

    rotationalRates.convertAngleUnits(angleUnits);

    return rotationalRates.getPitch();
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedFrameState::getRoll(const AngleUnitType &angleUnits,
                                       double t) const
{
    auto &&orientation = getOrientation(t);

    orientation.convertAngleUnits(angleUnits);

    return orientation.getRoll();
}

/**
 * Get this object's roll acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double InterpolatedFrameState::getRollAcceleration(const AngleUnitType &angleUnits) const
{
    auto &&rotationalAccelerations = getRotationalAccelerations();

    rotationalAccelerations.convertAngleUnits(angleUnits);

    return rotationalAccelerations.getRoll();
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedFrameState::getRollRate(const AngleUnitType &angleUnits,
                                           double t) const
{
    auto &&rotationalRates = getRotationalRates(t);

    rotationalRates.convertAngleUnits(angleUnits);

    return rotationalRates.getRoll();
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
void InterpolatedFrameState::getRotationalAccelerations(double &rollAccel,
                                                        double &pitchAccel,
                                                        double &yawAccel) const
{
    Eulers rotationalAccelerations;
    interpolate(m_t0, rotationalAccelerations, m_rotationalAccelerationsMap);

    pitchAccel = rotationalAccelerations.getPitch();
    rollAccel = rotationalAccelerations.getRoll();
    yawAccel = rotationalAccelerations.getYaw();
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
Eulers &InterpolatedFrameState::getRotationalAccelerations(void)
{
    m_rotationalAccelerations.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_rotationalAccelerations, m_rotationalAccelerationsMap);

    return m_rotationalAccelerations;
}

/**
 * Get the rotational accelerations of this frame with respect to its parent
 */
Eulers InterpolatedFrameState::getRotationalAccelerations(void) const
{
    Eulers rotationalAccelerations;
    interpolate(m_t0, rotationalAccelerations, m_rotationalAccelerationsMap);

    return rotationalAccelerations;
}

/**
 * Get the rotational rates of this frame with respect to its parent at the specified time
 */
void InterpolatedFrameState::getRotationalRates(double &rollRate,
                                                double &pitchRate,
                                                double &yawRate,
                                                double t) const
{
    Eulers rotationalRates;
    interpolate(t, rotationalRates, m_rotationalRatesMap);

    pitchRate = rotationalRates.getPitch();
    rollRate = rotationalRates.getRoll();
    yawRate = rotationalRates.getYaw();
}

/**
 * Get the rotational rates of this frame with respect to its parent
 */
Eulers &InterpolatedFrameState::getRotationalRates(void)
{
    m_rotationalRates.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_rotationalRates, m_rotationalRatesMap);

    return m_rotationalRates;
}

/**
 * Get the rotational rates of this frame with respect to its parent
 */
Eulers InterpolatedFrameState::getRotationalRates(void) const
{
    Eulers rotationalRates;
    interpolate(m_t0, rotationalRates, m_rotationalRatesMap);

    return rotationalRates;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent at the specified time
 */
void InterpolatedFrameState::getVelocity(double velocity[3],
                                         double t) const
{
    interpolate(t, velocity, m_velocityMap);
}

/**
 * Get the velocity of the origin of this frame with respect to its parent
 */
Vector3d &InterpolatedFrameState::getVelocity(void)
{
    m_velocity.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_velocity, m_velocityMap);

    return m_velocity;
}

/**
 * Get the velocity of the origin of this frame with respect to its parent
 */
Vector3d InterpolatedFrameState::getVelocity(void) const
{
    Vector3d velocity;
    interpolate(m_t0, velocity, m_velocityMap);

    return velocity;
}

/**
 * Get this object's yaw angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedFrameState::getYaw(const AngleUnitType &angleUnits,
                                      double t) const
{
    auto &&orientation = getOrientation(t);

    orientation.convertAngleUnits(angleUnits);

    return orientation.getYaw();
}

/**
 * Get this object's yaw acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double InterpolatedFrameState::getYawAcceleration(const AngleUnitType &angleUnits) const
{
    auto &&rotationalAccelerations = getRotationalAccelerations();

    rotationalAccelerations.convertAngleUnits(angleUnits);

    return rotationalAccelerations.getYaw();
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedFrameState::getYawRate(const AngleUnitType &angleUnits,
                                          double t) const
{
    auto &&rotationalRates = getRotationalRates(t);

    rotationalRates.convertAngleUnits(angleUnits);

    return rotationalRates.getYaw();
}

/**
 * Initialization function
 */
bool InterpolatedFrameState::initialize(void)
{
    bool bSuccess = FrameState::initialize();
    if (bSuccess)
    {
        m_accelerationMap.clear();
        m_rotationalAccelerationsMap.clear();
        m_rotationalRatesMap.clear();
        m_orientationMap.clear();
        m_originMap.clear();
        m_velocityMap.clear();
    }

    return bSuccess;
}

/**
 * Initialization function to reset time-derivatives to zero
 */
void InterpolatedFrameState::initializeTimeDerivatives(void)
{
    m_acceleration.set(0.0, 0.0, 0.0);
    std::for_each(m_accelerationMap.begin(),
                  m_accelerationMap.end(),
                  [] (auto &&pair) { pair.second.set(0.0, 0.0, 0.0); });

    m_rotationalAccelerations.set(0.0, 0.0, 0.0);
    std::for_each(m_rotationalAccelerationsMap.begin(),
                  m_rotationalAccelerationsMap.end(),
                  [] (auto &&pair) { pair.second.set(0.0, 0.0, 0.0); });

    m_rotationalRates.set(0.0, 0.0, 0.0);
    std::for_each(m_rotationalRatesMap.begin(),
                  m_rotationalRatesMap.end(),
                  [] (auto &&pair) { pair.second.set(0.0, 0.0, 0.0); });

    m_velocity.set(0.0, 0.0, 0.0);
    std::for_each(m_velocityMap.begin(),
                  m_velocityMap.end(),
                  [] (auto &&pair) { pair.second.set(0.0, 0.0, 0.0); });
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool InterpolatedFrameState::isEqual(const FrameState &state,
                                     double tol) const
{
    return isEqual(dynamic_cast<const InterpolatedFrameState &>(state));
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool InterpolatedFrameState::isEqual(const InterpolatedFrameState &state,
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
bool InterpolatedFrameState::isSpatiallyEqual(const FrameState &state,
                                              double tol) const
{
    return isSpatiallyEqual(dynamic_cast<const InterpolatedFrameState &>(state));
}

/**
 * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
 * (tests all but time)
 */
bool InterpolatedFrameState::isSpatiallyEqual(const InterpolatedFrameState &state,
                                              double tol) const
{
    bool bEqual = (memcmp((void *)this, (void *)&state, sizeof(FrameState)) == 0);
    if (!bEqual)
    {
        auto &&comparator = [&tol] (auto &&left, auto &&right)
                            {
                                return left.second.isEqual(right.second, tol);
                            };

        bEqual = m_accelerationMap.size() == state.m_accelerationMap.size() &&
                 std::equal(m_accelerationMap.cbegin(),
                            m_accelerationMap.cend(),
                            state.m_accelerationMap.begin(),
                            comparator);

        if (bEqual)
        {
            bEqual = m_orientationMap.size() == state.m_orientationMap.size() &&
                     std::equal(m_orientationMap.cbegin(),
                                m_orientationMap.cend(),
                                state.m_orientationMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_originMap.size() == state.m_originMap.size() &&
                     std::equal(m_originMap.cbegin(),
                                m_originMap.cend(),
                                state.m_originMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_rotationalAccelerationsMap.size() == state.m_rotationalAccelerationsMap.size() &&
                     std::equal(m_rotationalAccelerationsMap.cbegin(),
                                m_rotationalAccelerationsMap.cend(),
                                state.m_rotationalAccelerationsMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_rotationalRatesMap.size() == state.m_rotationalRatesMap.size() &&
                     std::equal(m_rotationalRatesMap.cbegin(),
                                m_rotationalRatesMap.cend(),
                                state.m_rotationalRatesMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_velocityMap.size() == state.m_velocityMap.size() &&
                     std::equal(m_velocityMap.cbegin(),
                                m_velocityMap.cend(),
                                state.m_velocityMap.begin(),
                                comparator);
        }
    }

    return bEqual;
}

/**
 * Function to print the contents of this reference frame state
 */
std::ostream &InterpolatedFrameState::print(std::ostream &stream) const
{
    FrameState::print(stream);

    if (stream)
    {
        static const std::vector<std::string> strings = { "Origin vs time:",
                                                          "Velocity vs time:",
                                                          "Acceleration vs time:",
                                                          "Orientation vs time:",
                                                          "Rotational rates vs time:",
                                                          "Rotational accelerations vs time:" };

        stream << std::endl;
        auto &&itStrings = strings.cbegin();
        for (auto &mapEntry : { m_originMap,
                                m_velocityMap,
                                m_accelerationMap })
        {
            stream << *itStrings << std::endl;
            std::for_each(mapEntry.cbegin(),
                          mapEntry.cend(),
                          [&stream] (auto &&pair)
                          {
                              stream << pair.first << ", ";
                              pair.second.print(stream) << std::endl;
                          });

            ++itStrings;
        }

        for (auto &mapEntry : { m_orientationMap,
                                m_rotationalRatesMap,
                                m_rotationalAccelerationsMap })
        {
            stream << *itStrings << std::endl;
            std::for_each(mapEntry.cbegin(),
                          mapEntry.cend(),
                          [&stream] (auto &&pair)
                          {
                              stream << pair.first << ", ";
                              pair.second.print(stream) << std::endl;
                          });

            ++itStrings;
        }
    }

    return stream;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool InterpolatedFrameState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = FrameState::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pOriginNode = pNode->first_node("origin");
        while (pOriginNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pOriginNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_originMap[t].readFromXML(pOriginNode);
            }

            pOriginNode = pOriginNode->next_sibling("origin");
        }

        auto *pVelocityNode = pNode->first_node("velocity");
        while (pVelocityNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pVelocityNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_velocityMap[t].readFromXML(pVelocityNode);
            }

            pVelocityNode = pVelocityNode->next_sibling("velocity");
        }

        auto *pAccelerationNode = pNode->first_node("acceleration");
        while (pAccelerationNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pAccelerationNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_accelerationMap[t].readFromXML(pAccelerationNode);
            }

            pAccelerationNode = pAccelerationNode->next_sibling("acceleration");
        }

        auto *pOrientationNode = pNode->first_node("orientation");
        while (pOrientationNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pOrientationNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_orientationMap[t].readFromXML(pOrientationNode);
            }

            pOrientationNode = pOrientationNode->next_sibling("orientation");
        }

        auto *pRotationalRatesNode = pNode->first_node("rotationalRates");
        while (pRotationalRatesNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pRotationalRatesNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_rotationalRatesMap[t].readFromXML(pRotationalRatesNode);
            }

            pRotationalRatesNode = pRotationalRatesNode->next_sibling("rotationalRates");
        }

        auto *pRotationalAccelerationsNode = pNode->first_node("rotationalAccelerations");
        while (pRotationalAccelerationsNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pRotationalAccelerationsNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_rotationalAccelerationsMap[t].readFromXML(pRotationalAccelerationsNode);
            }

            pRotationalAccelerationsNode = pRotationalAccelerationsNode->
                                           next_sibling("rotationalAccelerations");
        }
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &InterpolatedFrameState::serialize(std::ostream &stream) const
{
    FrameState::serialize(stream);

    if (stream)
    {
        auto numAccelerations = m_accelerationMap.size();
        stream.write((const char *)&numAccelerations, sizeof(std::size_t));
        auto &&itAcceleration = m_accelerationMap.cbegin();
        while (itAcceleration != m_accelerationMap.cend())
        {
            double time = itAcceleration->first;
            stream.write((const char *)&time, sizeof(double));
            itAcceleration->second.serialize(stream);
            ++itAcceleration;
        }

        auto numOrientations = m_orientationMap.size();
        stream.write((const char *)&numOrientations, sizeof(std::size_t));
        auto &&itOrientation = m_orientationMap.cbegin();
        while (itOrientation != m_orientationMap.cend())
        {
            double time = itOrientation->first;
            stream.write((const char *)&time, sizeof(double));
            itOrientation->second.serialize(stream);
            ++itOrientation;
        }

        auto numOrigins = m_originMap.size();
        stream.write((const char *)&numOrigins, sizeof(std::size_t));
        auto &&itOrigin = m_originMap.cbegin();
        while (itOrigin != m_originMap.cend())
        {
            double time = itOrigin->first;
            stream.write((const char *)&time, sizeof(double));
            itOrigin->second.serialize(stream);
            ++itOrigin;
        }

        auto numRotationalAccelerations = m_rotationalAccelerationsMap.size();
        stream.write((const char *)&numRotationalAccelerations, sizeof(std::size_t));
        auto &&itRotationalAccelerations = m_rotationalAccelerationsMap.cbegin();
        while (itRotationalAccelerations != m_rotationalAccelerationsMap.cend())
        {
            double time = itRotationalAccelerations->first;
            stream.write((const char *)&time, sizeof(double));
            itRotationalAccelerations->second.serialize(stream);
            ++itRotationalAccelerations;
        }

        auto numRotationalRates = m_rotationalRatesMap.size();
        stream.write((const char *)&numRotationalRates, sizeof(std::size_t));
        auto &&itRotationalRates = m_rotationalRatesMap.cbegin();
        while (itRotationalRates != m_rotationalRatesMap.cend())
        {
            double time = itRotationalRates->first;
            stream.write((const char *)&time, sizeof(double));
            itRotationalRates->second.serialize(stream);
            ++itRotationalRates;
        }

        auto numVelocities = m_velocityMap.size();
        stream.write((const char *)&numVelocities, sizeof(std::size_t));
        auto &&itVelocity = m_velocityMap.cbegin();
        while (itVelocity != m_velocityMap.cend())
        {
            double time = itVelocity->first;
            stream.write((const char *)&time, sizeof(double));
            itVelocity->second.serialize(stream);
            ++itVelocity;
        }
    }

    return stream;
}

/**
 * Set the acceleration of the origin of this frame with respect to its parent
 */
void InterpolatedFrameState::setAcceleration(double xAcceleration,
                                             double yAcceleration,
                                             double zAcceleration)
{
    m_accelerationMap[m_t0].set(xAcceleration,
                                yAcceleration,
                                zAcceleration);
    if (m_accelerationMap.size() > 5)
        m_accelerationMap.erase(m_accelerationMap.begin());
}

/**
 * Set angle units (Degrees or Radians)
 */
void InterpolatedFrameState::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;

    std::for_each(m_orientationMap.begin(),
                  m_orientationMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.setAngleUnits(angleUnits); });

    std::for_each(m_rotationalAccelerationsMap.begin(),
                  m_rotationalAccelerationsMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.setAngleUnits(angleUnits); });

    std::for_each(m_rotationalRatesMap.begin(),
                  m_rotationalRatesMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.setAngleUnits(angleUnits); });
}

/**
 * Set the orientation angles of this frame with respect to its parent
 */
void InterpolatedFrameState::setOrientation(const Eulers &orientation)
{
    setOrientation(orientation[EulerAxisType::Roll],
                   orientation[EulerAxisType::Pitch],
                   orientation[EulerAxisType::Yaw]);

    m_orientationMap[m_t0].convertAngleUnits(m_angleUnits);
    if (m_orientationMap.size() > 5)
        m_orientationMap.erase(m_orientationMap.begin());
}

/**
 * Set the orientation angles of this frame with respect to its parent
 */
void InterpolatedFrameState::setOrientation(double roll,
                                            double pitch,
                                            double yaw)
{
    auto &&orientation = m_orientationMap[m_t0];
    orientation.setPitch(pitch);
    orientation.setRoll(roll);
    orientation.setYaw(yaw);
    if (m_orientationMap.size() > 5)
        m_orientationMap.erase(m_orientationMap.begin());
}

/**
 * Set the position of the origin of this frame with respect to its parent
 */
void InterpolatedFrameState::setOrigin(double xPosition,
                                       double yPosition,
                                       double zPosition)
{
    m_originMap[m_t0].set(xPosition,
                          yPosition,
                          zPosition);
    if (m_originMap.size() > 5)
        m_originMap.erase(m_originMap.begin());
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setPitch(double pitch,
                                      const AngleUnitType &angleUnits)
{
    auto &&orientation = m_orientationMap[m_t0];
    orientation.setAngleUnits(m_angleUnits);
    orientation.setPitch(pitch, angleUnits);
    if (m_orientationMap.size() > 5)
        m_orientationMap.erase(m_orientationMap.begin());
}

/**
 * Set this object's pitch acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setPitchAcceleration(double pitchAcceleration,
                                                  const AngleUnitType &angleUnits)
{
    auto &&rotationalAccelerations = m_rotationalAccelerationsMap[m_t0];
    rotationalAccelerations.setAngleUnits(m_angleUnits);
    rotationalAccelerations.setPitch(pitchAcceleration, angleUnits);
    if (m_rotationalAccelerationsMap.size() > 5)
        m_rotationalAccelerationsMap.erase(m_rotationalAccelerationsMap.begin());
}

/**
 * Set this object's pitch rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setPitchRate(double pitchRate,
                                          const AngleUnitType &angleUnits)
{
    auto &&rotationalRates = m_rotationalRatesMap[m_t0];
    rotationalRates.setAngleUnits(m_angleUnits);
    rotationalRates.setPitch(pitchRate, angleUnits);
    if (m_rotationalRatesMap.size() > 5)
        m_rotationalRatesMap.erase(m_rotationalRatesMap.begin());
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setRoll(double roll,
                                     const AngleUnitType &angleUnits)
{
    auto &&orientation = m_orientationMap[m_t0];
    orientation.setAngleUnits(m_angleUnits);
    orientation.setRoll(roll, angleUnits);
    if (m_orientationMap.size() > 5)
        m_orientationMap.erase(m_orientationMap.begin());
}

/**
 * Set this object's roll acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setRollAcceleration(double rollAcceleration,
                                                 const AngleUnitType &angleUnits)
{
    auto &&rotationalAccelerations = m_rotationalAccelerationsMap[m_t0];
    rotationalAccelerations.setAngleUnits(m_angleUnits);
    rotationalAccelerations.setRoll(rollAcceleration, angleUnits);
    if (m_rotationalAccelerationsMap.size() > 5)
        m_rotationalAccelerationsMap.erase(m_rotationalAccelerationsMap.begin());
}

/**
 * Set this object's roll rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setRollRate(double rollRate,
                                         const AngleUnitType &angleUnits)
{
    auto &&rotationalRates = m_rotationalRatesMap[m_t0];
    rotationalRates.setAngleUnits(m_angleUnits);
    rotationalRates.setRoll(rollRate, angleUnits);
    if (m_rotationalRatesMap.size() > 5)
        m_rotationalRatesMap.erase(m_rotationalRatesMap.begin());
}

/**
 * Set the rotational accelerations of this frame with respect to its parent
 */
void InterpolatedFrameState::setRotationalAccelerations(const Eulers &rotationalAccelerations)
{
    setRotationalAccelerations(rotationalAccelerations[EulerAccelerationAxisType::Roll],
                               rotationalAccelerations[EulerAccelerationAxisType::Pitch],
                               rotationalAccelerations[EulerAccelerationAxisType::Yaw]);

    m_rotationalAccelerationsMap[m_t0].convertAngleUnits(m_angleUnits);
    if (m_rotationalAccelerationsMap.size() > 5)
        m_rotationalAccelerationsMap.erase(m_rotationalAccelerationsMap.begin());
}

/**
 * Set the rotational accelerations of this frame with respect to its parent
 */
void InterpolatedFrameState::setRotationalAccelerations(double rollAcceleration,
                                                        double pitchAcceleration,
                                                        double yawAcceleration)
{
    auto &&rotationalAccelerations = m_rotationalAccelerationsMap[m_t0];
    rotationalAccelerations.setPitch(pitchAcceleration);
    rotationalAccelerations.setRoll(rollAcceleration);
    rotationalAccelerations.setYaw(yawAcceleration);
    if (m_rotationalAccelerationsMap.size() > 5)
        m_rotationalAccelerationsMap.erase(m_rotationalAccelerationsMap.begin());
}

/**
 * Set the rotational rates of this frame with respect to its parent
 */
void InterpolatedFrameState::setRotationalRates(const Eulers &rotationalRates)
{
    setRotationalRates(rotationalRates[EulerRateAxisType::Roll],
                       rotationalRates[EulerRateAxisType::Pitch],
                       rotationalRates[EulerRateAxisType::Yaw]);

    m_rotationalRatesMap[m_t0].convertAngleUnits(m_angleUnits);
    if (m_rotationalRatesMap.size() > 5)
        m_rotationalRatesMap.erase(m_rotationalRatesMap.begin());
}

/**
 * Set the rotational rates of this frame with respect to its parent
 */
void InterpolatedFrameState::setRotationalRates(double rollRate,
                                                double pitchRate,
                                                double yawRate)
{
    auto &&rotationalRates = m_rotationalRatesMap[m_t0];
    rotationalRates.setPitch(pitchRate);
    rotationalRates.setRoll(rollRate);
    rotationalRates.setYaw(yawRate);
    if (m_rotationalRatesMap.size() > 5)
        m_rotationalRatesMap.erase(m_rotationalRatesMap.begin());
}

/**
 * Set the time at which this frame is currently defined
 */
void InterpolatedFrameState::setTime(double t0)
{
#if 0
    auto &&itAcceleration = m_accelerationMap.find(t0);
    if (itAcceleration != m_accelerationMap.cend() && t0 != m_t0)
    {
        m_accelerationMap[t0] = itAcceleration->second;
        m_accelerationMap.erase(itAcceleration);
    }

    auto &&itOrientation = m_orientationMap.find(t0);
    if (itOrientation != m_orientationMap.cend() && t0 != m_t0)
    {
        m_orientationMap[t0] = itOrientation->second;
        m_orientationMap.erase(itOrientation);
    }

    auto &&itOrigin = m_originMap.find(t0);
    if (itOrigin != m_originMap.cend() && t0 != m_t0)
    {
        m_originMap[t0] = itOrigin->second;
        m_originMap.erase(itOrigin);
    }

    auto &&itRotationalAccelerations = m_rotationalAccelerationsMap.find(t0);
    if (itRotationalAccelerations != m_rotationalAccelerationsMap.cend() && t0 != m_t0)
    {
        m_rotationalAccelerationsMap[t0] = itRotationalAccelerations->second;
        m_rotationalAccelerationsMap.erase(itRotationalAccelerations);
    }

    auto &&itRotationalRates = m_rotationalRatesMap.find(t0);
    if (itRotationalRates != m_rotationalRatesMap.cend() && t0 != m_t0)
    {
        m_rotationalRatesMap[t0] = itRotationalRates->second;
        m_rotationalRatesMap.erase(itRotationalRates);
    }

    auto &&itVelocity = m_velocityMap.find(t0);
    if (itVelocity != m_velocityMap.cend() && t0 != m_t0)
    {
        m_velocityMap[t0] = itVelocity->second;
        m_velocityMap.erase(itVelocity);
    }
#endif
    FrameState::setTime(t0);
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 */
void InterpolatedFrameState::setVelocity(double xVelocity,
                                         double yVelocity,
                                         double zVelocity)
{
    m_velocityMap[m_t0].set(xVelocity,
                            yVelocity,
                            zVelocity);
    if (m_velocityMap.size() > 5)
        m_velocityMap.erase(m_velocityMap.begin());
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setYaw(double yaw,
                                    const AngleUnitType &angleUnits)
{
    auto &&orientation = m_orientationMap[m_t0];
    orientation.setAngleUnits(m_angleUnits);
    orientation.setYaw(yaw, angleUnits);
    if (m_orientationMap.size() > 5)
        m_orientationMap.erase(m_orientationMap.begin());
}

/**
 * Set this object's yaw acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setYawAcceleration(double yawAcceleration,
                                                const AngleUnitType &angleUnits)
{
    auto &&rotationalAccelerations = m_rotationalAccelerationsMap[m_t0];
    rotationalAccelerations.setAngleUnits(m_angleUnits);
    rotationalAccelerations.setYaw(yawAcceleration, angleUnits);
    if (m_rotationalAccelerationsMap.size() > 5)
        m_rotationalAccelerationsMap.erase(m_rotationalAccelerationsMap.begin());
}

/**
 * Set this object's yaw rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedFrameState::setYawRate(double yawRate,
                                        const AngleUnitType &angleUnits)
{
    auto &&rotationalRates = m_rotationalRatesMap[m_t0];
    rotationalRates.setAngleUnits(m_angleUnits);
    rotationalRates.setYaw(yawRate, angleUnits);
    if (m_rotationalRatesMap.size() > 5)
        m_rotationalRatesMap.erase(m_rotationalRatesMap.begin());
}

/**
 * Swap function
 */
void InterpolatedFrameState::swap(InterpolatedFrameState &state)
{
    FrameState::swap(state);

    m_accelerationMap.swap(state.m_accelerationMap);
    m_orientationMap.swap(state.m_orientationMap);
    m_originMap.swap(state.m_originMap);
    m_rotationalAccelerations.swap(state.m_rotationalAccelerations);
    m_rotationalRatesMap.swap(state.m_rotationalRatesMap);
    m_velocityMap.swap(state.m_velocityMap);
}

/**
 * Update this reference frame state to the specified time. This function will update all quantities within the
 * frame state that vary with time, including the time tag.
 * @param t             the time value (s)
 * @param timeReference enumeration specifying the update time is an absolute time or a delta time (with
 *                      respect to current time)
 */
void InterpolatedFrameState::update(double t,
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
bool InterpolatedFrameState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = FrameState::writeToXML(pNode);
    if (bSuccess)
    {
        auto *pDocument = pNode->document();
        bSuccess = (pDocument != nullptr);
        if (bSuccess)
        {
            auto &&itAcceleration = m_accelerationMap.cbegin();
            while (itAcceleration != m_accelerationMap.cend())
            {
                double time = itAcceleration->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pAccelerationNode = pDocument->allocate_node(node_element, "acceleration");
                bSuccess = m_acceleration.writeToXML(pAccelerationNode);
                if (bSuccess)
                    pAccelerationNode->append_node(pTimeNode);

                pNode->append_node(pAccelerationNode);
                ++itAcceleration;
            }

            auto &&itOrientation = m_orientationMap.cbegin();
            while (itOrientation != m_orientationMap.cend())
            {
                double time = itOrientation->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pOrientationNode = pDocument->allocate_node(node_element, "orientation");
                bSuccess = m_orientation.writeToXML(pOrientationNode);
                if (bSuccess)
                    pOrientationNode->append_node(pTimeNode);

                pNode->append_node(pOrientationNode);
                ++itOrientation;
            }

            auto &&itOrigin = m_originMap.cbegin();
            while (itOrigin != m_originMap.cend())
            {
                double time = itOrigin->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pOriginNode = pDocument->allocate_node(node_element, "origin");
                bSuccess = m_origin.writeToXML(pOriginNode);
                if (bSuccess)
                    pOriginNode->append_node(pTimeNode);

                pNode->append_node(pOriginNode);
                ++itOrigin;
            }

            auto &&itRotationalAccelerations = m_rotationalAccelerationsMap.cbegin();
            while (itRotationalAccelerations != m_rotationalAccelerationsMap.cend())
            {
                double time = itRotationalAccelerations->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pRotationalAccelerationsNode = pDocument->allocate_node(node_element,
                                                                              "rotationalAccelerations");
                bSuccess = m_rotationalAccelerations.writeToXML(pRotationalAccelerationsNode);
                if (bSuccess)
                    pRotationalAccelerationsNode->append_node(pTimeNode);

                pNode->append_node(pRotationalAccelerationsNode);
                ++itRotationalAccelerations;
            }

            auto &&itRotationalRates = m_rotationalRatesMap.cbegin();
            while (itRotationalRates != m_rotationalRatesMap.cend())
            {
                double time = itRotationalRates->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pRotationalRatesNode = pDocument->allocate_node(node_element, "rotationalRates");
                bSuccess = m_rotationalRates.writeToXML(pRotationalRatesNode);
                if (bSuccess)
                    pRotationalRatesNode->append_node(pTimeNode);

                pNode->append_node(pRotationalRatesNode);
                ++itRotationalRates;
            }

            auto &&itVelocity = m_velocityMap.cbegin();
            while (itVelocity != m_velocityMap.cend())
            {
                double time = itVelocity->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pVelocityNode = pDocument->allocate_node(node_element, "velocity");
                bSuccess = m_velocity.writeToXML(pVelocityNode);
                if (bSuccess)
                    pVelocityNode->append_node(pTimeNode);

                pNode->append_node(pVelocityNode);
                ++itVelocity;
            }
        }
    }

    return bSuccess;
}
#endif
}

}
