#include "appliedEstimationFilter.h"
#include "dictionary.h"
#include "estimationFilter.h"
#include "estimationFilterUser.h"
#include "file_system.h"
#ifdef RAPID_XML
#include "rapidxml.hpp"
#endif
#include "stateVector.h"
#include <iostream>
#include <string>

// using namespace declarations
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
EstimationFilter::EstimationFilter(void)
: m_bInitialized(false),
  m_dt(0.0),
  m_pAppliedEstimationFilter(nullptr),
  m_pEstimationFilterUser(nullptr)
{

}

/**
 * Copy constructor
 */
EstimationFilter::EstimationFilter(const EstimationFilter &estimationFilter)
: m_pAppliedEstimationFilter(nullptr),
  m_pEstimationFilterUser(nullptr)
{
    operator = (estimationFilter);
}

/**
 * Move constructor
 */
EstimationFilter::EstimationFilter(EstimationFilter &&estimationFilter)
: m_pAppliedEstimationFilter(nullptr),
  m_pEstimationFilterUser(nullptr)
{
    operator = (std::move(estimationFilter));
}

/**
 * Destructor
 */
EstimationFilter::~EstimationFilter(void)
{
    if (m_pAppliedEstimationFilter != nullptr)
    {
        AppliedEstimationFilter *pAppliedEstimationFilter = nullptr;
        std::swap(m_pAppliedEstimationFilter, pAppliedEstimationFilter);
        delete pAppliedEstimationFilter;
    }
}

/**
 * Copy assignment operator
 */
EstimationFilter &EstimationFilter::operator = (const EstimationFilter &estimationFilter)
{
    if (&estimationFilter != this)
    {
        Loggable<std::string, std::ostream>::operator = (estimationFilter);

        m_bInitialized = estimationFilter.m_bInitialized;
        m_dt = estimationFilter.m_dt;

        if (m_pAppliedEstimationFilter != nullptr)
        {
            delete m_pAppliedEstimationFilter;
            m_pAppliedEstimationFilter = nullptr;
        }

        if (estimationFilter.m_pAppliedEstimationFilter != nullptr)
        {
            m_pAppliedEstimationFilter = estimationFilter.m_pAppliedEstimationFilter->clone();
            m_pAppliedEstimationFilter->setEstimationFilter(this);
        }

        // NOTE: the caller will need to set the associated estimation filter user separately
        m_pEstimationFilterUser = nullptr;

        m_xh = estimationFilter.m_xh;
    }

    return *this;
}

/**
 * Move assignment operator
 */
EstimationFilter &EstimationFilter::operator = (EstimationFilter &&estimationFilter)
{
    if (&estimationFilter != this)
    {
        estimationFilter.swap(*this);
    }

    return *this;
}

/**
 * Configure function (using a variable token map)
 */
bool EstimationFilter::configure(const std::string &filename)
{
    std::ifstream stream;
    bool bSuccess = FileSystem::openFileStream(filename, stream);
    if (bSuccess)
    {
        // process this object's variable registry
        Dictionary dictionary(&m_registry);

        auto &&tokenMap = dictionary.createTokenPairs<tTokenMap>(stream);
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
bool EstimationFilter::configure(tTokenMap &tokenMap)
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
#ifdef RAPID_XML
/**
 * Function to construct an instance from an XML node
 * @param pNode a pointer to an XML node
 */
EstimationFilter *EstimationFilter::createFromXML(xml_node<> *pNode)
{
    EstimationFilter *pEstimationFilter = nullptr;
    if (pNode != nullptr && std::strcmp(pNode->name(), "estimationFilter") == 0)
    {
        auto *pTypeAttribute = pNode->first_attribute("type");
        if (pTypeAttribute != nullptr)
        {
            std::string name("estimation_filter");
            auto &&type = pTypeAttribute->value();
            pEstimationFilter = EstimationFilter::create(type);
            if (pEstimationFilter != nullptr)
            {
                pEstimationFilter->readFromXML(pNode);
            }
            else
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create object of type \"" + std::string(type) + "\".\n",
                       getQualifiedStaticMethodName(__func__, EstimationFilter));
            }
        }
    }

    return pEstimationFilter;
}
#endif
/**
 * Get the applied estimation filter associated with this object
 */
AppliedEstimationFilter *EstimationFilter::getAppliedEstimationFilter(void)
{
    return m_pAppliedEstimationFilter;
}

/**
 * Get the name of this class
 */
std::string EstimationFilter::getClassName(void) const
{
    return "EstimationFilter";
}

/**
 * Get a pointer to the estimation filter user associated with this object
 */
EstimationFilterUser *EstimationFilter::getEstimationFilterUser(void)
{
    return m_pEstimationFilterUser;
}

/**
 * Retrieve a matrix from this object's registry with the specified name
 */
bool EstimationFilter::getMatrix(const std::string &name,
                                 Matrix2d &matrix)
{
    bool bSuccess = m_registry.retrieveValue(name, matrix);
    if (!bSuccess)
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "No matrix in the registry is associated with key \"" + name + "\".",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Get the state estimate
 */
void EstimationFilter::getStateEstimate(StateVector &stateEstimate)
{
    stateEstimate = m_xh;
}

/**
 * Get the update interval
 */
double EstimationFilter::getUpdateRate(void) const
{
    return m_dt;
}

/**
 * Check for estimation filter initialization
 */
bool EstimationFilter::initialized(void) const
{
    return m_bInitialized;
}
#ifdef RAPID_XML
/**
 * Function to read data from XML
 */
bool EstimationFilter::readFromXML(xml_node<> *pNode)
{
    bool bSuccess = (pNode != nullptr && std::strcmp(pNode->name(), "estimationFilter") == 0);
    if (bSuccess)
    {
        // read this object's applied estimation filter
        auto *pAppliedEstimationFilterNode = pNode->first_node("appliedEstimationFilter");
        if (bSuccess && pAppliedEstimationFilterNode != nullptr)
        {
            // attempt to create an applied estimation filter
            auto *pAppliedEstimationFilter = AppliedEstimationFilter::
                                             createFromXML(pAppliedEstimationFilterNode,
                                                           this);
            bSuccess = (pAppliedEstimationFilter != nullptr);
            if (!bSuccess)
            {
                logMsg(std::cout, LoggingLevel::Enum::Error,
                       "Failed to create applied estimation filter!\n",
                       getQualifiedMethodName(__func__));
            }
        }

        // read the update rate
        auto *pNameNode = pNode->first_node("updateRate");
        if (pNameNode != nullptr)
            setUpdateRate(std::stod(pNameNode->value()));
    }

    return bSuccess;
}
#endif
/**
 * Set the applied estimation filter associated with this object; the current object takes ownership of the
 * input argument and destroys the currently selected applied estimation filter upon success
 */
void EstimationFilter::setAppliedEstimationFilter(AppliedEstimationFilter *pAppliedEstimationFilter)
{
    if (m_pAppliedEstimationFilter != pAppliedEstimationFilter)
    {
        if (m_pAppliedEstimationFilter != nullptr)
        {
            delete m_pAppliedEstimationFilter;
            m_pAppliedEstimationFilter = nullptr;
        }

        m_pAppliedEstimationFilter = pAppliedEstimationFilter;
        if (m_pAppliedEstimationFilter != nullptr)
            m_pAppliedEstimationFilter->setEstimationFilter(this);
    }
}

/**
 * Set a pointer to the estimation filter user associated with this object
 */
void EstimationFilter::setEstimationFilterUser(EstimationFilterUser *pEstimationFilterUser)
{
    if (m_pEstimationFilterUser != pEstimationFilterUser)
    {
        m_pEstimationFilterUser = pEstimationFilterUser;
        if (m_pEstimationFilterUser != nullptr)
            m_pEstimationFilterUser->setEstimationFilter(this);
    }
}

/**
 * Set a matrix within this object's registry and associate it with the specified name
 */
bool EstimationFilter::setMatrix(const std::string &name,
                                 const Matrix2d &matrix)
{
    auto *pMatrix = m_registry.retrieveVariable<Matrix2d>(name);
    bool bSuccess = (pMatrix != nullptr);
    if (bSuccess)
        *pMatrix = matrix;
    else
    {
        logMsg(std::cout, LoggingLevel::Enum::Error,
               "No matrix in the registry is associated with key \"" + name + "\".",
               getQualifiedMethodName(__func__));
    }

    return bSuccess;
}

/**
 * Setup function
 */
bool EstimationFilter::setup(void)
{
    bool bSuccess = true;

    m_registry["stateEstimate"] = m_xh;
    m_registry["updateRate"] = m_dt;

    return bSuccess;
}

/**
 * Set the update rate
 * @param dt the sampling interval (s)
 */
void EstimationFilter::setUpdateRate(double dt)
{
    m_dt = dt;
}

/**
 * Swap function
 */
void EstimationFilter::swap(EstimationFilter &estimationFilter)
{
    Loggable<std::string, std::ostream>::swap(estimationFilter);

    std::swap(m_bInitialized, estimationFilter.m_bInitialized);
    std::swap(m_dt, estimationFilter.m_dt);

    // swap the applied estimation filters
    auto *pAppliedEstimationFilter = estimationFilter.getAppliedEstimationFilter();
    estimationFilter.setAppliedEstimationFilter(m_pAppliedEstimationFilter);
    setAppliedEstimationFilter(pAppliedEstimationFilter);

    // swap estimation filter users
    auto *pEstimationFilterUser = estimationFilter.getEstimationFilterUser();
    estimationFilter.setEstimationFilterUser(m_pEstimationFilterUser);
    setEstimationFilterUser(pEstimationFilterUser);

    m_xh.swap(estimationFilter.m_xh);
}

}

}

}
