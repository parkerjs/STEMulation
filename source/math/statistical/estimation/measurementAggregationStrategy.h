#ifndef MEASUREMENT_AGGREGATION_STRATEGY_H
#define MEASUREMENT_AGGREGATION_STRATEGY_H

#include "cloneable.h"
#include "factory_constructible.h"
#include "loggable.h"
#include "reflective.h"
#include "swappable.h"
#ifdef RAPID_XML
#include "xml_constructible.h"
#include "xml_readable.h"
#endif

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

// forward declarations
namespace control_systems { class StateMap; }

namespace statistical
{

namespace estimation
{

// forward declarations
class EstimationFilterUser;

/**
 * This class provides an abstract interface for derived classes to implement methods which perform aggregation
 * on collections of measurement samples
 */
class MeasurementAggregationStrategy
: public attributes::interfaces::Cloneable<MeasurementAggregationStrategy>,
  public attributes::abstract::FactoryConstructible<MeasurementAggregationStrategy>,
  public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<MeasurementAggregationStrategy>,
  public attributes::concrete::XML_Constructible<MeasurementAggregationStrategy, rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<MeasurementAggregationStrategy>
#endif
{
public:

    /**
     * Friend declarations
     */
    friend EstimationFilterUser;

    /**
     * Type alias declarations
     */
    using StateMap = math::control_systems::StateMap;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

protected:

    /**
     * Constructor
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    EXPORT_STEM MeasurementAggregationStrategy(EstimationFilterUser *pEstimationFilterUser);

    /**
     * Copy constructor
     */
    EXPORT_STEM MeasurementAggregationStrategy(const MeasurementAggregationStrategy &
                                               measurementAggregationStrategy);

    /**
     * Move constructor
     */
    EXPORT_STEM MeasurementAggregationStrategy(MeasurementAggregationStrategy &&measurementAggregationStrategy);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~MeasurementAggregationStrategy(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM MeasurementAggregationStrategy &
    operator = (const MeasurementAggregationStrategy &measurementAggregationStrategy);

    /**
     * Move assignment operator
     */
    EXPORT_STEM MeasurementAggregationStrategy &
    operator = (MeasurementAggregationStrategy &&measurementAggregationStrategy);

public:

    /**
     * Function to aggregate a collection of measurement samples
     * @param stateMeasurements         a vector of state measurement objects
     * @param aggregateStateMeasurement upon success, will contain the result of the aggregation
     */
    EXPORT_STEM virtual bool aggregate(const std::vector<StateMap *> &stateMeasurements,
                                       StateMap &aggregateStateMeasurement) = 0;

    /**
     * Function to aggregate a collection of measurement samples
     * @param stateMeasurements          a vector of state measurement objects
     * @param aggregateStateMeasurement  upon success, will contain the result of the aggregation
     * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
     *                                   samples
     */
    EXPORT_STEM virtual bool aggregate(const std::vector<StateMap *> &stateMeasurements,
                                       StateMap &aggregateStateMeasurement,
                                       StateMap &stateMeasurementDerivative) = 0;

    /**
     * Function to aggregate a collection of measurement samples
     * @param stateMeasurements                  a vector of state measurement objects
     * @param aggregateStateMeasurement          upon success, will contain the result of the aggregation
     * @param stateMeasurementDerivative         upon success, will contain the estimated derivative of the
     *                                           measurement samples
     * @param stateMeasurementStandardDeviations upon success, will contain the estimated standard deviations
     *                                           of the measurement samples
     */
    EXPORT_STEM virtual bool aggregate(const std::vector<StateMap *> &stateMeasurements,
                                       StateMap &aggregateStateMeasurement,
                                       StateMap &stateMeasurementDerivative,
                                       StateMap &stateMeasurementStandardDeviations) = 0;

    /**
     * clone() function
     */
    EXPORT_STEM virtual MeasurementAggregationStrategy *clone(void) const override = 0;

    /**
     * Create a measurement aggregation strategy of the indicated type
     * @param type                  the type of measurement aggregation strategy to be instantiated
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    template<typename ... Args>
    inline static MeasurementAggregationStrategy *
    create(const std::string &type,
           EstimationFilterUser *pEstimationFilterUser,
           Args && ... args)
    {
        return FactoryConstructible<MeasurementAggregationStrategy>::create(type,
                                                                            pEstimationFilterUser,
                                                                            std::forward<Args>(args) ...);
    }
#ifdef RAPID_XML
    /**
     * Function to construct an instance from an XML node
     * @param pNode                 a pointer to an XML node
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    static EXPORT_STEM MeasurementAggregationStrategy *
    createFromXML(rapidxml::xml_node<char> *pNode,
                  EstimationFilterUser *pEstimationFilterUser);
#endif
    /**
     * Function to estimate derivatives for the given collection of measurement samples
     * @param stateMeasurements          a vector of state measurement objects
     * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
     *                                   samples
     */
    EXPORT_STEM virtual bool estimateDerivatives(const std::vector<StateMap *> &stateMeasurements,
                                                 StateMap &stateMeasurementDerivative) = 0;

    /**
     * Function to estimate standard deviations for the given collection of measurement samples
     * @param stateMeasurements                  a vector of state measurement objects
     * @param stateMeasurementStandardDeviations upon success, will contain the estimated standard deviations
     *                                           of the measurement samples
     */
    EXPORT_STEM virtual
    bool estimateStandardDeviations(const std::vector<StateMap *> &stateMeasurements,
                                    StateMap &stateMeasurementStandardDeviations) = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get a pointer to the estimation filter user associated with this object
     */
    EXPORT_STEM virtual EstimationFilterUser *getEstimationFilterUser(void) final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set a pointer to the estimation filter user associated with this object
     */
    EXPORT_STEM void setEstimationFilterUser(EstimationFilterUser *pEstimationFilterUser);

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(MeasurementAggregationStrategy &measurementAggregationStrategy) override final;

protected:

    /**
     * a pointer to an estimation filter user associated with this object
     */
    EstimationFilterUser *m_pEstimationFilterUser;
};

}

}

}

#endif
