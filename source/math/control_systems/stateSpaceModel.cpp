#include "dictionary.h"
#include "file_system.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "rungeKutta.h"
#include "stateSpaceModel.h"

// using namespace declarations
using namespace math::integrators;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace utilities;
using namespace utilities::file_system;

namespace math
{

namespace control_systems
{

/**
 * Constructor
 */
StateSpaceModel::StateSpaceModel(void)
: m_pRungeKuttaMethod(nullptr)
{
    setRungeKuttaMethod("RungeKutta4");
}

/**
 * Constructor
 * @param order the order of this control system
 */
StateSpaceModel::StateSpaceModel(std::size_t order)
: m_pRungeKuttaMethod(nullptr),
  m_stateVector(order)
{
    setRungeKuttaMethod("RungeKutta4");
}

/**
 * Constructor
 * @param stateVector initializes this control system's state vector
 */
StateSpaceModel::StateSpaceModel(const StateVector &stateVector)
: m_pRungeKuttaMethod(nullptr),
  m_stateVector(stateVector)
{
    if (m_pRungeKuttaMethod != nullptr)
        delete m_pRungeKuttaMethod;
}

/**
 * Copy constructor
 */
StateSpaceModel::StateSpaceModel(const StateSpaceModel &system)
{
    operator = (system);
}

/**
 * Move constructor
 */
StateSpaceModel::StateSpaceModel(StateSpaceModel &&system)
{
    operator = (std::move(system));
}

/**
 * Destructor
 */
StateSpaceModel::~StateSpaceModel(void)
{

}

/**
 * Copy assignment operator
 */
StateSpaceModel &StateSpaceModel::operator = (const StateSpaceModel &system)
{
    if (&system != this)
    {
        if (m_pRungeKuttaMethod != nullptr)
        {
            delete m_pRungeKuttaMethod;
            m_pRungeKuttaMethod = nullptr;
        }

        if (system.m_pRungeKuttaMethod != nullptr)
            m_pRungeKuttaMethod = system.m_pRungeKuttaMethod->clone();

        m_stateVector = system.m_stateVector;
    }

    return *this;
}

/**
 * Move assignment operator
 */
StateSpaceModel &StateSpaceModel::operator = (StateSpaceModel &&system)
{
    if (&system != this)
    {
        system.swap(*this);
    }

    return *this;
}

/**
 * Configure function (using input file or input string)
 */
bool StateSpaceModel::configure(const std::string &filename)
{
    std::ifstream stream;
    bool bSuccess = FileSystem::openFileStream(filename, stream);
    if (bSuccess)
    {
        // process this object's variable registry
        Dictionary dictionary(&m_registry);

        auto &&tokenMap = dictionary.createTokenPairs<tTokenMap>(stream);
        stream.close();

        bSuccess = configure(tokenMap);
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to open or parse \"" + filename + "\".\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Configure function (using a variable token map)
 */
bool StateSpaceModel::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}

/**
 * Get the name of this class
 */
std::string StateSpaceModel::getClassName(void) const
{
    return "StateSpaceModel";
}

/**
 * Get this object's Runge-Kutta method
 */
RungeKutta *StateSpaceModel::getRungeKuttaMethod(void) const
{
    return m_pRungeKuttaMethod;
}

/**
 * Get this object's state vector
 */
StateVector &StateSpaceModel::getStateVector(void)
{
    return m_stateVector;
}

/**
 * Get this object's state vector
 */
StateVector StateSpaceModel::getStateVector(void) const
{
    return m_stateVector;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool StateSpaceModel::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        auto *pRungeKuttaMethodNode = pNode->first_node("rungeKuttaMethod");
        if (pRungeKuttaMethodNode != nullptr)
        {
            auto *pRungeKuttaMethod = RungeKutta::createFromXML(pRungeKuttaMethodNode);
            bSuccess = (pRungeKuttaMethod != nullptr);
            if (bSuccess)
            {
                bSuccess = setRungeKuttaMethod(std::move(pRungeKuttaMethod));
            }

            if (!bSuccess)
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create Runge-Kutta method from XML node!\n",
                       getQualifiedMethodName(__func__));

                if (pRungeKuttaMethod != nullptr)
                    delete pRungeKuttaMethod;
            }
        }
    }

    return bSuccess;
}
#endif
/**
 * Set the type of Runge-Kutta method used by this object; returns true if the type exists and is successfully
 * created
 */
bool StateSpaceModel::setRungeKuttaMethod(const std::string &type)
{
    auto *pRungeKuttaMethod = RungeKutta::create(type);
    bool bSuccess = (pRungeKuttaMethod != nullptr);
    if (bSuccess)
    {
        if (m_pRungeKuttaMethod != nullptr)
            delete m_pRungeKuttaMethod;

        m_pRungeKuttaMethod = pRungeKuttaMethod;
    }

    return bSuccess;
}

/**
 * Set the type of Runge-Kutta method used by this object
 * @param[in,out] pRungeKuttaMethod a pointer to a Runge-Kutta integrator; object takes ownership of the input
 *                                  argument and becomes responsible for its destruction
 * @return                          true upon success
 */
bool StateSpaceModel::setRungeKuttaMethod(RungeKutta *&&pRungeKuttaMethod)
{
    bool bSuccess = (pRungeKuttaMethod != nullptr);
    if (bSuccess)
    {
        if (m_pRungeKuttaMethod != nullptr)
            delete m_pRungeKuttaMethod;

        m_pRungeKuttaMethod = std::move(pRungeKuttaMethod);
        pRungeKuttaMethod = nullptr;
    }

    return bSuccess;
}

/**
 * Set this object's state vector
 */
void StateSpaceModel::setStateVector(const StateVector &stateVector)
{
    m_stateVector = stateVector;
}

/**
 * Setup function
 */
bool StateSpaceModel::setup(void)
{
    // TODO: implement this

    return true;
}

/**
 * Swap function
 */
void StateSpaceModel::swap(StateSpaceModel &system)
{
    std::swap(m_pRungeKuttaMethod, system.m_pRungeKuttaMethod);
    m_stateVector.swap(system.m_stateVector);
}

/**
 * Update function
 */
bool StateSpaceModel::update(double time)
{
    bool bSuccess = (m_pRungeKuttaMethod != nullptr);
    if (bSuccess)
    {
        RungeKutta::tStateDynamicsFunction dynamics = [&] (double t, const StateVector &x, StateVector &xd)
        {
            dynamicsModel(t, x, xd);
        };

        auto t0 = m_stateVector.getTime();
        auto t1 = time;
        if (t1 > t0)
        {
            m_pRungeKuttaMethod->solve(m_stateVector, dynamics, t0, t1);
        }

        m_stateVector.setTime(t1);
    }
    else
    {
        logMsg(std::cout, utilities::LoggingLevel::Enum::Error,
               "Runge-Kutta method has not been set, cannot integrate state.\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

}

}
