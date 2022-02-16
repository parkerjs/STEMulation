#include "angularSubtense.h"
#include "limiter.h"
#include "math_constants.h"
#include "phase_unwrapper.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "rungeKutta.h"
#include "servoMechanicalGimbal.h"

// file-scoped variables
static constexpr char factoryName[] = "ServoMechanicalGimbal";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::control_systems::blocks;
using namespace math::geometric;
using namespace math::trigonometric;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace utilities;

namespace math
{

namespace control_systems
{

namespace applied
{

// register factories...
static FactoryRegistrar<ServoMechanicalGimbal>
factory(factoryName, &ServoMechanicalGimbal::create);

/**
 * Constructor
 * @param angleUnits the angle units associated with this object's angular quantities (Degrees or Radians)
 * @param omega      the natural frequency in radians
 * @param zeta       the damping ratio
 */
ServoMechanicalGimbal::ServoMechanicalGimbal(const AngleUnitType &angleUnits,
                                             double omega,
                                             double zeta)
: SecondOrderSystem(omega, zeta),
  m_accelerationLimit(std::numeric_limits<double>::max()),
  m_achievedAcceleration(0.0),
  m_achievedPosition(0.0),
  m_achievedRate(0.0),
  m_angleUnits(angleUnits),
  m_commandPosition(0.0),
  m_initialPosition(0.0),
  m_positionTolerance(0.01),
  m_pPhaseUnwrapper(new PhaseUnwrapper(0.0, 0.0)), // set this below
  m_pPositionLimiter(new Limiter(-std::numeric_limits<double>::max(),
                                  std::numeric_limits<double>::max())),
  m_rateLimit(std::numeric_limits<double>::max()),
  m_rateTolerance(0.01)
{
    double cnv = 1.0;
    if (angleUnits == AngleUnitType::Radians)
        cnv = DEGREES_TO_RADIANS;

    // set the phase wrapping interval to [-180.0, 180.0]
    m_pPhaseUnwrapper->setLowerLimit(-180.0 * cnv);
    m_pPhaseUnwrapper->setUpperLimit(180.0 * cnv);

    // set the default position limits to +/- 180.0 degrees
    m_pPositionLimiter->setLowerLimit(-180.0 * cnv);
    m_pPositionLimiter->setUpperLimit(180.0 * cnv);
}

/**
 * Copy constructor
 */
ServoMechanicalGimbal::ServoMechanicalGimbal(const ServoMechanicalGimbal &servoMechanicalGimbal)
: m_pPhaseUnwrapper(nullptr),
  m_pPositionLimiter(nullptr)
{
    operator = (servoMechanicalGimbal);
}

/**
 * Move constructor
 */
ServoMechanicalGimbal::ServoMechanicalGimbal(ServoMechanicalGimbal &&servoMechanicalGimbal)
: m_pPhaseUnwrapper(nullptr),
  m_pPositionLimiter(nullptr)
{
    operator = (std::move(servoMechanicalGimbal));
}

/**
 * Destructor
 */
ServoMechanicalGimbal::~ServoMechanicalGimbal(void)
{
    if (m_pPhaseUnwrapper != nullptr)
        delete m_pPhaseUnwrapper;

    if (m_pPositionLimiter != nullptr)
        delete m_pPositionLimiter;
}

/**
 * Copy assignment operator
 */
ServoMechanicalGimbal &ServoMechanicalGimbal::operator = (const ServoMechanicalGimbal &servoMechanicalGimbal)
{
    if (&servoMechanicalGimbal != this)
    {
        Loggable<std::string, std::ostream>::operator = (servoMechanicalGimbal);
        SecondOrderSystem::operator = (servoMechanicalGimbal);

        m_accelerationLimit = servoMechanicalGimbal.m_accelerationLimit;
        m_achievedAcceleration = servoMechanicalGimbal.m_achievedAcceleration;
        m_achievedPosition = servoMechanicalGimbal.m_achievedPosition;
        m_achievedRate = servoMechanicalGimbal.m_achievedRate;
        m_angleUnits = servoMechanicalGimbal.m_angleUnits;
        m_commandPosition = servoMechanicalGimbal.m_commandPosition;
        m_initialPosition = servoMechanicalGimbal.m_initialPosition;
        m_positionTolerance = servoMechanicalGimbal.m_positionTolerance;

        if (m_pPhaseUnwrapper == nullptr)
        {
            double cnv = 1.0;
            if (m_angleUnits == AngleUnitType::Radians)
                cnv = DEGREES_TO_RADIANS;

            // set the phase wrapping interval to [-180.0, 180.0]
            m_pPhaseUnwrapper = new PhaseUnwrapper(-180.0 * cnv,
                                                    180.0 * cnv);
        }

        if (servoMechanicalGimbal.m_pPhaseUnwrapper != nullptr)
            *m_pPhaseUnwrapper = *servoMechanicalGimbal.m_pPhaseUnwrapper;

        if (m_pPositionLimiter == nullptr)
        {
            // set the position limits
            m_pPositionLimiter = new Limiter(-std::numeric_limits<double>::max(),
                                              std::numeric_limits<double>::max());
        }

        if (servoMechanicalGimbal.m_pPositionLimiter != nullptr)
            *m_pPositionLimiter = *servoMechanicalGimbal.m_pPositionLimiter;

        m_rateLimit = servoMechanicalGimbal.m_rateLimit;
        m_rateTolerance = servoMechanicalGimbal.m_rateTolerance;
    }

    return *this;
}

/**
 * Move assignment operator
 */
ServoMechanicalGimbal &ServoMechanicalGimbal::operator = (ServoMechanicalGimbal &&servoMechanicalGimbal)
{
    if (&servoMechanicalGimbal != this)
    {
        servoMechanicalGimbal.swap(*this);
    }

    return *this;
}

/**
 * Calculate the position error relative to this object's command position
 */
double ServoMechanicalGimbal::calcPositionError(void) const
{
    return calcPositionError(m_commandPosition);
}

/**
 * Calculate the position error relative to the specified command position
 */
double ServoMechanicalGimbal::calcPositionError(double commandPosition) const
{
    auto positionError = std::numeric_limits<double>::max();
    if (commandPositionIsValid(commandPosition))
    {
        positionError = commandPosition - m_achievedPosition;
        if (m_pPhaseUnwrapper != nullptr)
            positionError = m_pPhaseUnwrapper->apply(positionError);
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Command position is invalid!\n",
               getQualifiedMethodName(__func__));
    }

    return positionError;
}

/**
 * Determine if the specified command position is valid
 */
bool ServoMechanicalGimbal::commandPositionIsValid(double commandPosition) const
{
    bool bSuccess = (m_pPositionLimiter != nullptr);
    if (bSuccess)
    {
        auto lowerLimit = m_pPositionLimiter->getLowerLimit();
        auto upperLimit = m_pPositionLimiter->getUpperLimit();

        bSuccess = AngularSubtense::angleWithinSubtense(commandPosition,
                                                        lowerLimit,
                                                        upperLimit,
                                                        m_angleUnits);
    }

    return bSuccess;
}

/**
 * Create() function
 */
ServoMechanicalGimbal *ServoMechanicalGimbal::create(void)
{
    auto *pServoMechanicalGimbal = new ServoMechanicalGimbal();
    if (!pServoMechanicalGimbal->setup())
    {
        delete pServoMechanicalGimbal;
        pServoMechanicalGimbal = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Created an instance of \"" + pServoMechanicalGimbal->getClassName() + "\".\n",
               getQualifiedStaticMethodName(__func__, ServoMechanicalGimbal));
    }

    return pServoMechanicalGimbal;
}
#ifdef RAPID_XML
/**
 * Function to construct an instance from an XML node
 * @param pNode a pointer to an XML node
 */
ServoMechanicalGimbal *ServoMechanicalGimbal::createFromXML(xml_node<> *pNode)
{
    ServoMechanicalGimbal *pServoMechanicalGimbal = nullptr;
    if (pNode != nullptr &&
        std::strcmp(pNode->name(), "servoMechanicalGimbal") == 0)
    {
        auto *pAttribute = pNode->first_attribute("type");
        if (pAttribute != nullptr)
        {
            auto &&type = pAttribute->value();
            pServoMechanicalGimbal = FactoryConstructible<ServoMechanicalGimbal>::create(type);
            if (pServoMechanicalGimbal != nullptr)
            {
                pServoMechanicalGimbal->readFromXML(pNode);
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create servo-mechanical gimbal of type \"" + std::string(type) + "\"!\n",
                       getQualifiedStaticMethodName(__func__, ServoMechanicalGimbal));
            }
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Servo-mechanical gimbal XML node elements must specify a 'type' attribute!\n",
                   getQualifiedStaticMethodName(__func__, ServoMechanicalGimbal));
        }
    }

    return pServoMechanicalGimbal;
}
#endif
/**
 * Function to evaluate the dynamics model which represents this control system
 * @param t  time at which the dynamics model is evaluated
 * @param x  the state vector
 * @param xd the state derivative vector
 */
void ServoMechanicalGimbal::dynamicsModel(double t,
                                          const StateVector &x,
                                          StateVector &xd)
{
    xd[0] = x[1];

    auto absRateLimit = std::fabs(m_rateLimit);
    auto commandPosition = calcPositionError();
    auto commandRate = 0.5 * commandPosition * m_omega / m_zeta;
    auto signumCommandRate = (commandRate < 0.0 ? -1.0 : 1.0);
    if (std::fabs(commandRate) > absRateLimit)
        commandRate = signumCommandRate * absRateLimit;

    auto absAccelerationLimit = std::fabs(m_accelerationLimit);
    auto rateError = commandRate - x[1];
    auto commandAcceleration = 2 * m_zeta * m_omega * rateError;
    auto signumCommandAcceleration = (commandAcceleration < 0.0 ? -1.0 : 1.0);
    if (std::fabs(commandAcceleration) > absAccelerationLimit)
        commandAcceleration = signumCommandAcceleration * absAccelerationLimit;

    xd[1] = commandAcceleration;
}

/**
 * Get this object's acceleration
 */
double ServoMechanicalGimbal::getAcceleration(void) const
{
    return m_achievedAcceleration;
}

/**
 * Get this object's acceleration limit
 */
double ServoMechanicalGimbal::getAccelerationLimit(void) const
{
    return m_accelerationLimit;
}

/**
 * Get the angle units associated with this object's angular quantities (Degrees or Radians)
 */
AngleUnitType ServoMechanicalGimbal::getAngleUnits(void) const
{
    return m_angleUnits;
}

/**
 * Function to return this object's field of regard as an angular subtense
 */
AngularSubtense ServoMechanicalGimbal::getFieldOfRegard(void) const
{
    auto lowerLimit = m_pPositionLimiter->getLowerLimit();
    auto upperLimit = m_pPositionLimiter->getUpperLimit();

    return AngularSubtense(lowerLimit, upperLimit, m_angleUnits);
}

/**
 * Get the name of this class
 */
std::string ServoMechanicalGimbal::getClassName(void) const
{
    return "ServoMechanicalGimbal";
}

/**
 * Get this object's angular command position
 */
double ServoMechanicalGimbal::getCommandPosition(void) const
{
    return m_commandPosition;
}

/**
 * Get the factory name of this constructible
 */
std::string ServoMechanicalGimbal::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get this object's initial angular position
 */
double ServoMechanicalGimbal::getInitialPosition(void) const
{
    return m_initialPosition;
}

/**
 * Get this object's phase un-wrapper
 */
ServoMechanicalGimbal::PhaseUnwrapper *
ServoMechanicalGimbal::getPhaseUnwrapper(void) const
{
    return m_pPhaseUnwrapper;
}

/**
 * Get this object's angular position
 */
double ServoMechanicalGimbal::getPosition(void) const
{
    return m_achievedPosition;
}

/**
 * Get this object's angular position limiter
 */
ServoMechanicalGimbal::Limiter *
ServoMechanicalGimbal::getPositionLimiter(void) const
{
    return m_pPositionLimiter;
}

/**
 * Get this object's angular position tolerance
 */
double ServoMechanicalGimbal::getPositionTolerance(void) const
{
    return m_positionTolerance;
}

/**
 * Get this object's rate
 */
double ServoMechanicalGimbal::getRate(void) const
{
    return m_achievedRate;
}

/**
 * Get this object's rate limit
 */
double ServoMechanicalGimbal::getRateLimit(void) const
{
    return m_rateLimit;
}

/**
 * Get this object's angular rate tolerance
 */
double ServoMechanicalGimbal::getRateTolerance(void) const
{
    return m_rateTolerance;
}

/**
 * Function to determine if the angular position has settled at the current command
 */
bool ServoMechanicalGimbal::hasSettled(void) const
{
    auto positionError = calcPositionError();

    return positionError < m_positionTolerance && m_achievedRate < m_rateTolerance;
}

/**
 * Initialization function
 */
bool ServoMechanicalGimbal::initialize(void)
{
    bool bSuccess = (m_pPhaseUnwrapper != nullptr &&
                     m_pPositionLimiter != nullptr);
    if (bSuccess)
    {
        m_achievedAcceleration = 0.0;
        m_achievedPosition = m_initialPosition;
        m_achievedRate = 0.0;
    }

    return bSuccess;
}

/**
 * Test whether the angular position limit has been reached
 */
bool ServoMechanicalGimbal::limitReached(void) const
{
    bool bLimitReached = false;
    if (m_pPositionLimiter != nullptr)
    {
        auto lowerLimit = m_pPositionLimiter->getLowerLimit();
        auto upperLimit = m_pPositionLimiter->getUpperLimit();

        bLimitReached = (std::fabs(m_achievedPosition - lowerLimit) < m_positionTolerance ||
                         std::fabs(m_achievedPosition - upperLimit) < m_positionTolerance);

    }

    return bLimitReached;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool ServoMechanicalGimbal::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = SecondOrderSystem::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pAccelerationLimitNode = pNode->first_node("accelerationLimit");
        if (pAccelerationLimitNode != nullptr)
            setAccelerationLimit(std::stod(pAccelerationLimitNode->value()));

        auto *pAngleUnitsNode = pNode->first_node("angleType");
        if (pAngleUnitsNode != nullptr)
            m_angleUnits = pAngleUnitsNode->value();
        else
            m_angleUnits = AngleUnitType::Enum::Degrees;

        auto *pInitialPositionNode = pNode->first_node("initialPosition");
        if (pInitialPositionNode != nullptr)
            bSuccess = setInitialPosition(std::stod(pInitialPositionNode->value()));

        if (bSuccess)
        {
            auto *pPositionToleranceNode = pNode->first_node("positionTolerance");
            if (pPositionToleranceNode != nullptr)
                setPositionTolerance(std::stod(pPositionToleranceNode->value()));

            auto *pPhaseUnwrapperNode = pNode->first_node("phaseUnwrapper");
            if (m_pPhaseUnwrapper != nullptr &&
                pPhaseUnwrapperNode != nullptr)
            {
                bSuccess = m_pPhaseUnwrapper->readFromXML(pPhaseUnwrapperNode);
            }
        }

        if (bSuccess)
        {
            auto *pPositionLimiterNode = pNode->first_node("positionLimiter");
            if (m_pPositionLimiter != nullptr &&
                pPositionLimiterNode != nullptr)
            {
                bSuccess = m_pPositionLimiter->readFromXML(pPositionLimiterNode);
            }
        }

        if (bSuccess)
        {
            auto *pRateLimitNode = pNode->first_node("rateLimit");
            if (pRateLimitNode != nullptr)
                setRateLimit(std::stod(pRateLimitNode->value()));

            auto *pRateToleranceNode = pNode->first_node("rateTolerance");
            if (pRateToleranceNode != nullptr)
                setRateTolerance(std::stod(pRateToleranceNode->value()));
        }
    }

    return bSuccess;
}
#endif
/**
 * Set this object's acceleration limit
 */
void ServoMechanicalGimbal::setAccelerationLimit(double accelerationLimit)
{
    m_accelerationLimit = accelerationLimit;
}

/**
 * Set the angle units associated with this object's angular quantities (Degrees or Radians)
 */
void ServoMechanicalGimbal::setAngleUnits(const AngleUnitType &angleUnits)
{
    m_angleUnits = angleUnits;
}

/**
 * Set this object's angular command position; returns true upon success
 */
bool ServoMechanicalGimbal::setCommandPosition(double commandPosition)
{
    bool bSuccess = commandPositionIsValid(commandPosition);
    if (bSuccess)
    {
        m_commandPosition = commandPosition;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Command position is invalid!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Set this object's initial angular position; returns true upon success
 */
bool ServoMechanicalGimbal::setInitialPosition(double initialPosition)
{
    bool bSuccess = commandPositionIsValid(initialPosition);
    if (bSuccess)
    {
        m_initialPosition = initialPosition;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Command position is invalid!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Set this object's angular position; returns true upon success
 */
bool ServoMechanicalGimbal::setPosition(double position)
{
    bool bSuccess = commandPositionIsValid(position);
    if (bSuccess)
    {
        m_achievedPosition = position;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Warning,
               "Position is invalid!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Set this object's angular position tolerance
 */
void ServoMechanicalGimbal::setPositionTolerance(double tolerance)
{
    m_positionTolerance = tolerance;
}

/**
 * Set this object's rate limit
 */
void ServoMechanicalGimbal::setRateLimit(double rateLimit)
{
    m_rateLimit = rateLimit;
}

/**
 * Set this object's angular rate tolerance
 */
void ServoMechanicalGimbal::setRateTolerance(double tolerance)
{
    m_rateTolerance = tolerance;
}

/**
 * Setup function
 */
bool ServoMechanicalGimbal::setup(void)
{
    bool bSuccess = SecondOrderSystem::setup();
    if (bSuccess)
    {
        m_registry["accelerationLimit"] = m_accelerationLimit;
        m_registry["angleType"] = m_angleUnits;
        m_registry["initialPosition"] = m_initialPosition;

        if (m_pPhaseUnwrapper != nullptr)
        {
            m_registry["phaseLowerLimit"] = m_pPhaseUnwrapper->getLowerLimit();
            m_registry["phaseUpperLimit"] = m_pPhaseUnwrapper->getUpperLimit();
        }

        if (m_pPositionLimiter != nullptr)
        {
            m_registry["positionLowerLimit"] = m_pPositionLimiter->getLowerLimit();
            m_registry["positionUpperLimit"] = m_pPositionLimiter->getUpperLimit();
        }

        m_registry["positionTolerance"] = m_positionTolerance;
        m_registry["rateLimit"] = m_rateLimit;
        m_registry["rateTolerance"] = m_rateTolerance;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void ServoMechanicalGimbal::swap(ServoMechanicalGimbal &servoMechanicalGimbal)
{
    Loggable<std::string, std::ostream>::swap(servoMechanicalGimbal);
    SecondOrderSystem::swap(servoMechanicalGimbal);

    std::swap(m_accelerationLimit, servoMechanicalGimbal.m_accelerationLimit);
    std::swap(m_achievedAcceleration, servoMechanicalGimbal.m_achievedAcceleration);
    std::swap(m_achievedPosition, servoMechanicalGimbal.m_achievedPosition);
    std::swap(m_achievedRate, servoMechanicalGimbal.m_achievedRate);
    std::swap(m_angleUnits, servoMechanicalGimbal.m_angleUnits);
    std::swap(m_commandPosition, servoMechanicalGimbal.m_commandPosition);
    std::swap(m_initialPosition, servoMechanicalGimbal.m_initialPosition);
    std::swap(m_positionTolerance, servoMechanicalGimbal.m_positionTolerance);
    std::swap(m_pPhaseUnwrapper, servoMechanicalGimbal.m_pPhaseUnwrapper);
    std::swap(m_pPositionLimiter, servoMechanicalGimbal.m_pPositionLimiter);
    std::swap(m_rateLimit, servoMechanicalGimbal.m_rateLimit);
    std::swap(m_rateTolerance, servoMechanicalGimbal.m_rateTolerance);
}

/**
 * Update function
 */
bool ServoMechanicalGimbal::update(double time)
{
    bool bSuccess = true; // assume success
    auto t0 = m_stateVector.getTime();
    auto t1 = time;
    auto dt = t1 - t0;
    if (m_commandPosition != m_achievedPosition && dt > 0.0)
    {
        if (m_pRungeKuttaMethod != nullptr)
        {
            RungeKutta::tStateDynamicsFunction dynamics = [&] (double t, const StateVector &x, StateVector &xd)
            {
                dynamicsModel(t, x, xd);
                m_achievedAcceleration = xd[1];
            };

            m_stateVector[0] = m_achievedPosition;
            m_stateVector[1] = m_achievedRate;
            m_pRungeKuttaMethod->solve(m_stateVector, dynamics, t0, t1);
            m_stateVector.setTime(t1);
        }
        else
        {
            // use simple Euler integration if no Runge-Kutta method has been selected
            m_stateVector[0] = m_achievedPosition;
            m_stateVector[1] = m_achievedRate;

            StateVector xd(2);
            dynamicsModel(time, m_stateVector, xd);

            // integrate rates and accelerations
            m_stateVector[1] += 0.5 * (xd[1] + m_achievedAcceleration) * dt;
            m_stateVector[0] += 0.5 * (m_achievedRate + m_stateVector[1]) * dt;
            m_achievedAcceleration = xd[1];
        }

        // limit the position
        m_achievedPosition = AngularSubtense::limit(m_stateVector[0],
                                                    m_pPositionLimiter->getLowerLimit(),
                                                    m_pPositionLimiter->getUpperLimit(),
                                                    m_angleUnits);

        // limit the rate
        m_achievedRate = m_stateVector[1];
        auto signumRate = (m_achievedRate < 0.0 ? -1.0 : 1.0);
        auto absRateLimit = std::fabs(m_rateLimit);
        if (std::fabs(m_achievedRate) > absRateLimit)
            m_achievedRate = signumRate * absRateLimit;
    }

    m_stateVector.setTime(time);

    return bSuccess;
}

}

}

}
