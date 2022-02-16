#include "appliedEstimationFilter.h"
#include "dictionary.h"
#include "estimationFilter.h"
#include "file_system.h"
#include "matrix2d.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateMap.h"
#include "stateVector.h"

// using namespace declarations
using namespace math::control_systems;
using namespace math::linear_algebra::matrix;
#ifdef RAPID_XML
using namespace rapidxml;
#endif
using namespace utilities;
using namespace utilities::file_system;

namespace math
{

namespace statistical
{

namespace estimation
{

/**
 * Constructor
 */
AppliedEstimationFilter::AppliedEstimationFilter(void)
: m_pEstimationFilter(nullptr),
  m_time(0.0)
{

}

/**
 * Constructor
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
AppliedEstimationFilter::AppliedEstimationFilter(EstimationFilter *pEstimationFilter)
: m_pEstimationFilter(nullptr),
  m_processingDelay(0.0)
{
    setEstimationFilter(pEstimationFilter);
}

/**
 * Copy constructor
 */
AppliedEstimationFilter::AppliedEstimationFilter(AppliedEstimationFilter &appliedEstimationFilter)
: m_pEstimationFilter(nullptr)
{
    operator = (appliedEstimationFilter);
}

/**
 * Move constructor
 */
AppliedEstimationFilter::AppliedEstimationFilter(AppliedEstimationFilter &&appliedEstimationFilter)
: m_pEstimationFilter(nullptr)
{
    operator = (std::move(appliedEstimationFilter));
}

/**
 * Destructor
 */
AppliedEstimationFilter::~AppliedEstimationFilter(void)
{
    if (m_pEstimationFilter != nullptr)
        m_pEstimationFilter->setAppliedEstimationFilter(nullptr);
}

/**
 * Copy assignment operator
 */
AppliedEstimationFilter &
AppliedEstimationFilter::operator = (AppliedEstimationFilter &appliedEstimationFilter)
{
    if (&appliedEstimationFilter != this)
    {
        Loggable<std::string, std::ostream>::operator = (appliedEstimationFilter);

        // NOTE: the caller will need to set the associated estimation filter separately
        m_pEstimationFilter = nullptr;

        m_processingDelay = appliedEstimationFilter.m_processingDelay;
        m_time = appliedEstimationFilter.m_time;
    }

    return *this;
}

/**
 * Move assignment operator
 */
AppliedEstimationFilter &
AppliedEstimationFilter::operator = (AppliedEstimationFilter &&appliedEstimationFilter)
{
    if (&appliedEstimationFilter != this)
    {
        appliedEstimationFilter.swap(*this);
    }

    return *this;
}

/**
 * Process the measurement residual
 * @param yh the state estimate prior to measurement update
 * @param z  the state measurement vector
 */
Matrix2d AppliedEstimationFilter::computeMeasurementResidual(const Matrix2d &yh,
                                                             const Matrix2d &z)
{
    return z - yh;
}

/**
 * Configure function (using a variable token map)
 */
bool AppliedEstimationFilter::configure(const std::string &filename)
{
    std::ifstream stream;
    bool bSuccess = FileSystem::openFileStream(filename, stream);
    if (bSuccess)
    {
        // process this object's variable registry
        Dictionary dictionary(&m_registry);

        auto &&tokenMap = dictionary.createTokenPairs<std::map<std::string, std::string>>(stream);
        stream.close();

        bSuccess = configure(tokenMap);
        if (!bSuccess)
        {
            logMsg(std::cout, LoggingLevel::Enum::Error,
                   "Failed to perform token map configuration!\n",
                   getQualifiedMethodName(__func__));
        }
    }
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to open or parse \"" + filename + "\".\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Configure function (using input file or input string)
 */
bool AppliedEstimationFilter::configure(tTokenMap &tokenMap)
{
    // process this object's variable registry
    Dictionary dictionary(&m_registry);

    bool bSuccess = dictionary.populate(tokenMap);
    if (!bSuccess)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "Failed to perform token map configuration!\n",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Function to convert a map of state measurements to a vector of state measurements
 */
StateVector AppliedEstimationFilter::convertStateMeasurementToStateVector(const StateMap &stateMeasurement)
{
    StateVector measurement;

    convertStateMeasurementToStateVector(stateMeasurement, measurement);

    return measurement;
}
#ifdef RAPID_XML
/**
 * Function to construct an instance from an XML node
 * @param pNode             a pointer to an XML node
 * @param pEstimationFilter a pointer to the estimation filter associated with this object
 */
AppliedEstimationFilter *
AppliedEstimationFilter::createFromXML(xml_node<> *pNode,
                                       EstimationFilter *pEstimationFilter)
{
    AppliedEstimationFilter *pAppliedEstimationFilter = nullptr;
    if (pNode != nullptr && std::strcmp(pNode->name(), "appliedEstimationFilter") == 0)
    {
        auto *pTypeAttribute = pNode->first_attribute("type");
        if (pTypeAttribute != nullptr)
        {
            std::string name("applied_estimation_filter");
            auto &&type = pTypeAttribute->value();
            pAppliedEstimationFilter = AppliedEstimationFilter::create(type,
                                                                       pEstimationFilter);
            if (pAppliedEstimationFilter != nullptr)
            {
                pAppliedEstimationFilter->readFromXML(pNode);
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create object of type \"" + std::string(type) + "\".\n",
                       getQualifiedStaticMethodName(__func__, AppliedEstimationFilter));
            }
        }
    }

    return pAppliedEstimationFilter;
}
#endif
/**
 * Evaluate the dynamics model
 * @param dt the sampling interval (s)
 * @param x  the state vector
 */
Matrix2d AppliedEstimationFilter::dynamicsModel(double dt,
                                                const Matrix2d &x)
{
    Matrix2d u; // construct an empty input vector

    return dynamicsModel(dt, x, u);
}

/**
 * Get the name of this class
 */
std::string AppliedEstimationFilter::getClassName(void) const
{
    return "AppliedEstimationFilter";
}

/**
 * Get a pointer to the estimation filter associated with this object
 */
EstimationFilter *AppliedEstimationFilter::getEstimationFilter(void)
{
    return m_pEstimationFilter;
}

/**
 * Get a pointer to the estimation filter user associated with this object; returns non-null upon success
 */
EstimationFilterUser *AppliedEstimationFilter::getEstimationFilterUser(void)
{
    EstimationFilterUser *pEstimationFilterUser = nullptr;
    if (m_pEstimationFilter != nullptr)
    {
        pEstimationFilterUser = m_pEstimationFilter->getEstimationFilterUser();
    }

    return pEstimationFilterUser;
}

/**
 * Get the filter processing delay (s)
 */
double AppliedEstimationFilter::getProcessingDelay(void) const
{
    return m_processingDelay;
}

/**
 * Get the time maintained by this object
 */
double AppliedEstimationFilter::getTime(void) const
{
    return m_time;
}

/**
 * Evaluate the measurement model
 * @param x the state vector
 */
Matrix2d AppliedEstimationFilter::measurementModel(const Matrix2d &x)
{
    Matrix2d u; // construct an empty input vector

    return measurementModel(x, u);
}

/**
 * Perform post-processing on filtered data
 * @param xh the state estimate vector
 * @param dt the sampling interval (s)
 */
void AppliedEstimationFilter::postProcessFilteredData(Matrix2d &xh,
                                                      double dt)
{
    // do nothing
}

/**
 * Perform pre-processing on measured data prior to filtering
 * @param stateMeasurement a reference to a state vector object
 * @param dt               the sampling interval (s)
 */
void AppliedEstimationFilter::preProcessMeasurementData(StateVector &stateMeasurement,
                                                        double dt)
{
    // do nothing
}

#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool AppliedEstimationFilter::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr);
    if (bSuccess)
    {
        bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "appliedEstimationFilter") == 0);
        if (bSuccess)
        {
            // read the filter processing delay
            auto *pNameNode = pNode->first_node("processingDelay");
            if (pNameNode != nullptr)
                setProcessingDelay(std::stod(pNameNode->value()));
        }
    }

    return bSuccess;
}
#endif
/**
 * Set a pointer to the estimation filter associated with this object
 */
void AppliedEstimationFilter::setEstimationFilter(EstimationFilter *pEstimationFilter)
{
    if (m_pEstimationFilter != pEstimationFilter)
    {
        m_pEstimationFilter = pEstimationFilter;

        pEstimationFilter->setAppliedEstimationFilter(this);
    }
}

/**
 * Set the filter processing delay (s)
 */
void AppliedEstimationFilter::setProcessingDelay(double delay)
{
    m_processingDelay = delay;
}

/**
 * Set the time maintained by this object
 */
void AppliedEstimationFilter::setTime(double time)
{
    m_time = time;
}

/**
 * Setup function
 */
bool AppliedEstimationFilter::setup(void)
{
    bool bSuccess = true;

    m_registry["filterProcessingDelay"] = m_processingDelay;

    return bSuccess;
}

/**
 * Swap function
 */
void AppliedEstimationFilter::swap(AppliedEstimationFilter &appliedEstimationFilter)
{
    Loggable<std::string, std::ostream>::swap(appliedEstimationFilter);

    // swap the estimation filters
    auto *pEstimationFilter = appliedEstimationFilter.getEstimationFilter();
    appliedEstimationFilter.setEstimationFilter(m_pEstimationFilter);
    setEstimationFilter(pEstimationFilter);

    std::swap(m_processingDelay, appliedEstimationFilter.m_processingDelay);
    std::swap(m_time, appliedEstimationFilter.m_time);
}

/**
 * Update the time maintained by this object
 */
void AppliedEstimationFilter::updateTime(double dt)
{
    m_time += dt;
}

}

}

}
