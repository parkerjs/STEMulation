#ifndef LEAST_SQUARES_MEASUREMENT_STRATEGY_H
#define LEAST_SQUARES_MEASUREMENT_STRATEGY_H

#include "measurementAggregationStrategy.h"

namespace math
{

namespace statistical
{

namespace estimation
{

/**
 * This class implements a measurement aggregation strategy in which a least-squares fit is applied to the
 * measurements and then used to initialize the state estimate
 */
class LeastSquaresMeasurementStrategy
: public MeasurementAggregationStrategy,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<LeastSquaresMeasurementStrategy>
{
public:

    /**
     * Using declarations
     */
    using MeasurementAggregationStrategy::swap;

protected:

    /**
     * Constructor
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    EXPORT_STEM LeastSquaresMeasurementStrategy(EstimationFilterUser *pEstimationFilterUser);

    /**
     * Copy constructor
     */
    EXPORT_STEM LeastSquaresMeasurementStrategy(const LeastSquaresMeasurementStrategy &
                                                measurementAggregationStrategy);

    /**
     * Move constructor
     */
    EXPORT_STEM LeastSquaresMeasurementStrategy(LeastSquaresMeasurementStrategy &&
                                                measurementAggregationStrategy);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~LeastSquaresMeasurementStrategy(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM LeastSquaresMeasurementStrategy &
    operator = (const LeastSquaresMeasurementStrategy &measurementAggregationStrategy);

    /**
     * Move assignment operator
     */
    EXPORT_STEM LeastSquaresMeasurementStrategy &
    operator = (LeastSquaresMeasurementStrategy &&measurementAggregationStrategy);

public:

    /**
     * Function to aggregate a collection of measurement samples
     * @param stateMeasurements         a vector of state measurement objects
     * @param aggregateStateMeasurement upon success, will contain the result of the aggregation
     */
    EXPORT_STEM virtual bool aggregate(const std::vector<StateMap *> &stateMeasurements,
                                       StateMap &aggregateStateMeasurement) override;

    /**
     * Function to aggregate a collection of measurement samples
     * @param stateMeasurements          a vector of state measurement objects
     * @param aggregateStateMeasurement  upon success, will contain the result of the aggregation
     * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
     *                                   samples
     */
    EXPORT_STEM virtual bool aggregate(const std::vector<StateMap *> &stateMeasurements,
                                       StateMap &aggregateStateMeasurement,
                                       StateMap &stateMeasurementDerivative) override;

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
                                       StateMap &stateMeasurementStandardDeviations) override;

    /**
     * clone() function
     */
    EXPORT_STEM virtual LeastSquaresMeasurementStrategy *clone(void) const override;

    /**
     * Function to compute the regression standard error of the measurements relative to the least squares fit
     * @param stateMeasurements       a vector of state measurement objects
     * @param regressionStandardError upon success, will contain the computed regression standard errors
     */
    EXPORT_STEM virtual bool
    computeRegressionStandardError(const std::vector<StateMap *> &stateMeasurements,
                                   StateMap &regressionStandardError) final;

    /**
     * create() function
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    static EXPORT_STEM LeastSquaresMeasurementStrategy *create(EstimationFilterUser *pEstimationFilterUser);

    /**
     * Function to estimate derivatives for the given collection of measurement samples
     * @param stateMeasurements          a vector of state measurement objects
     * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
     *                                   samples
     */
    EXPORT_STEM virtual bool estimateDerivatives(const std::vector<StateMap *> &stateMeasurements,
                                                 StateMap &stateMeasurementDerivative) override;

    /**
     * Function to estimate standard deviations for the given collection of measurement samples
     * @param stateMeasurements                  a vector of state measurement objects
     * @param stateMeasurementStandardDeviations upon success, will contain the estimated standard deviations
     *                                           of the measurement samples
     */
    EXPORT_STEM virtual
    bool estimateStandardDeviations(const std::vector<StateMap *> &stateMeasurements,
                                    StateMap &stateMeasurementStandardDeviations) override;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get this object's vector of intercepts
     */
    EXPORT_STEM virtual std::map<std::string, double> &getIntercepts(void) final;

    /**
     * Get the standard deviation of the measurements relative to the least squares fit
     */
    EXPORT_STEM virtual std::map<std::string, double> &getSigmas(void) final;

    /**
     * Get this object's vector of slopes
     */
    EXPORT_STEM virtual std::map<std::string, double> &getSlopes(void) final;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Swap function
     */
    EXPORT_STEM virtual void
    swap(LeastSquaresMeasurementStrategy &measurementAggregationStrategy) override final;

protected:

    /**
     * the calculated intercepts
     */
    std::map<std::string, double> m_interceptMap;

    /**
     * a map to maintain the measurement count for each measurement
     */
    std::map<std::string, double> m_measurementCountMap;

    /**
     * the standard deviation of the measurements relative to the least squares fit
     */
    std::map<std::string, double> m_sigmaMap;

    /**
     * the calculated slopes
     */
    std::map<std::string, double> m_slopeMap;

    /**
     * the calculated cross-term product sums
     */
    std::map<std::string, double> m_xySumMap;

    /**
     * measurement sums
     */
    std::map<std::string, double> m_ySumMap;
};

}

}

}

#endif
