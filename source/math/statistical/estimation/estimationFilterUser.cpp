#include "estimationFilter.h"
#include "estimationFilterUser.h"
#include "measurementAggregationStrategy.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "referenceFrame.h"
#include "standard_deviation.h"
#include "stateMap.h"
#include "stateVector.h"

// using namespace declarations
using namespace math::control_systems;
using namespace physics::kinematics;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace utilities;

namespace math
{

namespace statistical
{

namespace estimation
{

/**
 * A state measurement comparison functor
 */
struct StateMeasurementComparator
{
    /**
     * Function call operator
     */
    inline virtual bool operator () (StateMap *pStateMeasurement,
                                     double time) const final
    {
        return pStateMeasurement != nullptr && pStateMeasurement->getTime() < time;
    }

    /**
     * Function call operator
     */
    inline virtual bool operator () (double time,
                                     StateMap *pStateMeasurement) const final
    {
        return pStateMeasurement != nullptr && time < pStateMeasurement->getTime();
    }
};

/**
 * Constructor
 */
EstimationFilterUser::EstimationFilterUser(void)
: EstimationFilterUser(nullptr, nullptr)
{

}

/**
 * Constructor
 * @param pEstimationFilter               a pointer to the estimation filter associated with this object
 * @param pMeasurementAggregationStrategy a pointer to the measurement aggregation strategy
 */
EstimationFilterUser::
EstimationFilterUser(EstimationFilter *pEstimationFilter,
                     MeasurementAggregationStrategy *pMeasurementAggregationStrategy)
: m_pEstimationFilter(nullptr),
  m_pMeasurementAggregationStrategy(pMeasurementAggregationStrategy),
  m_pMeasurementStandardDeviations(new StateMap)
{
    setEstimationFilter(pEstimationFilter);
}

/**
 * Copy constructor
 */
EstimationFilterUser::
EstimationFilterUser(const EstimationFilterUser &estimationFilterUser)
: m_pEstimationFilter(nullptr),
  m_pMeasurementAggregationStrategy(nullptr),
  m_pMeasurementStandardDeviations(nullptr)
{
    operator = (estimationFilterUser);
}

/**
 * Move constructor
 */
EstimationFilterUser::
EstimationFilterUser(EstimationFilterUser &&estimationFilterUser)
: m_pEstimationFilter(nullptr),
  m_pMeasurementAggregationStrategy(nullptr),
  m_pMeasurementStandardDeviations(nullptr)
{
    operator = (std::move(estimationFilterUser));
}

/**
 * Destructor
 */
EstimationFilterUser::~EstimationFilterUser(void)
{
    if (m_pEstimationFilter != nullptr)
        m_pEstimationFilter->setEstimationFilterUser(nullptr);

    if (m_pMeasurementAggregationStrategy != nullptr)
        delete m_pMeasurementAggregationStrategy;

    if (m_pMeasurementStandardDeviations != nullptr)
        delete m_pMeasurementStandardDeviations;
}

/**
 * Copy assignment operator
 */
EstimationFilterUser &EstimationFilterUser::
operator = (const EstimationFilterUser &estimationFilterUser)
{
    if (&estimationFilterUser != this)
    {
        Loggable<std::string, std::ostream>::operator = (estimationFilterUser);

        if (m_pMeasurementAggregationStrategy != nullptr)
        {
            delete m_pMeasurementAggregationStrategy;
            m_pMeasurementAggregationStrategy = nullptr;
        }

        if (estimationFilterUser.m_pMeasurementAggregationStrategy != nullptr)
        {
            m_pMeasurementAggregationStrategy = estimationFilterUser.
                                                m_pMeasurementAggregationStrategy->clone();
            m_pMeasurementAggregationStrategy->setEstimationFilterUser(this);
        }

        if (m_pMeasurementStandardDeviations == nullptr)
            m_pMeasurementStandardDeviations = new StateMap();

        if (estimationFilterUser.m_pMeasurementStandardDeviations != nullptr)
            *m_pMeasurementStandardDeviations = *estimationFilterUser.m_pMeasurementStandardDeviations;

//      m_standardDeviationCalculators = estimationFilterUser.m_standardDeviationCalculators;
    }

    return *this;
}

/**
 * Move assignment operator
 */
EstimationFilterUser &EstimationFilterUser::
operator = (EstimationFilterUser &&estimationFilterUser)
{
    if (&estimationFilterUser != this)
    {
        estimationFilterUser.swap(*this);
    }

    return *this;
}

/**
 * Add a state measurement; returns non-null upon success; returns true if the entry is successfully added and
 * the current object takes ownership of the input argument and becomes responsible for its destruction
 */
bool EstimationFilterUser::addStateMeasurement(StateMap *pStateMeasurement)
{
    bool bSuccess = (pStateMeasurement != nullptr);
    if (bSuccess)
    {
        pStateMeasurement->setName("measured");

        bSuccess = addEntry(pStateMeasurement);
    }

    return bSuccess;
}

/**
 * Add a state measurement; returns non-null upon success
 */
StateMap *EstimationFilterUser::addStateMeasurement(StateMap &stateMeasurement)
{
    auto *pStateMeasurement = stateMeasurement.clone();

    bool bSuccess = addStateMeasurement(pStateMeasurement);
    if (!bSuccess && pStateMeasurement != nullptr)
    {
        delete pStateMeasurement;

        pStateMeasurement = nullptr;
    }

    return pStateMeasurement;
}
#ifdef RAPID_XML
/**
 * Function to construct an instance from an XML node
 * @param pNode             a pointer to an XML node
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
EstimationFilterUser *
EstimationFilterUser::createFromXML(xml_node<> *pNode,
                                    EstimationFilter *pEstimationFilter)
{
    EstimationFilterUser *pEstimationFilterUser = nullptr;
    if (pNode != nullptr && std::strcmp(pNode->name(), "estimationFilterUser") == 0)
    {
        auto *pTypeAttribute = pNode->first_attribute("type");
        if (pTypeAttribute != nullptr)
        {
            std::string name("estimation_filter_user");
            auto &&type = pTypeAttribute->value();
            pEstimationFilterUser = EstimationFilterUser::create(type,
                                                                 pEstimationFilter);
            if (pEstimationFilterUser != nullptr)
            {
                pEstimationFilterUser->readFromXML(pNode);
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create object of type \"" + std::string(type) + "\".\n",
                       getQualifiedStaticMethodName(__func__, EstimationFilterUser));
            }
        }
    }

    return pEstimationFilterUser;
}
#endif
/**
 * Delete a state measurement by value from this object; sets input argument to null upon success
 */
bool EstimationFilterUser::deleteStateMeasurement(StateMap *&pStateMeasurement)
{
    return deleteEntry(pStateMeasurement);
}

/**
 * Delete all state measurements tagged between the specified starting and ending times
 */
void EstimationFilterUser::deleteStateMeasurements(double startTime,
                                                   double endTime)
{
    deleteEntries("measured", startTime, endTime);
}

/**
 * Delete all state measurements from this object
 */
void EstimationFilterUser::deleteStateMeasurements(void)
{
    deleteEntries("measured");
}

/**
 * Estimate the standard deviations of this object's state measurement associated with the specified name
 * @param name            the name associated with the state measurements for which the standard deviation will
 *                        be calculated
 * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
 */
double EstimationFilterUser::estimateMeasurementStandardDeviation(const std::string &name,
                                                                  bool bBiasedEstimate)
{
    if (m_standardDeviationCalculators[name] == nullptr)
        m_standardDeviationCalculators[name].reset(new StandardDeviation<double>());

    m_standardDeviationCalculators[name]->setBiasedEstimate(bBiasedEstimate);

    std::vector<StateMap *> stateMeasurements;
    getEntries("measured", stateMeasurements);
    auto &&itStateMeasurement = stateMeasurements.cbegin();
    while (itStateMeasurement != stateMeasurements.cend())
    {
        auto *pStateMeasurement = *itStateMeasurement;
        if (pStateMeasurement != nullptr)
        {
            double measurement;
            if (pStateMeasurement->get(name, measurement))
            {
                m_standardDeviationCalculators[name]->addSample(measurement);
            }
        }

        ++itStateMeasurement;
    }

    auto standardDeviation = m_standardDeviationCalculators[name]->calculate();
    m_standardDeviationCalculators[name]->initialize();

    return standardDeviation;
}

/**
 * Estimate the standard deviations of this object's state measurements
 * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
 */
bool EstimationFilterUser::estimateMeasurementStandardDeviations(bool bBiasedEstimate)
{
    bool bSuccess = (m_pMeasurementStandardDeviations != nullptr);
    if (bSuccess)
    {
        std::vector<StateMap *> stateMeasurements;
        getEntries("measured", stateMeasurements);
        auto &&itStateMeasurement = stateMeasurements.cbegin();
        while (bSuccess && itStateMeasurement != stateMeasurements.cend())
        {
            auto *pStateMeasurement = *itStateMeasurement;
            bSuccess = (pStateMeasurement != nullptr);
            if (bSuccess)
            {
                std::for_each(pStateMeasurement->cbegin(),
                              pStateMeasurement->cend(),
                              [this, bBiasedEstimate] (auto &&nameMeasurementPair)
                              {
                                  auto &&name = nameMeasurementPair.first;
                                  if (m_standardDeviationCalculators[name] == nullptr)
                                      m_standardDeviationCalculators[name].reset(new StandardDeviation<double>());

                                  auto &&measurement = nameMeasurementPair.second;
                                  m_standardDeviationCalculators[name]->setBiasedEstimate(bBiasedEstimate);
                                  m_standardDeviationCalculators[name]->addSample(measurement);
                              });
            }

            ++itStateMeasurement;
        }

        std::for_each(m_standardDeviationCalculators.cbegin(),
                      m_standardDeviationCalculators.cend(),
                      [this] (auto &&nameStandardDeviationCalculatorPair)
                      {
                          auto &&name = nameStandardDeviationCalculatorPair.first;
                          auto &&pStandardDeviationCalculator = nameStandardDeviationCalculatorPair.second;
                          auto standardDeviation = pStandardDeviationCalculator->calculate();
                          pStandardDeviationCalculator->initialize();
                          m_pMeasurementStandardDeviations->set(name, standardDeviation);
                      });
    }

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string EstimationFilterUser::getClassName(void) const
{
    return "EstimationFilterUser";
}

/**
 * Get a pointer to the estimation filter associated with this object
 */
EstimationFilter *EstimationFilterUser::getEstimationFilter(void) const
{
    return m_pEstimationFilter;
}

/**
 * Retrieve this object's latest state measurement; returns non-null upon success
 */
StateMap *EstimationFilterUser::getLatestStateMeasurement(void) const
{
    return getLatestEntry("measured");
}

/**
 * Get this object's measurement aggregation strategy
 */
MeasurementAggregationStrategy *EstimationFilterUser::getMeasurementAggregationStrategy(void) const
{
    return m_pMeasurementAggregationStrategy;
}

/**
 * Get this object's state map containing the measurement standard deviations associated with this object's
 * state measurements
 */
StateMap *EstimationFilterUser::getMeasurementStandardDeviations(void) const
{
    return m_pMeasurementStandardDeviations;
}

/**
 * Retrieve the most recently available state measurement; returns non-null upon success
 */
StateMap *EstimationFilterUser::getMostRecentAvailableStateMeasurement(double time) const
{
    return getMostRecentAvailableEntry("measured", time);
}

/**
 * Get the standard deviation calculator associated with the specified name
 */
StandardDeviation<double> *EstimationFilterUser::getStandardDeviationCalculator(const std::string &name)
{
    if (m_standardDeviationCalculators[name] == nullptr)
        m_standardDeviationCalculators[name].reset(new StandardDeviation<double>());

    return m_standardDeviationCalculators[name].get();
}

/**
 * Retrieve a state measurement tagged at the specified time; returns non-null upon success
 */
StateMap *EstimationFilterUser::getStateMeasurement(double time) const
{
    return getEntry("measured", time);
}

/**
 * Retrieve all state measurements tagged between the specified starting and ending times; upon success,
 * entries are appended to input argument
 */
bool EstimationFilterUser::getStateMeasurements(double startTime,
                                                double endTime,
                                                std::vector<StateMap *> &stateMeasurements) const
{
    return getEntries("measured", startTime, endTime, stateMeasurements);
}

/**
 * Get this object's state measurements
 */
std::vector<StateMap *> &EstimationFilterUser::getStateMeasurements(void)
{
    auto &&stateMeasurementsMap = getEntries();

    return stateMeasurementsMap["measured"];
}

/**
 * Get this object's state measurements
 */
std::vector<StateMap *> EstimationFilterUser::getStateMeasurements(void) const
{
    auto &&itStateMap = m_entries.find("measured");
    if (itStateMap != m_entries.cend())
    {
        return itStateMap->second;
    }

    return { };
}

/**
 * Initialization function
 */
bool EstimationFilterUser::initialize(void)
{
    bool bSuccess = (m_pMeasurementAggregationStrategy != nullptr);
    if (bSuccess)
    {
        bSuccess = tIdentifierAndTimeSortedContainer::initialize();
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Measurement aggregation strategy is null!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool EstimationFilterUser::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "estimationFilterUser") == 0);
    if (bSuccess)
    {
        // read this estimation filter user's measurement aggregation strategy
        auto *pMeasurementAggregationStrategyNode = pNode->first_node("measurementAggregationStrategy");
        if (pMeasurementAggregationStrategyNode != nullptr)
        {
            // attempt to create a measurement aggregation strategy
            auto *pMeasurementAggregationStrategy = MeasurementAggregationStrategy::
                                                    createFromXML(pMeasurementAggregationStrategyNode,
                                                                  this);
            bSuccess = (pMeasurementAggregationStrategy != nullptr);
            if (bSuccess)
            {
                m_pMeasurementAggregationStrategy = pMeasurementAggregationStrategy;
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create a measurement aggregation strategy!\n",
                       getQualifiedMethodName(__func__));
            }
        }
    }

    return bSuccess;
}
#endif
/**
 * Remove a state measurement by value from this object
 */
bool EstimationFilterUser::removeStateMeasurement(StateMap *pStateMeasurement)
{
    return removeEntry(pStateMeasurement);
}

/**
 * Remove all state measurements from this object
 */
void EstimationFilterUser::removeStateMeasurements(void)
{
    removeEntries("measured");
}

/**
 * Remove all state measurements tagged between the specified starting and ending times
 */
void EstimationFilterUser::removeStateMeasurements(double startTime,
                                                   double endTime)
{
    removeEntries("measured", startTime, endTime);
}

/**
 * Select this object's measurement aggregation strategy; returns true upon success
 */
bool EstimationFilterUser::
selectMeasurementAggregationStrategy(const std::string &measurementAggregationStrategy)
{
    if (m_pMeasurementAggregationStrategy != nullptr)
    {
        delete m_pMeasurementAggregationStrategy;
        m_pMeasurementAggregationStrategy = nullptr;
    }

    auto *pMeasurementAggregationStrategy = MeasurementAggregationStrategy::
                                            create(measurementAggregationStrategy,
                                                   this);
    bool bSuccess = (pMeasurementAggregationStrategy != nullptr);
    if (!bSuccess)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to create measurement aggregation strategy!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Set a pointer to the estimation filter associated with this object
 */
void EstimationFilterUser::setEstimationFilter(EstimationFilter *pEstimationFilter)
{
    if (m_pEstimationFilter != pEstimationFilter)
    {
        m_pEstimationFilter = pEstimationFilter;
        if (pEstimationFilter != nullptr)
            pEstimationFilter->setEstimationFilterUser(this);
    }
}

/**
 * Set this object's measurement aggregation strategy; the current object takes ownership of the input argument
 * and destroys the currently selected measurement aggregation strategy upon success
 */
void EstimationFilterUser::
setMeasurementAggregationStrategy(MeasurementAggregationStrategy *pMeasurementAggregationStrategy)
{
    if (m_pMeasurementAggregationStrategy != pMeasurementAggregationStrategy)
    {
        if (m_pMeasurementAggregationStrategy != nullptr)
        {
            delete m_pMeasurementAggregationStrategy;
            m_pMeasurementAggregationStrategy = nullptr;
        }

        m_pMeasurementAggregationStrategy = pMeasurementAggregationStrategy;
        if (m_pMeasurementAggregationStrategy != nullptr)
            m_pMeasurementAggregationStrategy->setEstimationFilterUser(this);
    }
}

/**
 * Set this object's state vector containing the measurement standard deviations associated with this object's
 * state measurements
 */
bool EstimationFilterUser::setMeasurementStandardDeviations(StateMap *pMeasurementStandardDeviations)
{
    bool bSuccess = (m_pMeasurementStandardDeviations != nullptr &&
                     pMeasurementStandardDeviations != nullptr);
    if (bSuccess)
    {
        *m_pMeasurementStandardDeviations = *pMeasurementStandardDeviations;
    }

    return bSuccess;
}

/**
 * Swap function
 */
void EstimationFilterUser::swap(EstimationFilterUser &estimationFilterUser)
{
    Loggable<std::string, std::ostream>::swap(estimationFilterUser);
    tIdentifierAndTimeSortedContainer::swap(estimationFilterUser);

    // swap the estimation filters
    auto *pEstimationFilter = estimationFilterUser.getEstimationFilter();
    estimationFilterUser.setEstimationFilter(m_pEstimationFilter);
    setEstimationFilter(pEstimationFilter);

    // swap the measurement aggregation strategies
    auto *pMeasurementAggregationStrategy = estimationFilterUser.getMeasurementAggregationStrategy();
    estimationFilterUser.setMeasurementAggregationStrategy(m_pMeasurementAggregationStrategy);
    setMeasurementAggregationStrategy(pMeasurementAggregationStrategy);

    std::swap(m_pMeasurementStandardDeviations,
              estimationFilterUser.m_pMeasurementStandardDeviations);
    m_standardDeviationCalculators.swap(estimationFilterUser.m_standardDeviationCalculators);
}

}

}

}
