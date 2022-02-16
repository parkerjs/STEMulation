#ifndef ADAPTIVE_RUNGE_KUTTA_H
#define ADAPTIVE_RUNGE_KUTTA_H

#include "rungeKutta.h"

namespace math
{

namespace integrators
{

/**
 * This class serves as an abstract base for adaptive Runge-Kutta numerical differential equation solvers
 */
class AdaptiveRungeKutta
: public RungeKutta,
  virtual private attributes::abstract::Reflective,
  public attributes::interfaces::Swappable<AdaptiveRungeKutta>
{
public:

    /**
     * Using declarations
     */
    using RungeKutta::swap;

    /**
     * Constructor
     * @param butcherTableau  a reference to this Runge-Kutta method's Butcher Tableau
     * @param tolerance       the desired error tolerance (default = 1e-4)
     * @param minimumStepSize the minimum step size (default = 1.0e-10)
     * @param maximumStepSize the maximum step size (default = 1e-2)
     */
    EXPORT_STEM AdaptiveRungeKutta(const ButcherTableau &butcherTableau,
                                   double tolerance = 1.0e-4,
                                   double minimumStepSize = 1.0e-10,
                                   double maximumStepSize = 1.0e-2);

    /**
     * Copy constructor
     */
    EXPORT_STEM AdaptiveRungeKutta(const AdaptiveRungeKutta &rungeKutta);

    /**
     * Move constructor
     */
    EXPORT_STEM AdaptiveRungeKutta(AdaptiveRungeKutta &&rungeKutta);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~AdaptiveRungeKutta(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM AdaptiveRungeKutta &operator = (const AdaptiveRungeKutta &rungeKutta);

    /**
     * Move assignment operator
     */
    EXPORT_STEM AdaptiveRungeKutta &operator = (AdaptiveRungeKutta &&rungeKutta);

protected:

    /**
     * Function to calculate the adaptive step size as a function of the current step size (h) and local
     * truncation error
     */
    EXPORT_STEM virtual double calcAdaptiveStepSize(double h,
                                                    double error) const = 0;

public:

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string getClassName(void) const override;

    /**
     * Get maximum step size
     */
    EXPORT_STEM virtual double getMaximumStepSize(void) const final;

    /**
     * Get minimum step size
     */
    EXPORT_STEM virtual double getMinimumStepSize(void) const final;

    /**
     * Get error tolerance
     */
    EXPORT_STEM virtual double getTolerance(void) const final;

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) override;
#ifdef RAPID_XML
    /**
     * Function to read data from XML
     */
    EXPORT_STEM virtual bool readFromXML(rapidxml::xml_node<char> *pNode) override;
#endif
    /**
     * Set maximum step size
     */
    EXPORT_STEM virtual void setMaximumStepSize(double maximumStepSize) final;

    /**
     * Set minimum step size
     */
    EXPORT_STEM virtual void setMinimumStepSize(double minimumStepSize) final;

    /**
     * Set error tolerance
     */
    EXPORT_STEM virtual void setTolerance(double tolerance) final;

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) override;

    /**
     * Function to numerically solve an ordinary differential equation
     * @param x        the state vector
     * @param dynamics a function object which formulates the dynamics as a series of first-order differential
     *                 equations; the function object's function call operator should accept three arguments,
     *                 the first two of which are the input time and state vector; the last argument contains
     *                 the computed state derivative on output.
     * @param t0       the start of the interval
     * @param t1       the end of the interval
     */
    EXPORT_STEM virtual bool solve(StateVector &x,
                                   tStateDynamicsFunction &dynamics,
                                   double t0,
                                   double t1) override;

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(AdaptiveRungeKutta &rungeKutta) override final;

protected:

    /**
     * maximum step size
     */
    double m_maximumStepSize;

    /**
     * minimum step size
     */
    double m_minimumStepSize;

    /**
     * a copy of the original state (internal use only)
     */
    StateVector *m_pState0;

    /**
     * error tolerance
     */
    double m_tolerance;
};

}

}

#endif
