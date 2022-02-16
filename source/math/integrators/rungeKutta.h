#ifndef RUNGE_KUTTA_H
#define RUNGE_KUTTA_H

#include "cloneable.h"
#include "factory_constructible.h"
#include "file_configurable.h"
#include "initializable.h"
#include "loggable.h"
#ifdef RAPID_XML
#include "xml_constructible.h"
#include "xml_readable.h"
#endif
#include "reflective.h"
#include "setupable.h"
#include "swappable.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#include "xml_constructible.h"
#include "xml_readable.h"

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

// forward declarations
namespace control_systems { class StateVector; }

namespace integrators
{

// forward declarations
class ButcherTableau;

/**
 * This class serves as an abstract base for Runge-Kutta numerical differential equation solvers
 */
class RungeKutta
: public attributes::interfaces::Cloneable<RungeKutta>,
  public attributes::abstract::FactoryConstructible<RungeKutta>,
  public attributes::interfaces::FileConfigurable,
  public attributes::interfaces::Initializable,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::Swappable<RungeKutta>,
  public attributes::interfaces::TokenMapConfigurable,
#ifdef RAPID_XML
  public attributes::concrete::VariableMapRegistrable,
  public attributes::concrete::XML_Constructible<RungeKutta, rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>
#else
  public attributes::concrete::VariableMapRegistrable
#endif
{
public:

    /**
     * Type alias declarations
     */
    using StateVector = math::control_systems::StateVector;

    /**
     * Typedef declarations
     */
    typedef std::function<void (double, const StateVector &, StateVector &)> tStateDynamicsFunction;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

    /**
     * Constructor
     * @param butcherTableau a reference to this Runge-Kutta method's Butcher Tableau
     */
    EXPORT_STEM RungeKutta(const ButcherTableau &butcherTableau);

    /**
     * Copy constructor
     */
    EXPORT_STEM RungeKutta(const RungeKutta &rungeKutta);

    /**
     * Move constructor
     */
    EXPORT_STEM RungeKutta(RungeKutta &&rungeKutta);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~RungeKutta(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RungeKutta &operator = (const RungeKutta &rungeKutta);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RungeKutta &operator = (RungeKutta &&rungeKutta);

    /**
     * Function to retrieve the i, j-th coefficient from the Butcher Tableau for the particular Runge-Kutta
     * method implemented by this class
     */
    EXPORT_STEM virtual double butcherTableauCoefficient(std::size_t i,
                                                         std::size_t j);
    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(const std::string &filename) override;

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;
#ifdef RAPID_XML
    /**
     * Function to construct an instance from an XML node
     * @param pNode a pointer to an XML node
     */
    static EXPORT_STEM RungeKutta *createFromXML(rapidxml::xml_node<char> *pNode);
#endif
    /**
     * Retrieve a pointer to this Runge-Kutta method's Butcher Tableau
     */
    EXPORT_STEM virtual const ButcherTableau *getButcherTableau(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Function to numerically solve an ordinary differential equation
     * @param x        the state vector
     * @param dynamics a function object which formulates the dynamics as a series of first-order differential
     *                 equations; the function object's function call operator should accept three arguments,
     *                 the first two of which are the input time and state vector; the last argument contains
     *                 the computed state derivative on output.
     * @param t0       the start of the interval
     * @param t1       the end of the interval
     */
    EXPORT_STEM virtual bool solve(StateVector &x,
                                   tStateDynamicsFunction &dynamics,
                                   double t0,
                                   double t1);

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(RungeKutta &rungeKutta) override final;

protected:

    /**
     * a pointer to this Runge-Kutta method's Butcher Tableau
     */
    ButcherTableau *m_pButcherTableau;

    /**
     * a table of incremental derivatives (internal use only)
     */
    StateVector *m_pDerivativeTable;

    /**
     * the state vector
     */
    StateVector *m_pState;

    /**
     * the state derivative
     */
    StateVector *m_pStateDerivative;
};

}

}

#endif
