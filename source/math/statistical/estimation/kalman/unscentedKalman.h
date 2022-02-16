#ifndef KALMAN_UNSCENTED_H
#define KALMAN_UNSCENTED_H

#include "kalman.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace kalman
{

/**
 * This class implements an unscented Kalman estimation filter
 */
class UnscentedKalman
: public Kalman,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<UnscentedKalman>
{
public:

    /**
     * Using declarations
     */
    using Kalman::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM UnscentedKalman(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM UnscentedKalman(const UnscentedKalman &kalman);

    /**
     * Copy constructor
     */
    EXPORT_STEM UnscentedKalman(UnscentedKalman &&kalman);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~UnscentedKalman(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM UnscentedKalman &operator = (const UnscentedKalman &kalman);

    /**
     * Move assignment operator
     */
    EXPORT_STEM UnscentedKalman &operator = (UnscentedKalman &&kalman);

    /**
     * Compute the sigma points
     */
    EXPORT_STEM virtual void calcSigmaPoints(void) final;

    /**
     * Compute the sigma points
     */
    EXPORT_STEM virtual void calcWeights(void) final;

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual UnscentedKalman *clone(void) const override;

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     */
    static EXPORT_STEM UnscentedKalman *create(void);

    /**
     * Get alpha parameter
     */
    EXPORT_STEM virtual double getAlpha(void) const final;

    /**
     * Get beta parameter
     */
    EXPORT_STEM virtual double getBeta(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Get kappa parameter
     */
    EXPORT_STEM virtual double getKappa(void) const final;

    /**
     * Initialization function
     * @param dt the sampling interval (s)
     */
    EXPORT_STEM virtual bool initialize(double dt) override;

    /**
     * Perform a measurement update
     * @param stateMeasurement a reference to a state vector object
     */
    EXPORT_STEM virtual bool measurementUpdate(StateVector &stateMeasurement) override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set alpha parameter
     */
    EXPORT_STEM virtual void setAlpha(double alpha) final;

    /**
     * Set beta parameter
     */
    EXPORT_STEM virtual void setBeta(double beta) final;

    /**
     * Set kappa parameter
     */
    EXPORT_STEM virtual void setKappa(double kappa) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(UnscentedKalman &kalman) override final;

    /**
     * Perform a time update
     * @param u the control input vector
     */
    EXPORT_STEM virtual void timeUpdate(const Matrix2d &u = Matrix2d()) override;

protected:

    /**
     * spread of sigma points around state estimate
     */
    double m_alpha;

    /**
     * pdf distribution parameter (beta = 2 for Gaussian distributions)
     */
    double m_beta;

    /**
     * column matrix of sigma point vectors
     */
    Matrix2d m_chiX;

    /**
     * sigma vectors propagated through measurement model
     */
    Matrix2d m_chiY;

    /**
     * secondary scaling parameter
     */
    double m_kappa;

    /**
     * Kalman gain matrix
     */
    Matrix2d m_K;

    /**
     * sigma vector error cross-covariance
     */
    Matrix2d m_Pxy;

    /**
     * sigma vector error auto-covariance
     */
    Matrix2d m_Pyy;

    /**
     * square-root of state covariance matrix
     */
    Matrix2d m_S;

    /**
     * covariance weights
     */
    Matrix2d m_Wc;

    /**
     * measurement weights
     */
    Matrix2d m_Wm;

    /**
     * observed estimate
     */
    Matrix2d m_yh;
};

}

}

}

}

#endif
