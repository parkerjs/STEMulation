#ifndef LINEAR_KALMAN_H
#define LINEAR_KALMAN_H

#include "extendedKalman.h"

namespace math
{

namespace statistical
{

namespace estimation
{

namespace kalman
{

/**
 * This class implements a linear Kalman estimation filter
 */
class LinearKalman
: public ExtendedKalman,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<LinearKalman>
{
public:

    /**
     * Using declarations
     */
    using ExtendedKalman::swap;

private:

    /**
     * Constructor
     */
    EXPORT_STEM LinearKalman(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM LinearKalman(const LinearKalman &kalman);

    /**
     * Move constructor
     */
    EXPORT_STEM LinearKalman(LinearKalman &&kalman);

public:

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~LinearKalman(void) override;

private:

    /**
     * Copy assignment operator
     */
    EXPORT_STEM LinearKalman &operator = (const LinearKalman &kalman);

    /**
     * Move assignment operator
     */
    EXPORT_STEM LinearKalman &operator = (LinearKalman &&kalman);

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual LinearKalman *clone(void) const override;

    /**
     * create() function
     */
    static EXPORT_STEM LinearKalman *create(void);

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get the factory name of this constructible
     */
    EXPORT_STEM virtual std::string getFactoryName(void) const override final;

    /**
     * Perform a measurement update
     * @param stateMeasurement a reference to a state vector object
     */
    EXPORT_STEM virtual bool measurementUpdate(StateVector &stateMeasurement) override;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(LinearKalman &kalman) override final;

private:

    /**
     * the measurement matrix
     */
    Matrix2d m_H;
};

}

}

}

}

#endif
