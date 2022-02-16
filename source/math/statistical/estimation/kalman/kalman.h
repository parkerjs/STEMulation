#ifndef KALMAN_H
#define KALMAN_H

#include "estimationFilter.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace kalman
{

/**
 * This class provides an abstract base for derived types to implement statistical estimation filters of the
 * Kalman variety
 */
class Kalman
: public EstimationFilter,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<Kalman>
{
public:

    /**
     * Using declarations
     */
    using EstimationFilter::swap;

protected:

    /**
     * Constructor
     */
    EXPORT_STEM Kalman(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM Kalman(const Kalman &kalman);

    /**
     * Move constructor
     */
    EXPORT_STEM Kalman(Kalman &&kalman);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~Kalman(void) override;

protected:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM Kalman &operator = (const Kalman &kalman);

    /**
     * Move assignment operator
     */
    EXPORT_STEM Kalman &operator = (Kalman &&kalman);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual Kalman *clone(void) const override = 0;

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(Kalman &kalman) override final;

protected:

    /**
     * the error covariance matrix
     */
    Matrix2d m_P;

    /**
     * the process covariance matrix
     */
    Matrix2d m_Q;

    /**
     * the measurement covariance matrix
     */
    Matrix2d m_R;
};

}

}

}

}

#endif
