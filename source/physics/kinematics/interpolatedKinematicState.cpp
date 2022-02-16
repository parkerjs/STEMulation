#include "angle_unit_type.h"
#include "euler_acceleration_axis_type.h"
#include "euler_rate_axis_type.h"
#include "interpolatedKinematicState.h"
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
static FactoryRegistrar<KinematicState>
factory(factoryName, &InterpolatedKinematicState::create);

/**
 * Constructor
 */
InterpolatedKinematicState::InterpolatedKinematicState(void)
: InterpolatedKinematicState(AngleUnitType::Degrees)
{

}

/**
 * Constructor
 * @param angleUnits the units of this object's Euler angles, degrees or Radians
 */
InterpolatedKinematicState::InterpolatedKinematicState(const AngleUnitType &angleUnits)
{
    setAngleUnits(angleUnits);
}

/**
 * Copy constructor
 */
InterpolatedKinematicState::InterpolatedKinematicState(const InterpolatedKinematicState &state)
{
    operator = (state);
}

/**
 * Move constructor
 */
InterpolatedKinematicState::InterpolatedKinematicState(InterpolatedKinematicState &&state)
{
    operator = (std::move(state));
}

/**
 * Destructor
 */
InterpolatedKinematicState::~InterpolatedKinematicState(void)
{

}

/**
 * Copy assignment operator
 */
InterpolatedKinematicState &InterpolatedKinematicState::operator = (const InterpolatedKinematicState &state)
{
    if (&state != this)
    {
        KinematicState::operator = (state);

        m_accelerationMap = state.m_accelerationMap;
        m_eulerAccelerationsMap = state.m_eulerAccelerationsMap;
        m_eulerRatesMap = state.m_eulerRatesMap;
        m_eulersMap = state.m_eulersMap;
        m_positionMap = state.m_positionMap;
        m_velocityMap = state.m_velocityMap;
    }

    return *this;
}

/**
 * Move assignment operator
 */
InterpolatedKinematicState &InterpolatedKinematicState::operator = (InterpolatedKinematicState &&state)
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
bool InterpolatedKinematicState::operator == (const KinematicState &state) const
{
    return operator == (dynamic_cast<const InterpolatedKinematicState &>(state));
}

/**
 * Equality operator
 */
bool InterpolatedKinematicState::operator == (const InterpolatedKinematicState &state) const
{
    bool bEqual = KinematicState::operator == (state);
    if (bEqual)
    {
        bEqual = (m_accelerationMap == state.m_accelerationMap &&
                  m_eulerAccelerationsMap == state.m_eulerAccelerationsMap &&
                  m_eulerRatesMap == state.m_eulerRatesMap &&
                  m_eulersMap == state.m_eulersMap &&
                  m_positionMap == state.m_positionMap &&
                  m_velocityMap == state.m_velocityMap);
    }

    return bEqual;
}

/**
 * Assign the values of this kinematic state from another kinematic state
 */
bool InterpolatedKinematicState::assign(const KinematicState *pState)
{
    return assign(dynamic_cast<const InterpolatedKinematicState *>(pState));
}

/**
 * Assign the values of this kinematic state from another kinematic state
 */
bool InterpolatedKinematicState::assign(const InterpolatedKinematicState *pState)
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
InterpolatedKinematicState *InterpolatedKinematicState::clone(void) const
{
    return new InterpolatedKinematicState(*this);
}

/**
 * Convert the units of this object's angular quantities (Degrees or Radians)
 */
void InterpolatedKinematicState::convertAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;

    std::for_each(m_eulerAccelerationsMap.begin(),
                  m_eulerAccelerationsMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.convertAngleUnits(angleUnits); });

    std::for_each(m_eulerRatesMap.begin(),
                  m_eulerRatesMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.convertAngleUnits(angleUnits); });

    std::for_each(m_eulersMap.begin(),
                  m_eulersMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.convertAngleUnits(angleUnits); });
}

/**
 * create() function
 * @param angleUnits the units of this object's Euler angles, degrees or Radians
 */
InterpolatedKinematicState *InterpolatedKinematicState::create(const AngleUnitType &angleUnits)
{
    return new InterpolatedKinematicState(angleUnits);
}

/**
 * Function to deserialize this object's data
 */
std::istream &InterpolatedKinematicState::deserialize(std::istream &stream)
{
    KinematicState::deserialize(stream);

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

        std::size_t numEulerAccelerations;
        stream.read((char *)&numEulerAccelerations, sizeof(std::size_t));
        m_eulerAccelerationsMap.clear();
        for (std::size_t i = 0; i < numEulerAccelerations; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_eulerAccelerationsMap[time].deserialize(stream);
        }

        std::size_t numEulerRates;
        stream.read((char *)&numEulerRates, sizeof(std::size_t));
        m_eulerRatesMap.clear();
        for (std::size_t i = 0; i < numEulerRates; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_eulerRatesMap[time].deserialize(stream);
        }

        std::size_t numEulers;
        stream.read((char *)&numEulers, sizeof(std::size_t));
        m_eulersMap.clear();
        for (std::size_t i = 0; i < numEulers; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_eulersMap[time].deserialize(stream);
        }

        std::size_t numPositions;
        stream.read((char *)&numPositions, sizeof(std::size_t));
        m_positionMap.clear();
        for (std::size_t i = 0; i < numPositions; ++i)
        {
            double time;
            stream.read((char *)&time, sizeof(double));
            m_positionMap[time].deserialize(stream);
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
 * Get the state acceleration
 */
void InterpolatedKinematicState::getAcceleration(double acceleration[3]) const
{
    interpolate(m_t0, acceleration, m_accelerationMap);
}

/**
 * Get the state acceleration
 */
Vector3d &InterpolatedKinematicState::getAcceleration(void)
{
    m_acceleration.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_acceleration, m_accelerationMap);

    return m_acceleration;
}

/**
 * Get the state acceleration
 */
Vector3d InterpolatedKinematicState::getAcceleration(void) const
{
    Vector3d acceleration;
    interpolate(m_t0, acceleration, m_accelerationMap);

    return acceleration;
}

/**
 * Get the name of this class
 */
std::string InterpolatedKinematicState::getClassName(void) const
{
    return "InterpolatedKinematicState";
}

/**
 * Get the Euler accelerations
 */
void InterpolatedKinematicState::getEulerAccelerations(double &rollAccel,
                                                       double &pitchAccel,
                                                       double &yawAccel) const
{
    Eulers eulerAccelerations;
    interpolate(m_t0, eulerAccelerations, m_eulerAccelerationsMap);

    pitchAccel = eulerAccelerations.getPitch();
    rollAccel = eulerAccelerations.getRoll();
    yawAccel = eulerAccelerations.getYaw();
}

/**
 * Get the Euler accelerations
 */
Eulers &InterpolatedKinematicState::getEulerAccelerations(void)
{
    m_eulerAccelerations.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_eulerAccelerations, m_eulerAccelerationsMap);

    return m_eulerAccelerations;
}

/**
 * Get the Euler accelerations
 */
Eulers InterpolatedKinematicState::getEulerAccelerations(void) const
{
    Eulers eulerAccelerations;
    interpolate(m_t0, eulerAccelerations, m_eulerAccelerationsMap);

    return eulerAccelerations;
}

/**
 * Get the Euler rates at the specified time
 */
void InterpolatedKinematicState::getEulerRates(double &rollRate,
                                               double &pitchRate,
                                               double &yawRate,
                                               double t) const
{
    Eulers eulerRates;
    interpolate(t, eulerRates, m_eulerRatesMap);

    pitchRate = eulerRates.getPitch();
    rollRate = eulerRates.getRoll();
    yawRate = eulerRates.getYaw();
}

/**
 * Get the Euler rates
 */
Eulers &InterpolatedKinematicState::getEulerRates(void)
{
    m_eulerRates.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_eulerRates, m_eulerRatesMap);

    return m_eulerRates;
}

/**
 * Get the Euler rates
 */
Eulers InterpolatedKinematicState::getEulerRates(void) const
{
    Eulers eulerRates;
    interpolate(m_t0, eulerRates, m_eulerRatesMap);

    return eulerRates;
}

/**
 * Get the Euler angles at the specified time
 */
void InterpolatedKinematicState::getEulers(double &roll,
                                           double &pitch,
                                           double &yaw,
                                           double t) const
{
    Eulers eulers;
    interpolate(t, eulers, m_eulersMap);

    pitch = eulers.getPitch();
    roll = eulers.getRoll();
    yaw = eulers.getYaw();
}

/**
 * Get the Euler angles
 */
Eulers &InterpolatedKinematicState::getEulers(void)
{
    m_eulers.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_eulers, m_eulersMap);

    return m_eulers;
}

/**
 * Get the Euler angles
 */
Eulers InterpolatedKinematicState::getEulers(void) const
{
    Eulers eulers;
    interpolate(m_t0, eulers, m_eulersMap);

    return eulers;
}

/**
 * Get the factory name of this constructible
 */
std::string InterpolatedKinematicState::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get this object's pitch angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedKinematicState::getPitch(const AngleUnitType &angleUnits,
                                            double t) const
{
    auto &&eulers = getEulers(t);

    eulers.convertAngleUnits(angleUnits);

    return eulers.getPitch();
}

/**
 * Get this object's pitch acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double InterpolatedKinematicState::getPitchAcceleration(const AngleUnitType &angleUnits) const
{
    auto &&eulerAccelerations = getEulerAccelerations();

    eulerAccelerations.convertAngleUnits(angleUnits);

    return eulerAccelerations.getPitch();
}

/**
 * Get this object's pitch rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedKinematicState::getPitchRate(const AngleUnitType &angleUnits,
                                                double t) const
{
    auto &&eulerRates = getEulerRates(t);

    eulerRates.convertAngleUnits(angleUnits);

    return eulerRates.getPitch();
}

/**
 * Get the state position at the specified time
 */
void InterpolatedKinematicState::getPosition(double position[3],
                                             double t) const
{
    interpolate(t, position, m_positionMap);
}

/**
 * Get the state position
 */
Vector3d &InterpolatedKinematicState::getPosition(void)
{
    m_position.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_position, m_positionMap);

    return m_position;
}

/**
 * Get the state position
 */
Vector3d InterpolatedKinematicState::getPosition(void) const
{
    Vector3d position;
    interpolate(m_t0, position, m_positionMap);

    return position;
}

/**
 * Get this object's roll angle
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedKinematicState::getRoll(const AngleUnitType &angleUnits,
                                           double t) const
{
    auto &&eulers = getEulers(t);

    eulers.convertAngleUnits(angleUnits);

    return eulers.getRoll();
}

/**
 * Get this object's roll acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double InterpolatedKinematicState::getRollAcceleration(const AngleUnitType &angleUnits) const
{
    auto &&eulerAccelerations = getEulerAccelerations();

    eulerAccelerations.convertAngleUnits(angleUnits);

    return eulerAccelerations.getRoll();
}

/**
 * Get this object's roll rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedKinematicState::getRollRate(const AngleUnitType &angleUnits,
                                               double t) const
{
    auto &&eulerRates = getEulerRates(t);

    eulerRates.convertAngleUnits(angleUnits);

    return eulerRates.getRoll();
}

/**
 * Get the state velocity at the specified time
 */
void InterpolatedKinematicState::getVelocity(double velocity[3],
                                             double t) const
{
    interpolate(t, velocity, m_velocityMap);
}

/**
 * Get the state velocity
 */
Vector3d &InterpolatedKinematicState::getVelocity(void)
{
    m_velocity.set(0.0, 0.0, 0.0);
    interpolate(m_t0, m_velocity, m_velocityMap);

    return m_velocity;
}

/**
 * Get the state velocity
 */
Vector3d InterpolatedKinematicState::getVelocity(void) const
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
double InterpolatedKinematicState::getYaw(const AngleUnitType &angleUnits,
                                          double t) const
{
    auto &&eulers = getEulers(t);

    eulers.convertAngleUnits(angleUnits);

    return eulers.getYaw();
}

/**
 * Get this object's yaw acceleration
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 */
double InterpolatedKinematicState::getYawAcceleration(const AngleUnitType &angleUnits) const
{
    auto &&eulerAccelerations = getEulerAccelerations();

    eulerAccelerations.convertAngleUnits(angleUnits);

    return eulerAccelerations.getYaw();
}

/**
 * Get this object's yaw rate
 * @param angleUnits the desired angle units of the return value, Degrees or Radians
 * @param t          the time at which the return value will be computed
 */
double InterpolatedKinematicState::getYawRate(const AngleUnitType &angleUnits,
                                              double t) const
{
    auto &&eulerRates = getEulerRates(t);

    eulerRates.convertAngleUnits(angleUnits);

    return eulerRates.getYaw();
}

/**
 * Initialization function
 */
bool InterpolatedKinematicState::initialize(void)
{
    bool bSuccess = KinematicState::initialize();
    if (bSuccess)
    {
        m_accelerationMap.clear();
        m_eulerAccelerationsMap.clear();
        m_eulerRatesMap.clear();
        m_eulersMap.clear();
        m_positionMap.clear();
        m_velocityMap.clear();
    }

    return bSuccess;
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool InterpolatedKinematicState::isEqual(const KinematicState &state,
                                         double tol) const
{
    return isEqual(dynamic_cast<const InterpolatedKinematicState &>(state));
}

/**
 * Determines whether or not two kinematic states are equivalent within the specified tolerance
 */
bool InterpolatedKinematicState::isEqual(const InterpolatedKinematicState &state,
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
bool InterpolatedKinematicState::isSpatiallyEqual(const KinematicState &state,
                                                  double tol) const
{
    return isSpatiallyEqual(dynamic_cast<const InterpolatedKinematicState &>(state));
}

/**
 * Determines whether or not two kinematic states are spatially equivalent within the specified tolerance
 * (tests all but time)
 */
bool InterpolatedKinematicState::isSpatiallyEqual(const InterpolatedKinematicState &state,
                                                  double tol) const
{
    bool bEqual = (memcmp((void *)this, (void *)&state, sizeof(KinematicState)) == 0);
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
            bEqual = m_eulerAccelerationsMap.size() == state.m_eulerAccelerationsMap.size() &&
                     std::equal(m_eulerAccelerationsMap.cbegin(),
                                m_eulerAccelerationsMap.cend(),
                                state.m_eulerAccelerationsMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_eulerRatesMap.size() == state.m_eulerRatesMap.size() &&
                     std::equal(m_eulerRatesMap.cbegin(),
                                m_eulerRatesMap.cend(),
                                state.m_eulerRatesMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_eulersMap.size() == state.m_eulersMap.size() &&
                     std::equal(m_eulersMap.cbegin(),
                                m_eulersMap.cend(),
                                state.m_eulersMap.begin(),
                                comparator);
        }

        if (bEqual)
        {
            bEqual = m_positionMap.size() == state.m_positionMap.size() &&
                     std::equal(m_positionMap.cbegin(),
                                m_positionMap.cend(),
                                state.m_positionMap.begin(),
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
std::ostream &InterpolatedKinematicState::print(std::ostream &stream) const
{
    KinematicState::print(stream);

    if (stream)
    {
        static const std::vector<std::string> strings = { "Position vs time:",
                                                          "Velocity vs time:",
                                                          "Acceleration vs time:",
                                                          "Eulers vs time:",
                                                          "Euler rates vs time:",
                                                          "Euler accelerations vs time:" };

        stream << std::endl;
        auto &&itStrings = strings.cbegin();
        for (auto &mapEntry : { m_positionMap,
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

        for (auto &mapEntry : { m_eulersMap,
                                m_eulerRatesMap,
                                m_eulerAccelerationsMap })
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
bool InterpolatedKinematicState::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = KinematicState::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pPositionNode = pNode->first_node("position");
        while (pPositionNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pPositionNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_positionMap[t].readFromXML(pPositionNode);
            }

            pPositionNode = pPositionNode->next_sibling("position");
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

        auto *pEulersNode = pNode->first_node("eulers");
        while (pEulersNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pEulersNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_eulersMap[t].readFromXML(pEulersNode);
            }

            pEulersNode = pEulersNode->next_sibling("eulers");
        }

        auto *pEulerRatesNode = pNode->first_node("eulerRates");
        while (pEulerRatesNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pEulerRatesNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_eulerRatesMap[t].readFromXML(pEulerRatesNode);
            }

            pEulerRatesNode = pEulerRatesNode->next_sibling("eulerRates");
        }

        auto *pEulerAccelerationsNode = pNode->first_node("eulerAccelerations");
        while (pEulerAccelerationsNode != nullptr)
        {
            double t = 0.0;
            auto *pTimeNode = pEulerAccelerationsNode->first_node("time");
            if (pTimeNode != nullptr)
            {
                t = std::stod(pTimeNode->value());
                m_eulerAccelerationsMap[t].readFromXML(pEulerAccelerationsNode);
            }

            pEulerAccelerationsNode = pEulerAccelerationsNode->next_sibling("eulerAccelerations");
        }
    }

    return bSuccess;
}
#endif
/**
 * Function to serialize this object's data
 */
std::ostream &InterpolatedKinematicState::serialize(std::ostream &stream) const
{
    KinematicState::serialize(stream);

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

        auto numEulerAccelerations = m_eulerAccelerationsMap.size();
        stream.write((const char *)&numEulerAccelerations, sizeof(std::size_t));
        auto &&itEulerAcceleration = m_eulerAccelerationsMap.cbegin();
        while (itEulerAcceleration != m_eulerAccelerationsMap.cend())
        {
            double time = itEulerAcceleration->first;
            stream.write((const char *)&time, sizeof(double));
            itEulerAcceleration->second.serialize(stream);
            ++itEulerAcceleration;
        }

        auto numEulerRates = m_eulerRatesMap.size();
        stream.write((const char *)&numEulerRates, sizeof(std::size_t));
        auto &&itEulerRates = m_eulerRatesMap.cbegin();
        while (itEulerRates != m_eulerRatesMap.cend())
        {
            double time = itEulerRates->first;
            stream.write((const char *)&time, sizeof(double));
            itEulerRates->second.serialize(stream);
            ++itEulerRates;
        }

        auto numEulers = m_eulersMap.size();
        stream.write((const char *)&numEulers, sizeof(std::size_t));
        auto &&itEulers = m_eulersMap.cbegin();
        while (itEulers != m_eulersMap.cend())
        {
            double time = itEulers->first;
            stream.write((const char *)&time, sizeof(double));
            itEulers->second.serialize(stream);
            ++itEulers;
        }

        auto numPositions = m_positionMap.size();
        stream.write((const char *)&numPositions, sizeof(std::size_t));
        auto &&itPosition = m_positionMap.cbegin();
        while (itPosition != m_positionMap.cend())
        {
            double time = itPosition->first;
            stream.write((const char *)&time, sizeof(double));
            itPosition->second.serialize(stream);
            ++itPosition;
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
 * Set the state acceleration
 */
void InterpolatedKinematicState::setAcceleration(double xAcceleration,
                                                 double yAcceleration,
                                                 double zAcceleration)
{
    m_accelerationMap[m_t0].set(xAcceleration,
                                yAcceleration,
                                zAcceleration);
}

/**
 * Set angle units (Degrees or Radians)
 */
void InterpolatedKinematicState::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;

    std::for_each(m_eulerAccelerationsMap.begin(),
                  m_eulerAccelerationsMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.setAngleUnits(angleUnits); });

    std::for_each(m_eulerRatesMap.begin(),
                  m_eulerRatesMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.setAngleUnits(angleUnits); });

    std::for_each(m_eulersMap.begin(),
                  m_eulersMap.end(),
                  [&angleUnits] (auto &&pair) { pair.second.setAngleUnits(angleUnits); });
}

/**
 * Set Euler accelerations used to propagate Euler rates
 */
void InterpolatedKinematicState::setEulerAccelerations(const Eulers &eulerAccelerations)
{
    setEulerAccelerations(eulerAccelerations[EulerAccelerationAxisType::Roll],
                          eulerAccelerations[EulerAccelerationAxisType::Pitch],
                          eulerAccelerations[EulerAccelerationAxisType::Yaw]);

    m_eulerAccelerationsMap[m_t0].convertAngleUnits(m_angleUnits);
}

/**
 * Set Euler accelerations used to propagate Euler rates
 */
void InterpolatedKinematicState::setEulerAccelerations(double rollAcceleration,
                                                       double pitchAcceleration,
                                                       double yawAcceleration)
{
    auto &&eulerAccelerations = m_eulerAccelerationsMap[m_t0];
    eulerAccelerations.setPitch(pitchAcceleration);
    eulerAccelerations.setRoll(rollAcceleration);
    eulerAccelerations.setYaw(yawAcceleration);
}

/**
 * Set Euler rates used to propagate Euler angles
 */
void InterpolatedKinematicState::setEulerRates(const Eulers &eulerRates)
{
    setEulerRates(eulerRates[EulerRateAxisType::Roll],
                  eulerRates[EulerRateAxisType::Pitch],
                  eulerRates[EulerRateAxisType::Yaw]);

    m_eulerRatesMap[m_t0].convertAngleUnits(m_angleUnits);
}

/**
 * Set Euler rates used to propagate Euler angles
 */
void InterpolatedKinematicState::setEulerRates(double rollRate,
                                               double pitchRate,
                                               double yawRate)
{
    auto &&eulerRates = m_eulerRatesMap[m_t0];
    eulerRates.setPitch(pitchRate);
    eulerRates.setRoll(rollRate);
    eulerRates.setYaw(yawRate);
}

/**
 * Set the Euler angles
 */
void InterpolatedKinematicState::setEulers(const Eulers &eulers)
{
    setEulers(eulers[EulerAxisType::Roll],
              eulers[EulerAxisType::Pitch],
              eulers[EulerAxisType::Yaw]);

    m_eulersMap[m_t0].convertAngleUnits(m_angleUnits);
}

/**
 * Set the Euler angles
 */
void InterpolatedKinematicState::setEulers(double roll,
                                           double pitch,
                                           double yaw)
{
    auto &&eulers = m_eulersMap[m_t0];
    eulers.setPitch(pitch);
    eulers.setRoll(roll);
    eulers.setYaw(yaw);
}

/**
 * Set this object's pitch angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setPitch(double pitch,
                                          const AngleUnitType &angleUnits)
{
    auto &&eulers = m_eulersMap[m_t0];
    eulers.setAngleUnits(m_angleUnits);
    eulers.setPitch(pitch, angleUnits);
}

/**
 * Set this object's pitch acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setPitchAcceleration(double pitchAcceleration,
                                                      const AngleUnitType &angleUnits)
{
    auto &&eulerAccelerations = m_eulerAccelerationsMap[m_t0];
    eulerAccelerations.setAngleUnits(m_angleUnits);
    eulerAccelerations.setPitch(pitchAcceleration, angleUnits);
}

/**
 * Set this object's pitch rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setPitchRate(double pitchRate,
                                              const AngleUnitType &angleUnits)
{
    auto &&eulerRates = m_eulerRatesMap[m_t0];
    eulerRates.setAngleUnits(m_angleUnits);
    eulerRates.setPitch(pitchRate, angleUnits);
}

/**
 * Set the position of the origin of this frame with respect to its parent
 */
void InterpolatedKinematicState::setPosition(double xPosition,
                                             double yPosition,
                                             double zPosition)
{
    m_positionMap[m_t0].set(xPosition,
                            yPosition,
                            zPosition);
}

/**
 * Set this object's roll angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setRoll(double roll,
                                         const AngleUnitType &angleUnits)
{
    auto &&eulers = m_eulersMap[m_t0];
    eulers.setAngleUnits(m_angleUnits);
    eulers.setRoll(roll, angleUnits);
}

/**
 * Set this object's roll acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setRollAcceleration(double rollAcceleration,
                                                     const AngleUnitType &angleUnits)
{
    auto &&eulerAccelerations = m_eulerAccelerationsMap[m_t0];
    eulerAccelerations.setAngleUnits(m_angleUnits);
    eulerAccelerations.setRoll(rollAcceleration, angleUnits);
}

/**
 * Set this object's roll rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setRollRate(double rollRate,
                                             const AngleUnitType &angleUnits)
{
    auto &&eulerRates = m_eulerRatesMap[m_t0];
    eulerRates.setAngleUnits(m_angleUnits);
    eulerRates.setRoll(rollRate, angleUnits);
}

/**
 * Set the velocity of the origin of this frame with respect to its parent
 */
void InterpolatedKinematicState::setVelocity(double xVelocity,
                                             double yVelocity,
                                             double zVelocity)
{
    m_velocityMap[m_t0].set(xVelocity,
                            yVelocity,
                            zVelocity);
}

/**
 * Set this object's yaw angle
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setYaw(double yaw,
                                        const AngleUnitType &angleUnits)
{
    auto &&eulers = m_eulersMap[m_t0];
    eulers.setAngleUnits(m_angleUnits);
    eulers.setYaw(yaw, angleUnits);
}

/**
 * Set this object's yaw acceleration
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setYawAcceleration(double yawAcceleration,
                                                    const AngleUnitType &angleUnits)
{
    auto &&eulerAccelerations = m_eulerAccelerationsMap[m_t0];
    eulerAccelerations.setAngleUnits(m_angleUnits);
    eulerAccelerations.setYaw(yawAcceleration, angleUnits);
}

/**
 * Set this object's yaw rate
 * @param angleUnits the angle units of the input argument, Degrees or Radians
 */
void InterpolatedKinematicState::setYawRate(double yawRate,
                                            const AngleUnitType &angleUnits)
{
    auto &&eulerRates = m_eulerRatesMap[m_t0];
    eulerRates.setAngleUnits(m_angleUnits);
    eulerRates.setYaw(yawRate, angleUnits);
}

/**
 * Swap function
 */
void InterpolatedKinematicState::swap(InterpolatedKinematicState &state)
{
    KinematicState::swap(state);

    m_accelerationMap.swap(state.m_accelerationMap);
    m_eulerAccelerationsMap.swap(state.m_eulerAccelerationsMap);
    m_eulerRatesMap.swap(state.m_eulerRatesMap);
    m_eulersMap.swap(state.m_eulersMap);
    m_positionMap.swap(state.m_positionMap);
    m_velocityMap.swap(state.m_velocityMap);
}
#ifdef RAPID_XML
/**
 * Function to write data to XML
 */
bool InterpolatedKinematicState::writeToXML(xml_node<> *pNode)
{
    bool bSuccess = KinematicState::writeToXML(pNode);
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

            auto &&itEulerAcceleration = m_eulerAccelerationsMap.cbegin();
            while (itEulerAcceleration != m_eulerAccelerationsMap.cend())
            {
                double time = itEulerAcceleration->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pEulerAccelerationsNode = pDocument->allocate_node(node_element, "eulerAccelerations");
                bSuccess = m_eulerAccelerations.writeToXML(pEulerAccelerationsNode);
                if (bSuccess)
                    pEulerAccelerationsNode->append_node(pTimeNode);

                pNode->append_node(pEulerAccelerationsNode);
                ++itEulerAcceleration;
            }

            auto &&itEulerRates = m_eulerRatesMap.cbegin();
            while (itEulerRates != m_eulerRatesMap.cend())
            {
                double time = itEulerRates->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pEulerRatesNode = pDocument->allocate_node(node_element, "eulerRates");
                bSuccess = m_eulerRates.writeToXML(pEulerRatesNode);
                if (bSuccess)
                    pEulerRatesNode->append_node(pTimeNode);

                pNode->append_node(pEulerRatesNode);
                ++itEulerRates;
            }

            auto &&itEulers = m_eulersMap.cbegin();
            while (itEulers != m_eulersMap.cend())
            {
                double time = itEulers->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pEulersNode = pDocument->allocate_node(node_element, "eulers");
                bSuccess = m_eulers.writeToXML(pEulersNode);
                if (bSuccess)
                    pEulersNode->append_node(pTimeNode);

                pNode->append_node(pEulersNode);
                ++itEulers;
            }

            auto &&itPosition = m_positionMap.cbegin();
            while (itPosition != m_positionMap.cend())
            {
                double time = itPosition->first;
                auto *pTimeNode = pDocument->allocate_node(node_element, "time");
                auto *pTimeString = pDocument->allocate_string(std::to_string(time).c_str());
                auto *pDataNode = pDocument->allocate_node(node_data, pTimeString);
                pTimeNode->append_node(pDataNode);

                auto *pPositionNode = pDocument->allocate_node(node_element, "position");
                bSuccess = m_position.writeToXML(pPositionNode);
                if (bSuccess)
                    pPositionNode->append_node(pTimeNode);

                pNode->append_node(pPositionNode);
                ++itPosition;
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
