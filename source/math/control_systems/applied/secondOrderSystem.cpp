#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "secondOrderSystem.h"

// using namespace declarations
using namespace math::integrators;
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

/**
 * Constructor
 * @param omega   the natural frequency in radians
 * @param zeta    the damping ratio
 * @param command the command input to this second order system
 */
SecondOrderSystem::SecondOrderSystem(double omega,
                                     double zeta,
                                     double command)
: StateSpaceModel(2),
  m_command(command),
  m_omega(omega),
  m_zeta(zeta)
{

}

/**
 * Copy constructor
 */
SecondOrderSystem::SecondOrderSystem(const SecondOrderSystem &system)
{
    operator = (system);
}

/**
 * Move constructor
 */
SecondOrderSystem::SecondOrderSystem(SecondOrderSystem &&system)
{
    operator = (std::move(system));
}

/**
 * Destructor
 */
SecondOrderSystem::~SecondOrderSystem(void)
{

}

/**
 * Copy assignment operator
 */
SecondOrderSystem &SecondOrderSystem::operator = (const SecondOrderSystem &system)
{
    if (&system != this)
    {
        StateSpaceModel::operator = (system);

        m_command = system.m_command;
        m_omega = system.m_omega;
        m_zeta = system.m_zeta;
    }

    return *this;
}

/**
 * Move assignment operator
 */
SecondOrderSystem &SecondOrderSystem::operator = (SecondOrderSystem &&system)
{
    if (&system != this)
    {
        system.swap(*this);
    }

    return *this;
}

/**
 * Function to evaluate the dynamics model which represents this control system
 * @param t  time at which the dynamics model is evaluated
 * @param x  the state vector
 * @param xd the state derivative vector
 */
void SecondOrderSystem::dynamicsModel(double t,
                                      const StateVector &x,
                                      StateVector &xd)
{
    xd[0] = x[1];
    xd[1] = m_omega * (m_omega * (m_command - x[0]) - 2 * m_zeta * x[1]);
}

/**
 * Get the name of this class
 */
std::string SecondOrderSystem::getClassName(void) const
{
    return "SecondOrderSystem";
}

/**
 * Get the command input to this second order system
 */
double SecondOrderSystem::getCommand(void) const
{
    return m_command;
}

/**
 * Get the damping ratio
 */
double SecondOrderSystem::getDampingRatio(void)
{
    return m_zeta;
}

/**
 * Get the natural frequency (radians)
 */
double SecondOrderSystem::getNaturalFrequency(void)
{
    return m_omega;
}

/**
 * Get the time-domain response of this system; upon success, returns true populates output vector
 */
bool SecondOrderSystem::getResponse(StateVector &output)
{
    if (output.size() != 1)
        output.resize(1);

    output[0] = m_stateVector[0];

    return true;
}

/**
 * Initialization function
 */
bool SecondOrderSystem::initialize(void)
{
    m_stateVector.resize(2);
    m_stateVector[0] = 0.0;
    m_stateVector[1] = 0.0;

    return true;
}

/**
 * Return whether or not this system is critically-damped
 */
bool SecondOrderSystem::isCriticallyDamped(void) const
{
    return m_zeta == 1.0;
}

/**
 * Return whether or not this system is over-damped
 */
bool SecondOrderSystem::isOverDamped(void) const
{
    return m_zeta > 1.0;
}

/**
 * Return whether or not this system is under-damped
 */
bool SecondOrderSystem::isUnderDamped(void) const
{
    return m_zeta < 1.0;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool SecondOrderSystem::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = StateSpaceModel::readFromXML(pNode);
    if (bSuccess)
    {
        auto *pDampingRatioNode = pNode->first_node("dampingRatio");
        if (pDampingRatioNode != nullptr)
            m_zeta = std::stod(pDampingRatioNode->value());

        auto *pNaturalFrequencyNode = pNode->first_node("naturalFrequency");
        if (pNaturalFrequencyNode != nullptr)
            m_omega = std::stod(pNaturalFrequencyNode->value());
    }

    return bSuccess;
}
#endif
/**
 * Set the command input to this second order system
 */
void SecondOrderSystem::setCommand(double command)
{
    m_command = command;
}

/**
 * Set the damping ratio
 */
void SecondOrderSystem::setDampingRatio(double zeta)
{
    m_zeta = zeta;
}

/**
 * Set the natural frequency (radians)
 */
void SecondOrderSystem::setNaturalFrequency(double omega)
{
    m_omega = omega;
}

/**
 * Setup function
 */
bool SecondOrderSystem::setup(void)
{
    bool bSuccess = StateSpaceModel::setup();
    if (bSuccess)
    {
        m_registry["naturalFrequency"] = m_omega;
        m_registry["dampingRatio"] = m_zeta;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void SecondOrderSystem::swap(SecondOrderSystem &system)
{
    StateSpaceModel::swap(system);

    std::swap(m_omega, system.m_omega);
    std::swap(m_command, system.m_command);
    std::swap(m_zeta, system.m_zeta);
}

}

}

}
