#ifndef RUNGE_KUTTA_FEHLBERG_45_H
#define RUNGE_KUTTA_FEHLBERG_45_H

#include "adaptiveRungeKutta.h"

namespace math
{

namespace integrators
{

/**
 * This class implements the Runge-Kutta-Fehlberg numerical ordinary differential equation solver
 */
class RungeKuttaFehlberg45
: public AdaptiveRungeKutta,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     * @param tolerance       the desired error tolerance (default = 1e-4)
     * @param minimumStepSize the minimum step size (default = 1.0e-10)
     * @param maximumStepSize the maximum step size (default = 1e-2)
     */
    EXPORT_STEM RungeKuttaFehlberg45(double tolerance = 1.0e-4,
                                     double minimumStepSize = 1.0e-10,
                                     double maximumStepSize = 1.0e-2);

    /**
     * Copy constructor
     */
    EXPORT_STEM RungeKuttaFehlberg45(const RungeKuttaFehlberg45 &rungeKutta);

    /**
     * Move constructor
     */
    EXPORT_STEM RungeKuttaFehlberg45(RungeKuttaFehlberg45 &&rungeKutta);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~RungeKuttaFehlberg45(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RungeKuttaFehlberg45 &operator = (const RungeKuttaFehlberg45 &rungeKutta);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RungeKuttaFehlberg45 &operator = (RungeKuttaFehlberg45 &&rungeKutta);

protected:

    /**
     * Function to calculate the adaptive step size as a function of the current step size (h) and local
     * truncation error
     */
    EXPORT_STEM virtual double calcAdaptiveStepSize(double h,
                                                    double error) const override;

public:

    /**
     * clone() function
     */
    EXPORT_STEM virtual RungeKuttaFehlberg45 *clone(void) const override;

    /**
     * create() function
     * @param tolerance       the desired error tolerance (default = 1e-4)
     * @param minimumStepSize the minimum step size (default = 1.0e-10)
     * @param maximumStepSize the maximum step size (default = 1e-2)
     */
    static EXPORT_STEM RungeKuttaFehlberg45 *create(double tolerance = 1.0e-4,
                                                    double minimumStepSize = 1.0e-10,
                                                    double maximumStepSize = 1.0e-2);

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

#endif
