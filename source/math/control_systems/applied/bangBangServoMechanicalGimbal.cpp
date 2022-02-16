#include "bangBangServoMechanicalGimbal.h"
#include "limiter.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif

// file-scoped variables
static constexpr char factoryName[] = "BangBangServoMechanicalGimbal";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::control_systems::blocks;
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
factory(factoryName, &BangBangServoMechanicalGimbal::create);

/**
 * Constructor
 * @param angleUnits the angle units associated with this object's angular quantities (Degrees or Radians)
 * @param omega      the natural frequency in radians
 * @param zeta       the damping ratio
 */
BangBangServoMechanicalGimbal::BangBangServoMechanicalGimbal(const AngleUnitType &angleUnits,
                                                             double omega,
                                                             double zeta)
: ServoMechanicalGimbal(angleUnits, omega, zeta),
  m_threshold(10.0)
{

}

/**
 * Copy constructor
 */
BangBangServoMechanicalGimbal::
BangBangServoMechanicalGimbal(const BangBangServoMechanicalGimbal &servoMechanicalGimbal)
{
    operator = (servoMechanicalGimbal);
}

/**
 * Move constructor
 */
BangBangServoMechanicalGimbal::
BangBangServoMechanicalGimbal(BangBangServoMechanicalGimbal &&servoMechanicalGimbal)
{
    operator = (std::move(servoMechanicalGimbal));
}

/**
 * Destructor
 */
BangBangServoMechanicalGimbal::~BangBangServoMechanicalGimbal(void)
{

}

/**
 * Copy assignment operator
 */
BangBangServoMechanicalGimbal &
BangBangServoMechanicalGimbal::operator = (const BangBangServoMechanicalGimbal &servoMechanicalGimbal)
{
    if (&servoMechanicalGimbal != this)
    {
        ServoMechanicalGimbal::operator = (servoMechanicalGimbal);

        m_threshold = servoMechanicalGimbal.m_threshold;
    }

    return *this;
}

/**
 * Move assignment operator
 */
BangBangServoMechanicalGimbal &
BangBangServoMechanicalGimbal::operator = (BangBangServoMechanicalGimbal &&servoMechanicalGimbal)
{
    if (&servoMechanicalGimbal != this)
    {
        servoMechanicalGimbal.swap(*this);
    }

    return *this;
}

/**
 * Create() function
 */
BangBangServoMechanicalGimbal *BangBangServoMechanicalGimbal::create(void)
{
    auto *pBangBangServoMechanicalGimbal = new BangBangServoMechanicalGimbal();
    if (!pBangBangServoMechanicalGimbal->setup())
    {
        delete pBangBangServoMechanicalGimbal;
        pBangBangServoMechanicalGimbal = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Created an instance of \"" + pBangBangServoMechanicalGimbal->getClassName() + "\".\n",
               getQualifiedStaticMethodName(__func__, BangBangServoMechanicalGimbal));
    }

    return pBangBangServoMechanicalGimbal;
}

/**
 * Function to evaluate the dynamics model which represents this control system
 * @param t  time at which the dynamics model is evaluated
 * @param x  the state vector
 * @param xd the state derivative vector
 */
void BangBangServoMechanicalGimbal::dynamicsModel(double t,
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

    // test for command angle larger than threshold (invokes bang-bang operation)
    double commandAcceleration = 0.0;
    if (std::fabs(commandPosition) > m_threshold)
    {
        // bang-bang region

        // calculate the angular travel that would be required to bring the gimbal to a halt if the maximum
        // angular acceleration were applied in opposition to the present direction and rate of angular motion
        auto angStop = 0.5 * x[1] * x[1] / m_accelerationLimit;
        auto decelerationThreshold = std::max(angStop, m_threshold);
        auto signumCommandPosition = (commandPosition < 0.0 ? -1.0 : 1.0);
        if (std::fabs(commandPosition) > decelerationThreshold)
            commandAcceleration = signumCommandPosition * m_accelerationLimit; // accelerate
        else
        {
            if (m_pPositionLimiter != nullptr)
            {
                auto posLowerLimit = m_pPositionLimiter->getLowerLimit();
                auto posUpperLimit = m_pPositionLimiter->getUpperLimit();

                // test whether position limits have been reached; if so, flip the sign
                if (x[0] == posLowerLimit || x[0] == posUpperLimit)
                    signumCommandPosition = -signumCommandPosition;
            }

            auto signumRate = (x[1] < 0.0 ? -1.0 : 1.0);
            if (signumCommandPosition == signumRate)
            {
                // accelerate in other direction
                commandAcceleration = -signumCommandPosition * m_accelerationLimit;
            }
            else
                commandAcceleration = signumCommandPosition * m_accelerationLimit;
        }
    }
    else
    {
        // linear operation
        auto absAccelerationLimit = std::fabs(m_accelerationLimit);
        auto rateError = commandRate - x[1];
        commandAcceleration = 2 * m_zeta * m_omega * rateError;
        auto signumCommandAcceleration = (commandAcceleration < 0.0 ? -1.0 : 1.0);
        if (std::fabs(commandAcceleration) > absAccelerationLimit)
            commandAcceleration = signumCommandAcceleration * absAccelerationLimit;
    }

    xd[1] = commandAcceleration;
}

/**
 * Get the name of this class
 */
std::string BangBangServoMechanicalGimbal::getClassName(void) const
{
    return "BangBangServoMechanicalGimbal";
}

/**
 * Get the factory name of this constructible
 */
std::string BangBangServoMechanicalGimbal::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get the angular threshold between bang-bang and linear operation
 */
double BangBangServoMechanicalGimbal::getThreshold(void) const
{
    return m_threshold;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool BangBangServoMechanicalGimbal::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = ServoMechanicalGimbal::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pThresholdNode = pNode->first_node("threshold");
        if (pThresholdNode != nullptr)
            setThreshold(std::stod(pThresholdNode->value()));
    }

    return bSuccess;
}
#endif
/**
 * Set the angular threshold between bang-bang and linear operation
 */
void BangBangServoMechanicalGimbal::setThreshold(double threshold)
{
    m_threshold = threshold;
}

/**
 * Setup function
 */
bool BangBangServoMechanicalGimbal::setup(void)
{
    bool bSuccess = ServoMechanicalGimbal::setup();
    if (bSuccess)
    {
        m_registry["threshold"] = m_threshold;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void BangBangServoMechanicalGimbal::swap(BangBangServoMechanicalGimbal &servoMechanicalGimbal)
{
    ServoMechanicalGimbal::swap(servoMechanicalGimbal);

    std::swap(m_threshold, servoMechanicalGimbal.m_threshold);
}

}

}

}
