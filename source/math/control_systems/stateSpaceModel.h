#ifndef STATE_SPACE_MODEL_H
#define STATE_SPACE_MODEL_H

#include "file_configurable.h"
#include "initializable.h"
#include "loggable.h"
#include "setupable.h"
#include "stateVector.h"
#include "swappable.h"
#include "token_map_configurable.h"
#include "updateable.h"
#include "variable_map_registrable.h"
#ifdef RAPID_XML
#include "xml_readable.h"
#endif
#include <cmath>

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

// forward declarations
namespace integrators { class RungeKutta; }

namespace control_systems
{

/**
 * This class provides an abstract base class for subclasses that implement state space models
 */
class StateSpaceModel
: public attributes::interfaces::FileConfigurable,
  public attributes::interfaces::Initializable,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::Swappable<StateSpaceModel>,
  public attributes::interfaces::TokenMapConfigurable,
  public attributes::interfaces::Updateable,
#ifdef RAPID_XML
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>,
  public attributes::concrete::VariableMapRegistrable
#else
  public attributes::concrete::VariableMapRegistrable
#endif
{
public:

    /**
     * Type alias declarations
     */
    using RungeKutta = integrators::RungeKutta;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

    /**
     * Constructor
     */
    EXPORT_STEM StateSpaceModel(void);

    /**
     * Constructor
     * @param order the order of this control system
     */
    EXPORT_STEM StateSpaceModel(std::size_t order);

    /**
     * Constructor
     * @param stateVector initializes this control system's state vector
     */
    EXPORT_STEM StateSpaceModel(const StateVector &stateVector);

    /**
     * Copy constructor
     */
    EXPORT_STEM StateSpaceModel(const StateSpaceModel &system);

    /**
     * Move constructor
     */
    EXPORT_STEM StateSpaceModel(StateSpaceModel &&system);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~StateSpaceModel(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM StateSpaceModel &operator = (const StateSpaceModel &system);

    /**
     * Move assignment operator
     */
    EXPORT_STEM StateSpaceModel &operator = (StateSpaceModel &&system);

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(const std::string &filename) override;

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * Function to evaluate the dynamics model which represents this control system
     * @param t  time at which the dynamics model is evaluated
     * @param x  the state vector
     * @param xd the state derivative vector
     */
    EXPORT_STEM virtual void dynamicsModel(double t,
                                           const StateVector &x,
                                           StateVector &xd) = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the time-domain response of this system; upon success, returns true populates output vector
     */
    EXPORT_STEM virtual bool getResponse(StateVector &output) = 0;

    /**
     * Get this object's Runge-Kutta method
     */
    EXPORT_STEM virtual RungeKutta *getRungeKuttaMethod(void) const final;

    /**
     * Get this object's state vector
     */
    EXPORT_STEM virtual StateVector &getStateVector(void) final;

    /**
     * Get this object's state vector
     */
    EXPORT_STEM virtual StateVector getStateVector(void) const final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set the type of Runge-Kutta method used by this object; returns true if the type exists and is
     * successfully created
     */
    EXPORT_STEM virtual bool setRungeKuttaMethod(const std::string &type) final;

    /**
     * Set the type of Runge-Kutta method used by this object
     * @param[in,out] pRungeKuttaMethod a pointer to a Runge-Kutta integrator; object takes ownership of the
     *                                  input argument and becomes responsible for its destruction
     * @return                          true upon success
     */
    EXPORT_STEM virtual bool setRungeKuttaMethod(RungeKutta *&&pRungeKuttaMethod) final;

    /**
     * Set this object's state vector
     */
    EXPORT_STEM virtual void setStateVector(const StateVector &stateVector) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(StateSpaceModel &system) override final;

    /**
     * Update function
     */
    EXPORT_STEM virtual bool update(double time) override;

protected:

    /**
     * the Runge-Kutta method used to integrate this model's dynamics
     */
    RungeKutta *m_pRungeKuttaMethod;

    /**
     * this object's state vector
     */
    StateVector m_stateVector;
};

}

}

#endif
