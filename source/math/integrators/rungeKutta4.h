#ifndef RUNGE_KUTTA_4_H
#define RUNGE_KUTTA_4_H

#include "rungeKutta.h"

namespace math
{

// forward declarations
namespace control_systems { class StateVector; }

namespace integrators
{

/**
 * This class implements the classic fourth-order Runge-Kutta method
 */
class RungeKutta4
: public RungeKutta,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     */
    EXPORT_STEM RungeKutta4(void);

    /**
     * Copy constructor
     */
    EXPORT_STEM RungeKutta4(const RungeKutta4 &rungeKutta);

    /**
     * Move constructor
     */
    EXPORT_STEM RungeKutta4(RungeKutta4 &&rungeKutta);

    /**
     * Destructor
     */
    EXPORT_STEM virtual ~RungeKutta4(void) override;

    /**
     * Copy assignment operator
     */
    EXPORT_STEM RungeKutta4 &operator = (const RungeKutta4 &rungeKutta);

    /**
     * Move assignment operator
     */
    EXPORT_STEM RungeKutta4 &operator = (RungeKutta4 &&rungeKutta);

    /**
     * clone() function
     */
    EXPORT_STEM virtual RungeKutta4 *clone(void) const override;

    /**
     * create() function
     */
    static EXPORT_STEM RungeKutta4 *create(void);

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
