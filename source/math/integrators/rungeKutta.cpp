#include "butcherTableau.h"
#include "dictionary.h"
#include "file_system.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "rungeKutta.h"
#include "stateVector.h"

// using namespace declarations
using namespace math::control_systems;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace utilities;
using namespace utilities::file_system;

namespace math
{

namespace integrators
{

/**
 * Constructor
 * @param butcherTableau a reference to this Runge-Kutta method's Butcher Tableau
 */
RungeKutta::RungeKutta(const ButcherTableau &butcherTableau)
: m_pButcherTableau(new ButcherTableau(butcherTableau)),
  m_pDerivativeTable(new StateVector()),
  m_pState(new StateVector()),
  m_pStateDerivative(new StateVector())
{

}

/**
 * Copy constructor
 */
RungeKutta::RungeKutta(const RungeKutta &rungeKutta)
: m_pButcherTableau(nullptr),
  m_pDerivativeTable(nullptr),
  m_pState(nullptr),
  m_pStateDerivative(nullptr)
{
    operator = (rungeKutta);
}

/**
 * Move constructor
 */
RungeKutta::RungeKutta(RungeKutta &&rungeKutta)
: m_pButcherTableau(nullptr),
  m_pDerivativeTable(nullptr),
  m_pState(nullptr),
  m_pStateDerivative(nullptr)
{
    operator = (std::move(rungeKutta));
}

/**
 * Destructor
 */
RungeKutta::~RungeKutta(void)
{
    if (m_pButcherTableau != nullptr)
        delete m_pButcherTableau;

    if (m_pDerivativeTable != nullptr)
        delete m_pDerivativeTable;

    if (m_pState != nullptr)
        delete m_pState;

    if (m_pStateDerivative != nullptr)
        delete m_pStateDerivative;
}

/**
 * Copy assignment operator
 */
RungeKutta &RungeKutta::operator = (const RungeKutta &rungeKutta)
{
    if (&rungeKutta != this)
    {
        if (rungeKutta.m_pButcherTableau != nullptr)
        {
            if (m_pButcherTableau != nullptr)
                *m_pButcherTableau = *rungeKutta.m_pButcherTableau;
            else
                m_pButcherTableau = rungeKutta.m_pButcherTableau->clone();
        }
        else
        {
            // this should not happen...
            throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                                     "Input object's ButcherTableau is null!");
        }

        if (m_pDerivativeTable == nullptr)
            m_pDerivativeTable = new StateVector();

        if (rungeKutta.m_pDerivativeTable != nullptr)
            *m_pDerivativeTable = *rungeKutta.m_pDerivativeTable;

        if (m_pState == nullptr)
            m_pState = new StateVector();

        if (rungeKutta.m_pState != nullptr)
            *m_pState = *rungeKutta.m_pState;

        if (m_pStateDerivative == nullptr)
            m_pStateDerivative = new StateVector();

        if (rungeKutta.m_pStateDerivative != nullptr)
            *m_pStateDerivative = *rungeKutta.m_pStateDerivative;
    }

    return *this;
}

/**
 * Move assignment operator
 */
RungeKutta &RungeKutta::operator = (RungeKutta &&rungeKutta)
{
    if (&rungeKutta != this)
    {
        rungeKutta.swap(*this);
    }

    return *this;
}

/**
 * Function to retrieve the i, j-th coefficient from the Butcher Tableau for the particular Runge-Kutta method
 * implemented by this class
 */
double RungeKutta::butcherTableauCoefficient(std::size_t i,
                                             std::size_t j)
{
    if (m_pButcherTableau != nullptr)
    {
        return m_pButcherTableau->coefficient(i, j);
    }

    // this should not happen...
    throw std::runtime_error("Exception thrown from " + getQualifiedMethodName(__func__) + ": " +
                             "ButcherTableau is null!");
}

/**
 * Configure function (using input file or input string)
 */
bool RungeKutta::configure(const std::string &filename)
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
bool RungeKutta::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    return dictionary.populate(tokenMap);
}
#ifdef RAPID_XML
/**
 * Function to construct an instance from an XML node
 * @param pNode a pointer to an XML node
 */
RungeKutta *RungeKutta::createFromXML(xml_node<> *pNode)
{
    RungeKutta *pRungeKutta = nullptr;
    if (pNode != nullptr &&
        std::strcmp(pNode->name(), "rungeKutta") == 0)
    {
        auto *pAttribute = pNode->first_attribute("type");
        if (pAttribute != nullptr)
        {
            auto &&type = pAttribute->value();
            pRungeKutta = FactoryConstructible<RungeKutta>::create(type);
            if (pRungeKutta != nullptr)
            {
                pRungeKutta->readFromXML(pNode);
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create Runge-Kutta ode solver of type \"" + std::string(type) + "\"!\n",
                       getQualifiedStaticMethodName(__func__, RungeKutta));
            }
        }
        else
        {
            logMsg(std::cout, LoggingLevel::Enum::Warning,
                   "Runge-Kutta ode solver XML node elements must specify a 'type' attribute!\n",
                   getQualifiedStaticMethodName(__func__, RungeKutta));
        }
    }

    return pRungeKutta;
}
#endif
/**
 * Retrieve a pointer to this Runge-Kutta method's Butcher Tableau
 */
const ButcherTableau *RungeKutta::getButcherTableau(void) const
{
    return m_pButcherTableau;
}

/**
 * Get the name of this class
 */
std::string RungeKutta::getClassName(void) const
{
    return "RungeKutta";
}

/**
 * Initialization function
 */
bool RungeKutta::initialize(void)
{
    bool bSuccess = (m_pButcherTableau != nullptr);
    if (bSuccess)
    {
        if (m_pDerivativeTable == nullptr)
            m_pDerivativeTable = new StateVector();

        if (m_pState == nullptr)
            m_pState = new StateVector();

        if (m_pStateDerivative == nullptr)
            m_pStateDerivative = new StateVector();
    }

    return bSuccess;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool RungeKutta::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        // TODO: implement this
    }

    return bSuccess;
}
#endif
/**
 * Setup function
 */
bool RungeKutta::setup(void)
{
    // TODO: implement this

    return true;
}

/**
 * Function to numerically solve an ordinary differential equation
 * @param x        the state vector
 * @param dynamics a function object which formulates the dynamics as a series of first-order differential
 *                 equations; the function object's function call operator should accept three arguments, the
 *                 first two of which are the input time and state vector; the last argument contains the
 *                 computed state derivative on output.
 * @param t0       the start of the interval
 * @param t1       the end of the interval
 */
bool RungeKutta::solve(StateVector &x,
                       tStateDynamicsFunction &dynamics,
                       double t0,
                       double t1)
{
    bool bSuccess = (m_pButcherTableau != nullptr &&
                     m_pDerivativeTable != nullptr &&
                     m_pState != nullptr &&
                     m_pStateDerivative != nullptr);
    if (bSuccess)
    {
        auto n = x.size();
        if (m_pState->size() != n)
            m_pState->resize(n);

        if (m_pStateDerivative->size() != n)
            m_pStateDerivative->resize(n);

        auto &&butcherTableau = *m_pButcherTableau;
        auto s = butcherTableau.stages();
        if (m_pDerivativeTable->size() < s * n)
            m_pDerivativeTable->resize(s * n);

        // calculate the derivative (usually denoted as 'k' in literature) increments
        auto &&derivative = *m_pStateDerivative;
        auto &&derivativeTable = *m_pDerivativeTable;
        auto &&state = *m_pState;
        auto h = t1 > t0 ? t1 - t0 : t0 - t1;
        if (h > 0.0)
        {
            for (std::size_t i = 0; i < s; ++i)
            {
                std::copy(x.begin(), x.end(), state.begin());
                for (std::size_t j = 0; j < i; ++j)
                {
                    auto a_ij = butcherTableau.coefficient(i, j);
                    auto m = (i - 1) * n;
                    for (std::size_t k = 0; k < n; ++k)
                        state[k] += h * a_ij * derivativeTable[m + k];
                }

                auto c_i = butcherTableau.coefficient(i, 0);
                dynamics(t0 + h * c_i, state, derivative);

                auto m = i * n;
                for (std::size_t k = 0; k < n; ++k)
                    derivativeTable[m + k] = derivative[k];
            }

            for (std::size_t j = 0; j < s; ++j)
            {
                auto b_j = butcherTableau.coefficient(s, j + 1);
                auto m = j * n;
                for (std::size_t k = 0; k < n; ++k)
                    x[k] += h * b_j * derivativeTable[m + k];
            }
        }
    }

    return bSuccess;
}

/**
 * Swap function
 */
void RungeKutta::swap(RungeKutta &rungeKutta)
{
    Loggable<std::string, std::ostream>::swap(rungeKutta);

    std::swap(m_pButcherTableau, rungeKutta.m_pButcherTableau);
    std::swap(m_pState, rungeKutta.m_pState);
    std::swap(m_pStateDerivative, rungeKutta.m_pStateDerivative);
}

}

}
