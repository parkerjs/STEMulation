#include "butcherTableau.h"
#include "rungeKuttaFehlberg45.h"
#include <cmath>

// file-scoped variables
static constexpr char factoryName[] = "RungeKuttaFehlberg45";
static constexpr double butcherTableauCoefficients[7][7] =
{ { 1./4,   1./4,       0.,         0.,          0.,           0.,     0.    },
  { 3./8,   3./32,      9./32,      0.,          0.,           0.,     0.    },
  { 12./13, 1932./2197,-7200./2197, 7296./2197,  0.,           0.,     0.    },
  { 1.,     439./216,  -8.,         3680./513,  -845./4104,    0.,     0.    },
  { 1./2,  -8./27,      2.,        -3544./2565,  1859./4104,  -11./40, 0.    },
  { 0.,     16./135,    0.,         6656./12825, 28561./56430,-9./50,  2./55 },
//{ 0.,     25./216,    0.,         1408./2565,  2197./4104,  -1./5,   0.    },
  { 0.,     1./360,     0.,        -128./4275,  -2197./75240,  1./50,  2./55 } };

// using namespace declarations
using namespace attributes::abstract;

namespace math
{

namespace integrators
{

// file-scoped variables
static ButcherTableau butcherTableau(butcherTableauCoefficients, 5, 2);

// register factories...
static FactoryRegistrar<AdaptiveRungeKutta, RungeKutta>
factory(factoryName, &RungeKuttaFehlberg45::create);

/**
 * Constructor
 * @param tolerance       the desired error tolerance (default = 1e-4)
 * @param minimumStepSize the minimum step size (default = 1.0e-10)
 * @param maximumStepSize the maximum step size (default = 1e-2)
 */
RungeKuttaFehlberg45::RungeKuttaFehlberg45(double tolerance,
                                           double minimumStepSize,
                                           double maximumStepSize)
: AdaptiveRungeKutta(butcherTableauCoefficients)
{

}

/**
 * Copy constructor
 */
RungeKuttaFehlberg45::RungeKuttaFehlberg45(const RungeKuttaFehlberg45 &rungeKutta)
: AdaptiveRungeKutta(rungeKutta)
{
    operator = (rungeKutta);
}

/**
 * Move constructor
 */
RungeKuttaFehlberg45::RungeKuttaFehlberg45(RungeKuttaFehlberg45 &&rungeKutta)
: AdaptiveRungeKutta(std::move(rungeKutta))
{
    operator = (rungeKutta);
}

/**
 * Destructor
 */
RungeKuttaFehlberg45::~RungeKuttaFehlberg45(void)
{

}

/**
 * Copy assignment operator
 */
RungeKuttaFehlberg45 &RungeKuttaFehlberg45::operator = (const RungeKuttaFehlberg45 &rungeKutta)
{
    if (&rungeKutta != this)
    {
        AdaptiveRungeKutta::operator = (rungeKutta);

        // TODO: copy members here
    }

    return *this;
}

/**
 * Move assignment operator
 */
RungeKuttaFehlberg45 &RungeKuttaFehlberg45::operator = (RungeKuttaFehlberg45 &&rungeKutta)
{
    if (&rungeKutta != this)
    {
        AdaptiveRungeKutta::operator = (std::move(rungeKutta));

        // TODO: move members here
    }

    return *this;
}

/**
 * Function to calculate the adaptive step size as a function of the current step size (h) and local truncation
 * error
 */
double RungeKuttaFehlberg45::calcAdaptiveStepSize(double h,
                                                  double error) const
{
    auto d = 0.84 * std::pow(m_tolerance / error, 0.25);
    if (d <= 0.1)
        return h * 0.1;
    else if (d >= 4.0)
        return h * 4.0;

    return h * d;
}

/**
 * clone() function
 */
RungeKuttaFehlberg45 *RungeKuttaFehlberg45::clone(void) const
{
    return new RungeKuttaFehlberg45(*this);
}

/**
 * create() function
 */
RungeKuttaFehlberg45 *RungeKuttaFehlberg45::create(double tolerance,
                                                   double minimumStepSize,
                                                   double maximumStepSize)
{
    return new RungeKuttaFehlberg45(tolerance, minimumStepSize, maximumStepSize);
}

/**
 * Get the name of this class
 */
std::string RungeKuttaFehlberg45::getClassName(void) const
{
    return "RungeKuttaFehlberg45";
}

/**
 * Get the factory name of this constructible
 */
std::string RungeKuttaFehlberg45::getFactoryName(void) const
{
    return factoryName;
}

}

}
