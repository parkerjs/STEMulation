#include "butcherTableau.h"
#include "rungeKutta4.h"

// file-scoped variables
static constexpr char factoryName[] = "RungeKutta4";
static constexpr double butcherTableauCoefficients[5][5] =
{ { 0.,  0.,   0.,   0.,   0.   },
  { 0.5, 0.5,  0.,   0.,   0.   },
  { 0.5, 0.,   0.5,  0.,   0.   },
  { 1.,  0.,   0.,   1.,   0.   },
  { 0.0, 1./6, 1./3, 1./3, 1./6 } };

// using namespace declarations
using namespace attributes::abstract;

namespace math
{

namespace integrators
{

// file-scoped variables
static ButcherTableau butcherTableau(butcherTableauCoefficients);

// register factories...
static FactoryRegistrar<RungeKutta>
factory(factoryName, &RungeKutta4::create);

/**
 * Constructor
 */
RungeKutta4::RungeKutta4(void)
: RungeKutta(butcherTableauCoefficients)
{

}

/**
 * Copy constructor
 */
RungeKutta4::RungeKutta4(const RungeKutta4 &rungeKutta)
: RungeKutta(rungeKutta)
{
    operator = (rungeKutta);
}

/**
 * Move constructor
 */
RungeKutta4::RungeKutta4(RungeKutta4 &&rungeKutta)
: RungeKutta(std::move(rungeKutta))
{
    operator = (rungeKutta);
}

/**
 * Destructor
 */
RungeKutta4::~RungeKutta4(void)
{

}

/**
 * Copy assignment operator
 */
RungeKutta4 &RungeKutta4::operator = (const RungeKutta4 &rungeKutta)
{
    if (&rungeKutta != this)
    {
        RungeKutta::operator = (rungeKutta);

        // TODO: copy members here
    }

    return *this;
}

/**
 * Move assignment operator
 */
RungeKutta4 &RungeKutta4::operator = (RungeKutta4 &&rungeKutta)
{
    if (&rungeKutta != this)
    {
        RungeKutta::operator = (std::move(rungeKutta));

        // TODO: move members here
    }

    return *this;
}

/**
 * clone() function
 */
RungeKutta4 *RungeKutta4::clone(void) const
{
    return new RungeKutta4(*this);
}

/**
 * create() function
 */
RungeKutta4 *RungeKutta4::create(void)
{
    return new RungeKutta4();
}

/**
 * Get the name of this class
 */
std::string RungeKutta4::getClassName(void) const
{
    return "RungeKutta4";
}

/**
 * Get the factory name of this constructible
 */
std::string RungeKutta4::getFactoryName(void) const
{
    return factoryName;
}

}

}
