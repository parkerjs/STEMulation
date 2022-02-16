#ifndef ESTIMATION_FILTER_INITIALIZER_H
#define ESTIMATION_FILTER_INITIALIZER_H

#include "export_library.h"
#include "factory_constructible.h"
#include "identifier_and_time_sorted_container.h"
#include "initializable.h"
#include "loggable.h"
#include "reflective.h"
#include "stateMap.h"
#ifdef RAPID_XML
#include "xml_constructible.h"
#include "xml_readable.h"
#endif

// forward declarations
namespace physics { namespace kinematics { class ReferenceFrame; } }
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

// forward declarations
namespace control_systems { class StateVector; }

namespace statistical
{

// forward declarations
template<typename> class StandardDeviation;

namespace estimation
{

// forward declarations
class EstimationFilter;
class MeasurementAggregationStrategy;

/**
 * This class provides an abstract interface from which users of estimation filters should derive
 */
class EstimationFilterUser
: public attributes::abstract::FactoryConstructible<EstimationFilterUser>,
  public containers::IdentifierAndTimeSortedContainer<control_systems::StateMap,
                                                      std::string,
                                                      double,
                                                      &control_systems::StateMap::getName,
                                                      &control_systems::StateMap::getAvailabilityTime>,
  virtual public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
#ifdef RAPID_XML
  public attributes::interfaces::Swappable<EstimationFilterUser>,
  public attributes::concrete::XML_Constructible<EstimationFilterUser, rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>
#else
  public attributes::interfaces::Swappable<EstimationFilterUser>
#endif
{
public:

    /**
     * Friend declarations
     */
    friend EstimationFilter;

    /**
     * Type alias declarations
     */
    using ReferenceFrame = physics::kinematics::ReferenceFrame;
    using StateMap = control_systems::StateMap;
    using StateVector = control_systems::StateVector;

    /**
     * Using declarations
     */
    using tIdentifierAndTimeSortedContainer::swap;
    using Loggable<std::string, std::ostream>::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM EstimationFilterUser(void);

    /**
     * Constructor
     * @param pEstimationFilter               a pointer to the estimation filter associated with this object
     * @param pMeasurementAggregationStrategy a pointer to the measurement aggregation strategy
     */
    EXPORT_STEM EstimationFilterUser(EstimationFilter *pEstimationFilter,
                                     MeasurementAggregationStrategy *pMeasurementAggregationStrategy = nullptr);

    /**
     * Copy constructor
     */
    EXPORT_STEM EstimationFilterUser(const EstimationFilterUser &estimationFilterUser);

    /**
     * Move constructor
     */
    EXPORT_STEM EstimationFilterUser(EstimationFilterUser &&estimationFilterUser);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~EstimationFilterUser(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM EstimationFilterUser &operator = (const EstimationFilterUser &estimationFilterUser);

    /**
     * Move assignment operator
     */
    EXPORT_STEM EstimationFilterUser &operator = (EstimationFilterUser &&estimationFilterUser);

public:

    /**
     * Add a state measurement; returns non-null upon success; returns true if the entry is successfully added
     * and the current object takes ownership of the input argument and becomes responsible for its destruction
     */
    EXPORT_STEM virtual bool addStateMeasurement(StateMap *pStateMeasurement) final;

    /**
     * Add a state measurement; returns non-null upon success
     */
    EXPORT_STEM virtual StateMap *addStateMeasurement(StateMap &stateMeasurement) final;

    /**
     * Create an estimation filter user of the indicated type
     * @param type the type of estimation filter user to be instantiated
     */
    template<typename ... Args>
    inline static EstimationFilterUser *create(const std::string &type,
                                               Args && ... args)
    {
        return FactoryConstructible<EstimationFilterUser>::create(type, std::forward<Args>(args) ...);
    }
#ifdef RAPID_XML
    /**
     * Function to construct an instance from an XML node
     * @param pNode             a pointer to an XML node
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    static EXPORT_STEM EstimationFilterUser *createFromXML(rapidxml::xml_node<char> *pNode,
                                                           EstimationFilter *pEstimationFilter);
#endif
    /**
     * Create a measurement aggregation strategy of the indicated type; returns non-null and destroys this
     * object's currently selected measurement aggregation strategy upon success
     * @param type the type of applied estimation filter to be created
     */
    template<typename ... Args>
    inline MeasurementAggregationStrategy *createMeasurementAggregationStrategy(const std::string &type,
                                                                                Args && ... args)
    {
        return attributes::abstract::
               FactoryConstructible<MeasurementAggregationStrategy>::create(type,
                                                                            this,
                                                                            std::forward<Args>(args) ...);
    }

    /**
     * Delete a state measurement by value from this object; sets input argument to null upon success
     */
    EXPORT_STEM virtual bool deleteStateMeasurement(StateMap *&pStateMeasurement) final;

    /**
     * Delete all state measurements tagged between the specified starting and ending times
     */
    EXPORT_STEM virtual void deleteStateMeasurements(double startTime,
                                                     double endTime) final;

    /**
     * Delete all state measurements from this object
     */
    EXPORT_STEM virtual void deleteStateMeasurements(void) final;

    /**
     * Function to estimate the initial state from this object's measurements
     */
    EXPORT_STEM virtual bool estimateInitialState(StateVector &stateEstimate) = 0;

    /**
     * Estimate the standard deviations of this object's state measurement associated with the specified name
     * @param name            the name associated with the state measurements for which the standard deviation
     *                        will be calculated
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    EXPORT_STEM virtual double estimateMeasurementStandardDeviation(const std::string &name,
                                                                    bool bBiasedEstimate = false);

    /**
     * Estimate the standard deviations of this object's state measurements
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    EXPORT_STEM virtual bool estimateMeasurementStandardDeviations(bool bBiasedEstimate = false);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM virtual EstimationFilter *getEstimationFilter(void) const final;

    /**
     * Retrieve this object's latest state measurement; returns non-null upon success
     */
    EXPORT_STEM virtual StateMap *getLatestStateMeasurement(void) const final;

    /**
     * Get this object's measurement aggregation strategy
     */
    EXPORT_STEM virtual MeasurementAggregationStrategy *getMeasurementAggregationStrategy(void) const final;

    /**
     * Get a pointer to the frame of reference associated with the measurement at the specified time
     */
    EXPORT_STEM virtual ReferenceFrame *getMeasurementFrame(double time) const = 0;

    /**
     * Get this object's state map containing the measurement standard deviations associated with this object's
     * state measurements
     */
    EXPORT_STEM virtual StateMap *getMeasurementStandardDeviations(void) const final;

    /**
     * Retrieve the most recently available state measurement; returns non-null upon success
     */
    EXPORT_STEM virtual StateMap *getMostRecentAvailableStateMeasurement(double time) const final;

    /**
     * Get the standard deviation calculator associated with the specified name
     */
    EXPORT_STEM virtual StandardDeviation<double> *
    getStandardDeviationCalculator(const std::string &name) final;

    /**
     * Get a pointer to the frame of reference associated with the state estimate at the specified time
     */
    EXPORT_STEM virtual ReferenceFrame *getStateEstimateFrame(double time) const = 0;

    /**
     * Retrieve a state measurement tagged at the specified time; returns non-null upon success
     */
    EXPORT_STEM virtual StateMap *getStateMeasurement(double time) const final;

    /**
     * Retrieve all state measurements tagged between the specified starting and ending times; upon success,
     * entries are appended to input argument
     */
    EXPORT_STEM virtual bool getStateMeasurements(double startTime,
                                                  double endTime,
                                                  std::vector<StateMap *> &stateMeasurements) const final;

    /**
     * Get this object's state measurements
     */
    EXPORT_STEM virtual std::vector<StateMap *> &getStateMeasurements(void) final;

    /**
     * Get this object's state measurements
     */
    EXPORT_STEM virtual std::vector<StateMap *> getStateMeasurements(void) const final;

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
     * Remove a state measurement by value from this object
     */
    EXPORT_STEM virtual bool removeStateMeasurement(StateMap *pStateMeasurement) final;

    /**
     * Remove all state measurements from this object
     */
    EXPORT_STEM virtual void removeStateMeasurements(void) final;

    /**
     * Remove all state measurements tagged between the specified starting and ending times
     */
    EXPORT_STEM virtual void removeStateMeasurements(double startTime,
                                                     double endTime) final;

    /**
     * Select this object's measurement aggregation strategy; returns true upon success
     */
    EXPORT_STEM virtual bool selectMeasurementAggregationStrategy(const std::string &
                                                                  measurementAggregationStrategy) final;

    /**
     * Set a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM void setEstimationFilter(EstimationFilter *pEstimationFilter);

    /**
     * Set this object's measurement aggregation strategy; the current object takes ownership of the input
     * argument and destroys the currently selected measurement aggregation strategy upon success
     */
    EXPORT_STEM virtual void
    setMeasurementAggregationStrategy(MeasurementAggregationStrategy *
                                      pMeasurementAggregationStrategy) final;

    /**
     * Set this object's state vector containing the measurement standard deviations associated with this
     * object's state measurements
     */
    EXPORT_STEM virtual bool
    setMeasurementStandardDeviations(StateMap *pMeasurementStandardDeviations) final;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(EstimationFilterUser &estimationFilterUser) override final;

protected:

    /**
     * a pointer to the estimation filter associated with this object
     */
    EstimationFilter *m_pEstimationFilter;

    /**
     * a pointer to the measurement aggregation strategy
     */
    MeasurementAggregationStrategy *m_pMeasurementAggregationStrategy;

    /**
     * a pointer to a state vector object containing the measurement noise standard deviations associated with
     * this object's state measurements
     */
    StateMap *m_pMeasurementStandardDeviations;

    /**
     * a map of standard deviation calculators
     */
    std::map<std::string, std::unique_ptr<StandardDeviation<double>>> m_standardDeviationCalculators;
};

}

}

}

#endif
