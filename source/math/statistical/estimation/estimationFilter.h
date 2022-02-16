#ifndef ESTIMATION_FILTER_H
#define ESTIMATION_FILTER_H

#include "cloneable.h"
#include "factory_constructible.h"
#include "file_configurable.h"
#include "loggable.h"
#include "matrix2d.h"
#include "reflective.h"
#include "setupable.h"
#include "swappable.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#ifdef RAPID_XML
#include "xml_constructible.h"
#include "xml_readable.h"
#endif
#include <map>

namespace math
{

// forward declarations
namespace control_systems { class StateVector; }

namespace statistical
{

namespace estimation
{

// forward declarations
class AppliedEstimationFilter;
class EstimationFilterUser;

/**
 * This class provides an abstract base for derived types to implement statistical estimation filters
 */
class EstimationFilter
: public attributes::interfaces::Cloneable<EstimationFilter>,
  public attributes::abstract::FactoryConstructible<EstimationFilter>,
  public attributes::interfaces::FileConfigurable,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::Swappable<EstimationFilter>,
  public attributes::interfaces::TokenMapConfigurable,
#ifdef RAPID_XML
  public attributes::concrete::VariableMapRegistrable,
  public attributes::concrete::XML_Constructible<EstimationFilter, rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>
#else
  public attributes::concrete::VariableMapRegistrable
#endif
{
public:

    /**
     * Friend declarations
     */
    friend AppliedEstimationFilter;

    /**
     * Type alias declarations
     */
    using Matrix2d = linear_algebra::matrix::Matrix2d;
    using StateVector = control_systems::StateVector;
    using VariableRegistry = utilities::VariableRegistry;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM EstimationFilter(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM EstimationFilter(const EstimationFilter &estimationFilter);

    /**
     * Move constructor
     */
    EXPORT_STEM EstimationFilter(EstimationFilter &&estimationFilter);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~EstimationFilter(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM EstimationFilter &operator = (const EstimationFilter &estimationFilter);

    /**
     * Move assignment operator
     */
    EXPORT_STEM EstimationFilter &operator = (EstimationFilter &&estimationFilter);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual EstimationFilter *clone(void) const override = 0;

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(const std::string &filename) override;

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * Create an estimation filter of the indicated type
     * @param type the type of estimation filter to be instantiated
     */
    template<typename ... Args>
    inline static EstimationFilter *create(const std::string &type,
                                           Args && ... args)
    {
        return FactoryConstructible<EstimationFilter>::create(type,
                                                              std::forward<Args>(args) ...);
    }

    /**
     * Create an applied estimation filter of the indicated type; returns non-null and destroys this object's
     * currently selected applied estimation filter upon success
     * @param type the type of applied estimation filter to be created
     */
    template<typename ... Args>
    inline AppliedEstimationFilter *createAppliedEstimationFilter(const std::string &type,
                                                                  Args && ... args)
    {
        return attributes::abstract::
               FactoryConstructible<AppliedEstimationFilter>::create(type,
                                                                     this,
                                                                     std::forward<Args>(args) ...);
    }

    /**
     * Create an estimation filter user of the indicated type; returns non-null and destroys this object's
     * currently selected estimation filter user upon success
     * @param type the type of applied estimation filter to be created
     */
    template<typename ... Args>
    inline AppliedEstimationFilter *createEstimationFilterUser(const std::string &type,
                                                               Args && ... args)
    {
        return attributes::abstract::
               FactoryConstructible<AppliedEstimationFilter>::create(type,
                                                                     this,
                                                                     std::forward<Args>(args) ...);
    }
#ifdef RAPID_XML
    /**
     * Function to construct an instance from an XML node
     * @param pNode a pointer to an XML node
     */
    static EXPORT_STEM EstimationFilter *createFromXML(rapidxml::xml_node<char> *pNode);
#endif
    /**
     * Get the applied estimation filter associated with this object
     */
    EXPORT_STEM virtual AppliedEstimationFilter *getAppliedEstimationFilter(void) final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get a pointer to the estimation filter user associated with this object
     */
    EXPORT_STEM virtual EstimationFilterUser *getEstimationFilterUser(void) final;

    /**
     * Retrieve a matrix from this object's registry with the specified name
     */
    EXPORT_STEM virtual bool getMatrix(const std::string &name,
                                       Matrix2d &matrix) final;

    /**
     * Get the state estimate
     */
    EXPORT_STEM virtual void getStateEstimate(StateVector &stateEstimate) final;

    /**
     * Retrieve the update rate
     */
    EXPORT_STEM virtual double getUpdateRate(void) const final;

    /**
     * Initialization function
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual bool initialize(double dt) = 0;

    /**
     * Check for estimation filter initialization
     */
    EXPORT_STEM virtual bool initialized(void) const final;

    /**
     * Perform a measurement update
     * @param stateMeasurement a reference to a state vector object
     */
    EXPORT_STEM virtual bool measurementUpdate(StateVector &stateMeasurement) = 0;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set the applied estimation filter associated with this object; the current object takes ownership of the
     * input argument and destroys the currently selected applied estimation filter upon success
     */
    EXPORT_STEM virtual void setAppliedEstimationFilter(AppliedEstimationFilter *
                                                        pAppliedEstimationFilter) final;

    /**
     * Set a pointer to the estimation filter user associated with this object
     */
    EXPORT_STEM virtual void setEstimationFilterUser(EstimationFilterUser *pEstimationFilterUser) final;

    /**
     * Set a matrix within this object's registry and associate it with the specified name
     */
    EXPORT_STEM virtual bool setMatrix(const std::string &name,
                                       const Matrix2d &matrix);

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Set the update rate
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual void setUpdateRate(double dt);

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(EstimationFilter &estimationFilter) override final;

    /**
     * Perform a time update
     * @param u the control input vector
     */
    EXPORT_STEM virtual void timeUpdate(const Matrix2d &u = Matrix2d()) = 0;

protected:

    /**
     * initialization flag
     */
    bool m_bInitialized;

    /**
     * update rate (s)
     */
    double m_dt;

    /**
     * this object's applied estimation filter
     */
    AppliedEstimationFilter *m_pAppliedEstimationFilter;

    /**
     * this object's estimation filter user object
     */
    EstimationFilterUser *m_pEstimationFilterUser;

    /**
     * state estimate vector
     */
    Matrix2d m_xh;
};

}

}

}

#endif
