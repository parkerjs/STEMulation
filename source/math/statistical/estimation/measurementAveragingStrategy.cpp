#include "finite_difference.h"
#include "measurementAveragingStrategy.h"
#include "standard_deviation.h"
#include "stateMap.h"
#include <numeric>

// file-scoped variables
static constexpr char factoryName[] = "MeasurementAveragingStrategy";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::differentiators::sequence;
using namespace math::statistical;
using namespace utilities;

namespace math
{

namespace statistical
{

namespace estimation
{

// register factories...
static FactoryRegistrar<MeasurementAggregationStrategy>
factory(factoryName, &MeasurementAveragingStrategy::create);

/**
 * Constructor
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
MeasurementAveragingStrategy::
MeasurementAveragingStrategy(EstimationFilterUser *pEstimationFilterUser)
: MeasurementAggregationStrategy(pEstimationFilterUser)
{

}

/**
 * Destructor
 */
MeasurementAveragingStrategy::~MeasurementAveragingStrategy(void)
{

}

/**
 * Function to aggregate a collection of measurement samples
 * @param stateMeasurements         a vector of state measurement objects
 * @param aggregateStateMeasurement upon success, will contain the result of the aggregation
 */
bool MeasurementAveragingStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                             StateMap &aggregateStateMeasurement)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        aggregateStateMeasurement.clear();
        std::map<std::string, std::size_t> measurementCountMap;
        auto &&itStateMeasurementVector = stateMeasurements.cbegin();
        while (bSuccess && itStateMeasurementVector != stateMeasurements.cend())
        {
            auto *pStateMeasurement = *itStateMeasurementVector;
            bSuccess = (pStateMeasurement != nullptr);
            if (bSuccess)
            {
                auto &&itStateMeasurement = pStateMeasurement->cbegin();
                while (itStateMeasurement != pStateMeasurement->cend())
                {
                    auto &&nameMeasurementPair = *itStateMeasurement;
                    auto &&name = nameMeasurementPair.first;
                    auto &&measurement = nameMeasurementPair.second;

                    if (!aggregateStateMeasurement.contains(name))
                    {
                        aggregateStateMeasurement[name] = 0.0;
                        measurementCountMap[name] = 0;
                    }

                    aggregateStateMeasurement[name] += measurement;
                    ++measurementCountMap[name];
                    ++itStateMeasurement;
                }
            }

            ++itStateMeasurementVector;
        }

        auto &&itStateMeasurement = aggregateStateMeasurement.begin();
        while (itStateMeasurement != aggregateStateMeasurement.end())
        {
            auto &&name = itStateMeasurement->first;
            itStateMeasurement->second /= measurementCountMap[name];
            ++itStateMeasurement;
        }
    }

    return bSuccess;
}

/**
 * Function to aggregate a collection of measurement samples
 * @param stateMeasurements          a vector of state measurement objects
 * @param aggregateStateMeasurement  upon success, will contain the result of the aggregation
 * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
 *                                   samples
 */
bool MeasurementAveragingStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                             StateMap &aggregateStateMeasurement,
                                             StateMap &stateMeasurementDerivative)
{
    bool bSuccess = aggregate(stateMeasurements, aggregateStateMeasurement);
    if (bSuccess)
    {
        bSuccess = estimateDerivatives(stateMeasurements, stateMeasurementDerivative);
    }

    return bSuccess;
}

/**
 * Function to aggregate a collection of measurement samples
 * @param stateMeasurements                  a vector of state measurement objects
 * @param aggregateStateMeasurement          upon success, will contain the result of the aggregation
 * @param stateMeasurementDerivative         upon success, will contain the estimated derivative of the
 *                                           measurement samples
 * @param stateMeasurementStandardDeviations upon success, will contain the estimated standard deviations of
 *                                           the measurement samples
 */
bool MeasurementAveragingStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                             StateMap &aggregateStateMeasurement,
                                             StateMap &stateMeasurementDerivative,
                                             StateMap &stateMeasurementStandardDeviations)
{
    bool bSuccess = aggregate(stateMeasurements,
                              aggregateStateMeasurement,
                              stateMeasurementDerivative);
    if (bSuccess)
    {
        bSuccess = estimateStandardDeviations(stateMeasurements,
                                              stateMeasurementStandardDeviations);
    }

    return bSuccess;
}

/**
 * clone() function
 */
MeasurementAveragingStrategy *MeasurementAveragingStrategy::clone(void) const
{
    auto *pMeasurementAveragingStrategy = new MeasurementAveragingStrategy(*this);
    if (false /*!pMeasurementAveragingStrategy->setup()*/) // TODO: implement a setup() function?
    {
        delete pMeasurementAveragingStrategy;
        pMeasurementAveragingStrategy = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Cloned an instance of \"" + pMeasurementAveragingStrategy->getClassName() + "\".\n",
               getQualifiedMethodName(__func__));
    }

    return pMeasurementAveragingStrategy;
}

/**
 * create() function
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
MeasurementAveragingStrategy *
MeasurementAveragingStrategy::create(EstimationFilterUser *pEstimationFilterUser)
{
    auto *pMeasurementAveragingStrategy = new MeasurementAveragingStrategy(pEstimationFilterUser);
    if (false /*!pMeasurementAveragingStrategy->setup()*/) // TODO: implement a setup() function?
    {
        delete pMeasurementAveragingStrategy;
        pMeasurementAveragingStrategy = nullptr;
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Debug,
               "Created an instance of \"" + pMeasurementAveragingStrategy->getClassName() + "\".\n",
               getQualifiedStaticMethodName(__func__, MeasurementAveragingStrategy));
    }

    return pMeasurementAveragingStrategy;
}

/**
 * Function to estimate derivatives for the given collection of measurement samples
 * @param stateMeasurements          a vector of state measurement objects
 * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
 *                                   samples
 */
bool MeasurementAveragingStrategy::
estimateDerivatives(const std::vector<StateMap *> &stateMeasurements,
                    StateMap &stateMeasurementDerivative)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        std::vector<double> derivative, time;
        std::map<std::string, std::vector<double>> stateMeasurementVectorMap;
        StateMap::toMap(stateMeasurements,
                        stateMeasurementVectorMap,
                        time);

        FiniteDifference<double> differentiator;
        auto &&itStateMeasurementVector = stateMeasurementVectorMap.cbegin();
        while (itStateMeasurementVector != stateMeasurementVectorMap.cend())
        {
            auto &&stateMeasurements = itStateMeasurementVector->second;
            auto &&name = itStateMeasurementVector->first;
            differentiator.calcDerivative(time,
                                          stateMeasurements,
                                          derivative);

            stateMeasurementDerivative[name] = std::accumulate(derivative.cbegin(),
                                                               derivative.cend(), 0.0) / derivative.size();

            ++itStateMeasurementVector;
        }
    }

    return bSuccess;
}

/**
 * Function to estimate standard deviations for the given collection of measurement samples
 * @param stateMeasurements                  a vector of state measurement objects
 * @param stateMeasurementStandardDeviations upon success, will contain the estimated standard deviations of
 *                                           the measurement samples
 */
bool MeasurementAveragingStrategy::
estimateStandardDeviations(const std::vector<StateMap *> &stateMeasurements,
                           StateMap &stateMeasurementStandardDeviations)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        std::vector<double> derivative, time;
        std::map<std::string, std::vector<double>> stateMeasurementVectorMap;
        StateMap::toMap(stateMeasurements,
                        stateMeasurementVectorMap,
                        time);

        StandardDeviation<double> standardDeviationCalculator;
        auto &&itStateMeasurementVector = stateMeasurementVectorMap.cbegin();
        while (itStateMeasurementVector != stateMeasurementVectorMap.cend())
        {
            auto &&measurements = itStateMeasurementVector->second;
            auto &&name = itStateMeasurementVector->first;
            standardDeviationCalculator.addSamples(measurements);
            stateMeasurementStandardDeviations[name] = standardDeviationCalculator.calculate();
            standardDeviationCalculator.initialize();
            ++itStateMeasurementVector;
        }
    }

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string MeasurementAveragingStrategy::getClassName(void) const
{
    return "MeasurementAveragingStrategy";
}

/**
 * Get the factory name of this constructible
 */
std::string MeasurementAveragingStrategy::getFactoryName(void) const
{
    return factoryName;
}

}

}

}
