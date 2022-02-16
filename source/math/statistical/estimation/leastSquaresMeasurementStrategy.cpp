#include "leastSquaresMeasurementStrategy.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateMap.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "LeastSquaresMeasurementStrategy";

// using namespace declarations
using namespace attributes::abstract;
#ifdef RAPID_XML
using namespace rapidxml;
#endif

namespace math
{

namespace statistical
{

namespace estimation
{

// register factories...
static FactoryRegistrar<MeasurementAggregationStrategy>
factory(factoryName, &LeastSquaresMeasurementStrategy::create);

/**
 * Constructor
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
LeastSquaresMeasurementStrategy::
LeastSquaresMeasurementStrategy(EstimationFilterUser *pEstimationFilterUser)
: MeasurementAggregationStrategy(pEstimationFilterUser)
{

}

/**
 * Copy constructor
 */
LeastSquaresMeasurementStrategy::
LeastSquaresMeasurementStrategy(const LeastSquaresMeasurementStrategy &measurementAggregationStrategy)
: MeasurementAggregationStrategy(measurementAggregationStrategy)
{
    operator = (measurementAggregationStrategy);
}

/**
 * Move constructor
 */
LeastSquaresMeasurementStrategy::
LeastSquaresMeasurementStrategy(LeastSquaresMeasurementStrategy &&measurementAggregationStrategy)
: MeasurementAggregationStrategy(std::move(measurementAggregationStrategy))
{
    operator = (std::move(measurementAggregationStrategy));
}

/**
 * Destructor
 */
LeastSquaresMeasurementStrategy::~LeastSquaresMeasurementStrategy(void)
{

}

/**
 * Copy assignment operator
 */
LeastSquaresMeasurementStrategy &
LeastSquaresMeasurementStrategy::operator = (const LeastSquaresMeasurementStrategy &measurementAggregationStrategy)
{
    if (&measurementAggregationStrategy != this)
    {
        MeasurementAggregationStrategy::operator = (measurementAggregationStrategy);

        m_interceptMap = measurementAggregationStrategy.m_interceptMap;
        m_measurementCountMap = measurementAggregationStrategy.m_measurementCountMap;
        m_sigmaMap = measurementAggregationStrategy.m_sigmaMap;
        m_slopeMap = measurementAggregationStrategy.m_slopeMap;
        m_xySumMap = measurementAggregationStrategy.m_xySumMap;
        m_ySumMap = measurementAggregationStrategy.m_ySumMap;
    }

    return *this;
}

/**
 * Move assignment operator
 */
LeastSquaresMeasurementStrategy &
LeastSquaresMeasurementStrategy::operator = (LeastSquaresMeasurementStrategy &&measurementAggregationStrategy)
{
    if (&measurementAggregationStrategy != this)
    {
        measurementAggregationStrategy.swap(*this);
    }

    return *this;
}

/**
 * Function to aggregate a collection of measurement samples
 * @param stateMeasurements         a vector of state measurement objects
 * @param aggregateStateMeasurement upon success, will contain the result of the aggregation
 */
bool LeastSquaresMeasurementStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                                StateMap &aggregateStateMeasurement)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        double xSum = 0.0, xxSum = 0.0;
        m_measurementCountMap.clear();
        m_xySumMap.clear();
        m_ySumMap.clear();
        auto &&itStateMeasurementVector = stateMeasurements.cbegin();
        while (bSuccess && itStateMeasurementVector != stateMeasurements.cend())
        {
            auto *pStateMeasurement = *itStateMeasurementVector;
            bSuccess = (pStateMeasurement != nullptr);
            if (bSuccess)
            {
                auto t = pStateMeasurement->getTime();
                xSum += t;
                xxSum += t * t;

                auto &&itStateMeasurement = pStateMeasurement->cbegin();
                while (itStateMeasurement != pStateMeasurement->cend())
                {
                    auto &&nameMeasurementPair = *itStateMeasurement;
                    auto &&name = nameMeasurementPair.first;
                    auto &&y = nameMeasurementPair.second;
                    if (m_xySumMap.find(name) == m_xySumMap.cend())
                        m_xySumMap[name] = 0.0;

                    if (m_ySumMap.find(name) == m_ySumMap.cend())
                        m_ySumMap[name] = 0.0;

                    m_xySumMap[name] += t * y;
                    m_ySumMap[name] += y;

                    if (m_measurementCountMap.find(name) == m_measurementCountMap.cend())
                        m_measurementCountMap[name] = 0;

                    ++m_measurementCountMap[name];
                    ++itStateMeasurement;
                }
            }

            ++itStateMeasurementVector;
        }

        // calculate the slopes
        auto &&m = stateMeasurements.size();
        auto denominator = m * xxSum - xSum * xSum;
        if (denominator != 0.0)
        {
            m_slopeMap.clear();
            auto &&itMeasurementCountMap = m_measurementCountMap.cbegin();
            while (itMeasurementCountMap != m_measurementCountMap.cend())
            {
                auto &&name = itMeasurementCountMap->first;
                auto n = itMeasurementCountMap->second;
                auto xySum = m_xySumMap[name];
                auto ySum = m_ySumMap[name];
                auto numerator = n * xySum - xSum * ySum;
                auto slope = numerator / denominator;
                m_slopeMap[name] = slope;

                ++itMeasurementCountMap;
            }

            m_interceptMap.clear();
            aggregateStateMeasurement.clear();
            auto t = stateMeasurements.back()->getTime();
            auto &&itSlope = m_slopeMap.cbegin();
            while (itSlope != m_slopeMap.cend())
            {
                // calculate the intercepts and the aggregate state measurement
                auto &&name = itSlope->first;
                auto slope = itSlope->second;
                auto n = m_measurementCountMap[name];
                auto ySum = m_ySumMap[name];
                auto intercept = (ySum - slope * xSum) / n;
                m_interceptMap[name] = intercept;

                // calculate the aggregate state measurement at the most recent time
                auto y = intercept + slope * t;
                aggregateStateMeasurement[name] = y;

                ++itSlope;
            }
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
bool LeastSquaresMeasurementStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                                StateMap &aggregateStateMeasurement,
                                                StateMap &stateMeasurementDerivative)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        bSuccess = aggregate(stateMeasurements,
                             aggregateStateMeasurement);
        if (bSuccess)
        {
            stateMeasurementDerivative.clear();
            auto &&itSlope = m_slopeMap.cbegin();
            while (itSlope != m_slopeMap.cend())
            {
                auto &&name = itSlope->first;
                auto slope = itSlope->second;
                stateMeasurementDerivative[name] = slope;

                ++itSlope;
            }
        }
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
bool LeastSquaresMeasurementStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                                StateMap &aggregateStateMeasurement,
                                                StateMap &stateMeasurementDerivative,
                                                StateMap &stateMeasurementStandardDeviations)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        bSuccess = aggregate(stateMeasurements,
                             aggregateStateMeasurement,
                             stateMeasurementDerivative);
        if (bSuccess)
        {
            bSuccess = computeRegressionStandardError(stateMeasurements,
                                                      stateMeasurementStandardDeviations);
        }
    }

    return bSuccess;
}

/**
 * clone() function
 */
LeastSquaresMeasurementStrategy *LeastSquaresMeasurementStrategy::clone(void) const
{
    return new LeastSquaresMeasurementStrategy(*this);
}

/**
 * Function to compute the regression standard error of the measurements relative to the least squares fit
 * @param stateMeasurements       a vector of state measurement objects
 * @param regressionStandardError upon success, will contain the computed regression standard errors
 */
bool LeastSquaresMeasurementStrategy::
computeRegressionStandardError(const std::vector<StateMap *> &stateMeasurements,
                               StateMap &regressionStandardError)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        m_sigmaMap.clear();
        m_ySumMap.clear();
        auto &&itStateMeasurementVector = stateMeasurements.cbegin();
        while (bSuccess && itStateMeasurementVector != stateMeasurements.cend())
        {
            auto *pStateMeasurement = *itStateMeasurementVector;
            bSuccess = (pStateMeasurement != nullptr);
            if (bSuccess)
            {
                auto t = pStateMeasurement->getTime();
                auto &&itStateMeasurement = pStateMeasurement->cbegin();
                while (itStateMeasurement != pStateMeasurement->cend())
                {
                    auto &&nameMeasurementPair = *itStateMeasurement;
                    auto &&name = nameMeasurementPair.first;
                    auto intercept = m_interceptMap[name];
                    auto slope = m_slopeMap[name];
                    auto y = nameMeasurementPair.second;

                    // calculate the residual
                    auto residual = y - slope * t - intercept;
                    if (m_sigmaMap.find(name) == m_sigmaMap.cend())
                        m_sigmaMap[name] = 0.0;

                    m_sigmaMap[name] += residual * residual;
                    if (m_ySumMap.find(name) == m_ySumMap.cend())
                        m_ySumMap[name] = 0.0;

                    m_ySumMap[name] += residual;

                    ++itStateMeasurement;
                }
            }

            ++itStateMeasurementVector;
        }

        regressionStandardError.clear();
        auto &&itSigmas = m_sigmaMap.cbegin();
        while (itSigmas != m_sigmaMap.cend())
        {
            auto &&name = itSigmas->first;
            auto n = m_measurementCountMap[name];
            auto avgResiduals = m_ySumMap[name] / n;
            auto sumResidualsSquared = m_sigmaMap[name];
            m_sigmaMap[name] = std::sqrt((sumResidualsSquared - avgResiduals * avgResiduals) / (n - 1));
            regressionStandardError[name] = m_sigmaMap[name];

            ++itSigmas;
        }
    }

    return bSuccess;
}

/**
 * create() function
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
LeastSquaresMeasurementStrategy *
LeastSquaresMeasurementStrategy::create(EstimationFilterUser *pEstimationFilterUser)
{
    return new LeastSquaresMeasurementStrategy(pEstimationFilterUser);
}

/**
 * Function to estimate derivatives for the given collection of measurement samples
 * @param stateMeasurements          a vector of state measurement objects
 * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
 *                                   samples
 */
bool LeastSquaresMeasurementStrategy::
estimateDerivatives(const std::vector<StateMap *> &stateMeasurements,
                    StateMap &stateMeasurementDerivative)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        // we don't care about the result from a call to aggregate(), but as a by-product it will calculate
        // the slope and intercept that results from the linear least squares fit
        bSuccess = aggregate(stateMeasurements,
                             stateMeasurementDerivative);
        if (bSuccess)
        {
            stateMeasurementDerivative.clear();
            auto &&itSlope = m_slopeMap.cbegin();
            while (itSlope != m_slopeMap.cend())
            {
                auto &&name = itSlope->first;
                auto &&slope = itSlope->second;
                stateMeasurementDerivative[name] = slope;

                ++itSlope;
            }
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
bool LeastSquaresMeasurementStrategy::
estimateStandardDeviations(const std::vector<StateMap *> &stateMeasurements,
                           StateMap &stateMeasurementStandardDeviations)
{
    bool bSuccess = (!stateMeasurements.empty());
    if (bSuccess)
    {
        // we don't care about the result from a call to aggregate(), but as a by-product it will calculate
        // the slope and intercept that results from the linear least squares fit
        StateMap stateMeasurementDerivative;
        bSuccess = aggregate(stateMeasurements,
                             stateMeasurementDerivative,
                             stateMeasurementStandardDeviations);
    }

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string LeastSquaresMeasurementStrategy::getClassName(void) const
{
    return "LeastSquaresMeasurementStrategy";
}

/**
 * Get the factory name of this constructible
 */
std::string LeastSquaresMeasurementStrategy::getFactoryName(void) const
{
    return factoryName;
}

/**
 * Get this object's vector of intercepts
 */
std::map<std::string, double> &LeastSquaresMeasurementStrategy::getIntercepts(void)
{
    return m_interceptMap;
}

/**
 * Get the standard deviation of the measurements relative to the least squares fit
 */
std::map<std::string, double> &LeastSquaresMeasurementStrategy::getSigmas(void)
{
    return m_sigmaMap;
}

/**
 * Get this object's vector of slopes
 */
std::map<std::string, double> &LeastSquaresMeasurementStrategy::getSlopes(void)
{
    return m_slopeMap;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool LeastSquaresMeasurementStrategy::readFromXML(xml_node<char> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "measurementAggregationStrategy") == 0);
    if (bSuccess)
    {
        // TODO: implement this
    }

    return bSuccess;
}
#endif
/**
 * Swap function
 */
void LeastSquaresMeasurementStrategy::swap(LeastSquaresMeasurementStrategy &measurementAggregationStrategy)
{
    MeasurementAggregationStrategy::swap(measurementAggregationStrategy);

    m_interceptMap.swap(measurementAggregationStrategy.m_interceptMap);
    m_measurementCountMap.swap(measurementAggregationStrategy.m_measurementCountMap);
    m_sigmaMap.swap(measurementAggregationStrategy.m_sigmaMap);
    m_slopeMap.swap(measurementAggregationStrategy.m_slopeMap);
    m_xySumMap.swap(measurementAggregationStrategy.m_xySumMap);
    m_ySumMap.swap(measurementAggregationStrategy.m_ySumMap);
}

}

}

}
