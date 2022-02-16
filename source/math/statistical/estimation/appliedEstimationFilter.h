#ifndef APPLIED_ESTIMATION_FILTER_H
#define APPLIED_ESTIMATION_FILTER_H

#include "cloneable.h"
#include "export_library.h"
#include "factory_constructible.h"
#include "file_configurable.h"
#include "initializable.h"
#include "loggable.h"
#include "reflective.h"
#include "setupable.h"
#include "swappable.h"
#include "token_map_configurable.h"
#include "variable_map_registrable.h"
#ifdef RAPID_XML
#include "xml_constructible.h"
#include "xml_readable.h"
#endif

// forward declarations
#ifdef RAPID_XML
namespace rapidxml { template<class> class xml_node; }
#endif

namespace math
{

// forward declarations
namespace linear_algebra { namespace matrix { class Matrix2d; } }
namespace control_systems { class StateMap;
                            class StateVector; }

namespace statistical
{

namespace estimation
{

// forward declarations
class EstimationFilter;
class EstimationFilterUser;

/**
 * This class provides an abstract base for derived types to implement applied statistical estimation filters
 */
class AppliedEstimationFilter
: public attributes::interfaces::Cloneable<AppliedEstimationFilter &>,
  public attributes::abstract::FactoryConstructible<AppliedEstimationFilter>,
  public attributes::interfaces::FileConfigurable,
  public attributes::interfaces::Initializable,
  virtual public attributes::concrete::Loggable<std::string, std::ostream>,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Setupable,
  public attributes::interfaces::Swappable<AppliedEstimationFilter>,
  public attributes::interfaces::TokenMapConfigurable,
#ifdef RAPID_XML
  virtual public attributes::concrete::VariableMapRegistrable,
  public attributes::concrete::XML_Constructible<AppliedEstimationFilter, rapidxml::xml_node<char>>,
  public attributes::interfaces::XML_Readable<rapidxml::xml_node<char>>
#else
  public attributes::concrete::VariableMapRegistrable
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
    using Matrix2d = linear_algebra::matrix::Matrix2d;
    using StateMap = control_systems::StateMap;
    using StateVector = control_systems::StateVector;
    using VariableRegistry = utilities::VariableRegistry;

    /**
     * Using declarations
     */
    using Loggable<std::string, std::ostream>::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM AppliedEstimationFilter(void);

    /**
     * Constructor
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM AppliedEstimationFilter(EstimationFilter *pEstimationFilter);

    /**
     * Copy constructor
     */
    EXPORT_STEM AppliedEstimationFilter(AppliedEstimationFilter &appliedEstimationFilter);

    /**
     * Move constructor
     */
    EXPORT_STEM AppliedEstimationFilter(AppliedEstimationFilter &&appliedEstimationFilter);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~AppliedEstimationFilter(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM AppliedEstimationFilter &operator = (AppliedEstimationFilter &appliedEstimationFilter);

    /**
     * Move assignment operator
     */
    EXPORT_STEM AppliedEstimationFilter &operator = (AppliedEstimationFilter &&appliedEstimationFilter);

    /**
     * clone() function
     */
    EXPORT_STEM virtual AppliedEstimationFilter *clone(void) override = 0;

public:

    /**
     * Compute error covariance
     * @param xh                             the state estimate vector
     * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
     * @param dt                             the sampling interval (s)
     */
    EXPORT_STEM virtual Matrix2d computeErrorCovariance(const Matrix2d &xh,
                                                        const StateMap *pMeasurementStandardDeviations,
                                                        double dt) = 0;

    /**
     * Compute measurement covariance
     * @param pMeasurementStandardDeviations a pointer to a map of measurement standard deviations
     * @param dt                             the sampling interval (s)
     */
    EXPORT_STEM virtual Matrix2d computeMeasurementCovariance(const StateMap *pMeasurementStandardDeviations,
                                                              double dt) = 0;

    /**
     * Process the measurement residual
     * @param yh the state estimate prior to measurement update
     * @param z  the state measurement vector
     */
    EXPORT_STEM virtual Matrix2d computeMeasurementResidual(const Matrix2d &yh,
                                                            const Matrix2d &z);

    /**
     * Update process covariance
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual Matrix2d computeProcessCovariance(double dt) = 0;

    /**
     * Configure function (using a variable token map)
     */
    EXPORT_STEM virtual bool configure(const std::string &filename) override;

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * Function to convert a map of state measurements to a vector of state measurements
     */
    EXPORT_STEM virtual StateVector convertStateMeasurementToStateVector(const StateMap &stateMeasurement);

    /**
     * Function to convert a map of state measurements to a vector of state measurements
     * @param stateMeasurement the input map of state measurements
     * @param stateVector      upon success, contains the information converted from the input map of state
     *                         measurements
     */
    EXPORT_STEM virtual void convertStateMeasurementToStateVector(const StateMap &stateMeasurement,
                                                                  StateVector &measurementVector) = 0;
#ifdef RAPID_XML
    /**
     * Function to construct an instance from an XML node
     * @param pNode             a pointer to an XML node
     * @param pEstimationFilter a pointer to the estimation filter associated with this object
     */
    static EXPORT_STEM AppliedEstimationFilter *
    createFromXML(rapidxml::xml_node<char> *pNode,
                  EstimationFilter *pEstimationFilter);
#endif
    /**
     * Evaluate the dynamics Jacobian
     * @param dt the sampling interval (s)
     * @param x  the state vector
     */
    EXPORT_STEM virtual Matrix2d dynamicsJacobian(double dt,
                                                  const Matrix2d &x) const = 0;

    /**
     * Evaluate the dynamics model
     * @param dt the sampling interval (s)
     * @param x  the state vector
     */
    EXPORT_STEM virtual Matrix2d dynamicsModel(double dt,
                                               const Matrix2d &x) final;

    /**
     * Evaluate the dynamics model
     * @param dt the sampling interval (s)
     * @param x  the state vector
     * @param u  an input vector
     */
    EXPORT_STEM virtual Matrix2d dynamicsModel(double dt,
                                               const Matrix2d &x,
                                               const Matrix2d &u) = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM virtual EstimationFilter *getEstimationFilter(void) final;

    /**
     * Get a pointer to the estimation filter user associated with this object; returns non-null upon success
     */
    EXPORT_STEM virtual EstimationFilterUser *getEstimationFilterUser(void) final;

    /**
     * Get the filter processing delay (s)
     */
    EXPORT_STEM double getProcessingDelay(void) const;

    /**
     * Get the time maintained by this object
     */
    EXPORT_STEM virtual double getTime(void) const;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override = 0;

    /**
     * Evaluate the measurement Jacobian
     * @param x the state vector
     */
    EXPORT_STEM virtual Matrix2d measurementJacobian(const Matrix2d &x) const = 0;

    /**
     * Evaluate the measurement model
     * @param x the state vector
     */
    EXPORT_STEM virtual Matrix2d measurementModel(const Matrix2d &x) final;

    /**
     * Evaluate the measurement model
     * @param x the state vector
     * @param u an input vector
     */
    EXPORT_STEM virtual Matrix2d measurementModel(const Matrix2d &x,
                                                  const Matrix2d &u) = 0;

    /**
     * Perform post-processing on filtered data
     * @param xh the state estimate vector
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual void postProcessFilteredData(Matrix2d &xh,
                                                     double dt = 0.0);

    /**
     * Perform pre-processing on measured data prior to filtering
     * @param stateMeasurement a reference to a state vector object
     * @param dt               the sampling interval (s)
     */
    EXPORT_STEM virtual void preProcessMeasurementData(StateVector &stateMeasurement,
                                                       double dt = 0.0);
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set a pointer to the estimation filter associated with this object
     */
    EXPORT_STEM void setEstimationFilter(EstimationFilter *pEstimationFilter);

    /**
     * Set the filter processing delay (s)
     */
    EXPORT_STEM void setProcessingDelay(double delay);

    /**
     * Set the time maintained by this object
     */
    EXPORT_STEM virtual void setTime(double time);

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(AppliedEstimationFilter &appliedEstimationFilter) override final;

    /**
     * Update the time maintained by this object
     */
    EXPORT_STEM virtual void updateTime(double dt);

protected:

    /**
     * a pointer to the estimation filter associated with this object
     */
    EstimationFilter *m_pEstimationFilter;

    /**
     * filter data processing delay
     */
    double m_processingDelay;

    /**
     * the time maintained by this object
     */
    double m_time;
};

}

}

}

#endif
