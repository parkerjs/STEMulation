#ifndef EXTENDED_KALMAN_H
#define EXTENDED_KALMAN_H

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
 * This class implements an extended Kalman estimation filter
 */
class ExtendedKalman
: public Kalman,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<ExtendedKalman>
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
    EXPORT_STEM ExtendedKalman(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM ExtendedKalman(const ExtendedKalman &kalman);

    /**
     * Move constructor
     */
    EXPORT_STEM ExtendedKalman(ExtendedKalman &&kalman);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~ExtendedKalman(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM ExtendedKalman &operator = (const ExtendedKalman &kalman);

    /**
     * Move assignment operator
     */
    EXPORT_STEM ExtendedKalman &operator = (ExtendedKalman &&kalman);

    /**
     * Adaptively estimate the process covariance
     * @param x the state estimate prior to measurement update
     * @param P the error covariance prior to measurement update
     */
    EXPORT_STEM virtual void adaptProcessCovariance(const Matrix2d &x,
                                                    const Matrix2d &P);

    /**
     * Adjust the residual covariance
     * @param A      the dynamics Jacobian
     * @param H      the measurement Jacobian
     * @param lambda the norm residual ratio
     * @param S      the residual covariance
     */
    EXPORT_STEM virtual void adjustResidualCovariance(const Matrix2d &A,
                                                      const Matrix2d &H,
                                                      double lambda,
                                                      Matrix2d &S);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual ExtendedKalman *clone(void) const override;

protected:

    /**
     * Adaptive reverse state estimation
     * @param H the measurement Jacobian
     * @param S the residual covariance
     * @param v the measurement residual
     * @param z the state measurement vector
     */
    EXPORT_STEM virtual void computeReversePrediction(const Matrix2d &H,
                                                      Matrix2d &S,
                                                      const Matrix2d &v,
                                                      const Matrix2d &z);

public:

    /**
     * Configure function (using input file or input string)
     */
    EXPORT_STEM virtual bool configure(tTokenMap &tokenMap) override;

    /**
     * create() function
     */
    static EXPORT_STEM ExtendedKalman *create(void);

    /**
     * Get adaptive reverse prediction norm residual threshold
     */
    EXPORT_STEM virtual double getAdaptiveNormResidualThreshold(void) const final;

    /**
     * Get adaptive process window size
     */
    EXPORT_STEM virtual double getAdaptiveWindow(void) const final;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override;

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
     * Set adaptive reverse prediction norm residual threshold
     * @param lambda the norm residual ratio threshold (a value > 0 enables adaptive reverse prediction)
     */
    EXPORT_STEM virtual void setAdaptiveNormResidualThreshold(double lambda = 0.0) final;

    /**
     * Set adaptive window size
     */
    EXPORT_STEM virtual bool setAdaptiveWindow(double window) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(ExtendedKalman &kalman) override final;

    /**
     * Perform a time update
     * @param u the control input vector
     */
    EXPORT_STEM virtual void timeUpdate(const Matrix2d &u = Matrix2d()) override;

protected:

    /**
     * window size controls level of expected process noise update change
     */
    double m_gamma;

    /**
     * norm residual ratio threshold
     */
    double m_lambda;

    /**
     * temporary variables
     */
    Matrix2d m_temp_q;
    Matrix2d m_temp_vSIv;
    Matrix2d m_temp_wSIw;
    Matrix2d m_temp_x;
    Matrix2d m_tempP;
    Matrix2d m_tempQ;
    Matrix2d m_tempS;
};

}

}

}

}

#endif
