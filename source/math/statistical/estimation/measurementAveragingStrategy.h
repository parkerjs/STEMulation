#ifndef MEASUREMENT_AVERAGING_STRATEGY_H
#define MEASUREMENT_AVERAGING_STRATEGY_H

#include "measurementAggregationStrategy.h"

namespace math
{

namespace statistical
{

namespace estimation
{

/**
 * This class implements a measurement aggregation strategy in which the measurements are averaged and then
 * used to initialize the state estimate
 */
class MeasurementAveragingStrategy
: public MeasurementAggregationStrategy,
  virtual private attributes::abstract::Reflective
{
private:

    /**
     * Constructor
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    EXPORT_STEM MeasurementAveragingStrategy(EstimationFilterUser *pEstimationFilterUser);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~MeasurementAveragingStrategy(void) override;

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
    EXPORT_STEM virtual MeasurementAveragingStrategy *clone(void) const override;

    /**
     * create() function
     * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
     */
    static EXPORT_STEM MeasurementAveragingStrategy *create(EstimationFilterUser *pEstimationFilterUser);

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
};

}

}

}

#endif
