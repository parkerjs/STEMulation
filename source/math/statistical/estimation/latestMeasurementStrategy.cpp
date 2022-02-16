#include "finite_difference.h"
#include "latestMeasurementStrategy.h"
#include "standard_deviation.h"
#include "stateMap.h"

// file-scoped variables
static constexpr char factoryName[] = "LatestMeasurementStrategy";

// using namespace declarations
using namespace attributes::abstract;
using namespace math::control_systems;
using namespace math::differentiators::sequence;
using namespace math::statistical;

namespace math
{

namespace statistical
{

namespace estimation
{

// register factories...
static FactoryRegistrar<MeasurementAggregationStrategy>
factory(factoryName, &LatestMeasurementStrategy::create);

/**
 * Constructor
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
LatestMeasurementStrategy::
LatestMeasurementStrategy(EstimationFilterUser *pEstimationFilterUser)
: MeasurementAggregationStrategy(pEstimationFilterUser)
{

}

/**
 * Destructor
 */
LatestMeasurementStrategy::~LatestMeasurementStrategy(void)
{

}

/**
 * Function to aggregate a collection of measurement samples
 * @param stateMeasurements         a vector of state measurement objects
 * @param aggregateStateMeasurement upon success, will contain the result of the aggregation
 */
bool LatestMeasurementStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                          StateMap &aggregateStateMeasurement)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        auto *pStateMeasurement = stateMeasurements.back();
        aggregateStateMeasurement = *pStateMeasurement;
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
bool LatestMeasurementStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
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
bool LatestMeasurementStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
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
LatestMeasurementStrategy *LatestMeasurementStrategy::clone(void) const
{
    return new LatestMeasurementStrategy(*this);
}

/**
 * create() function
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
LatestMeasurementStrategy *
LatestMeasurementStrategy::create(EstimationFilterUser *pEstimationFilterUser)
{
    return new LatestMeasurementStrategy(pEstimationFilterUser);
}

/**
 * Function to estimate derivatives for the given collection of measurement samples
 */
bool LatestMeasurementStrategy::
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
            auto &&measurements = itStateMeasurementVector->second;
            auto &&name = itStateMeasurementVector->first;
            differentiator.calcDerivative(time,
                                          measurements,
                                          derivative);
            stateMeasurementDerivative[name] = derivative.back();

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
bool LatestMeasurementStrategy::
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
std::string LatestMeasurementStrategy::getClassName(void) const
{
    return "LatestMeasurementStrategy";
}

/**
 * Get the factory name of this constructible
 */
std::string LatestMeasurementStrategy::getFactoryName(void) const
{
    return factoryName;
}

}

}

}
