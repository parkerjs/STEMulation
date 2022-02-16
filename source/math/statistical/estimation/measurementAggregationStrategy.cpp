#include "estimationFilterUser.h"
#include "measurementAggregationStrategy.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif

// using namespace declarations
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
 * Constructor
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
MeasurementAggregationStrategy::
MeasurementAggregationStrategy(EstimationFilterUser *pEstimationFilterUser)
: m_pEstimationFilterUser(nullptr)
{
    setEstimationFilterUser(pEstimationFilterUser);
}

/**
 * Copy constructor
 */
MeasurementAggregationStrategy::
MeasurementAggregationStrategy(const MeasurementAggregationStrategy &measurementAggregationStrategy)
: m_pEstimationFilterUser(nullptr)
{
    operator = (measurementAggregationStrategy);
}

/**
 * Move constructor
 */
MeasurementAggregationStrategy::
MeasurementAggregationStrategy(MeasurementAggregationStrategy &&measurementAggregationStrategy)
: m_pEstimationFilterUser(nullptr)
{
    operator = (std::move(measurementAggregationStrategy));
}

/**
 * Destructor
 */
MeasurementAggregationStrategy::~MeasurementAggregationStrategy(void)
{

}

/**
 * Copy assignment operator
 */
MeasurementAggregationStrategy &
MeasurementAggregationStrategy::operator = (const MeasurementAggregationStrategy &measurementAggregationStrategy)
{
    if (&measurementAggregationStrategy != this)
    {
        Loggable<std::string, std::ostream>::operator = (measurementAggregationStrategy);

        // NOTE: the caller will need to set the associated estimation filter user separately
        m_pEstimationFilterUser = nullptr;
    }

    return *this;
}

/**
 * Move assignment operator
 */
MeasurementAggregationStrategy &
MeasurementAggregationStrategy::operator = (MeasurementAggregationStrategy &&measurementAggregationStrategy)
{
    if (&measurementAggregationStrategy != this)
    {
        measurementAggregationStrategy.swap(*this);
    }

    return *this;
}
#ifdef RAPID_XML
/**
 * Function to construct an instance from an XML node
 * @param pNode                 a pointer to an XML node
 * @param pEstimationFilterUser a pointer to an estimation filter user associated with this object
 */
MeasurementAggregationStrategy *
MeasurementAggregationStrategy::createFromXML(xml_node<char> *pNode,
                                              EstimationFilterUser *pEstimationFilterUser)
{
    MeasurementAggregationStrategy *pMeasurementAggregationStrategy = nullptr;
    if (pNode != nullptr && std::strcmp(pNode->name(), "measurementAggregationStrategy") == 0)
    {
        auto *pTypeAttribute = pNode->first_attribute("type");
        if (pTypeAttribute != nullptr)
        {
            std::string name("measurement_aggregation_strategy");
            auto &&type = pTypeAttribute->value();
            pMeasurementAggregationStrategy = MeasurementAggregationStrategy::
                                              create(type,
                                                     pEstimationFilterUser);
            if (pMeasurementAggregationStrategy != nullptr)
            {
                pMeasurementAggregationStrategy->readFromXML(pNode);
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create object of type \"" + std::string(type) + "\".\n",
                       getQualifiedStaticMethodName(__func__, MeasurementAggregationStrategy));
            }
        }
    }

    return pMeasurementAggregationStrategy;
}
#endif
/**
 * Function to aggregate a collection of measurement samples
 * @param stateMeasurements          a vector of state measurement objects
 * @param aggregateStateMeasurement  upon success, will contain the result of the aggregation
 * @param stateMeasurementDerivative upon success, will contain the estimated derivative of the measurement
 *                                   samples
 */
bool MeasurementAggregationStrategy::aggregate(const std::vector<StateMap *> &stateMeasurements,
                                               StateMap &aggregateStateMeasurement,
                                               StateMap &stateMeasurementDerivative)
{
    bool bSuccess = aggregate(stateMeasurements,
                              aggregateStateMeasurement);
    if (bSuccess)
        bSuccess = estimateDerivatives(stateMeasurements,
                                       stateMeasurementDerivative);

    return bSuccess;
}

/**
 * Get the name of this class
 */
std::string MeasurementAggregationStrategy::getClassName(void) const
{
    return "MeasurementAggregationStrategy";
}

/**
 * Get a pointer to the estimation filter user associated with this object
 */
EstimationFilterUser *MeasurementAggregationStrategy::getEstimationFilterUser(void)
{
    return m_pEstimationFilterUser;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool MeasurementAggregationStrategy::readFromXML(xml_node<char> *pNode)
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
 * Set a pointer to the estimation filter user associated with this object
 */
void MeasurementAggregationStrategy::
setEstimationFilterUser(EstimationFilterUser *pEstimationFilterUser)
{
    if (m_pEstimationFilterUser != pEstimationFilterUser)
    {
        m_pEstimationFilterUser = pEstimationFilterUser;

        pEstimationFilterUser->setMeasurementAggregationStrategy(this);
    }
}

/**
 * Swap function
 */
void MeasurementAggregationStrategy::swap(MeasurementAggregationStrategy &measurementAggregationStrategy)
{
    Loggable<std::string, std::ostream>::swap(measurementAggregationStrategy);

    // swap the estimation filter users
    auto *pEstimationFilterUser = measurementAggregationStrategy.getEstimationFilterUser();
    measurementAggregationStrategy.setEstimationFilterUser(m_pEstimationFilterUser);
    setEstimationFilterUser(pEstimationFilterUser);
}

}

}

}
