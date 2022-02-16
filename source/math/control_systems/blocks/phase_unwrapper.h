#ifndef PHASE_UNWRAPPER_H
#define PHASE_UNWRAPPER_H

#include "limiter.h"
#include <cmath>

namespace math
{

namespace control_systems
{

namespace blocks
{

/**
 * This class implements a phase un-wrapper
 */
template<typename T>
class PhaseUnwrapper
: public Limiter<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Constructor
     * @param lowerLimit the lower limit value
     * @param upperLimit the upper limit value
     */
    PhaseUnwrapper(T lowerLimit,
                   T upperLimit)
    : Limiter<T>(lowerLimit,
                 upperLimit)
    {

    }

    /**
     * Copy constructor
     */
    PhaseUnwrapper(const PhaseUnwrapper<T> &phaseUnwrapper)
    {
        operator = (phaseUnwrapper);
    }

    /**
     * Move constructor
     */
    PhaseUnwrapper(PhaseUnwrapper<T> &&phaseUnwrapper)
    {
        operator = (std::move(phaseUnwrapper));
    }

    /**
     * Destructor
     */
    virtual ~PhaseUnwrapper(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    PhaseUnwrapper<T> &operator = (const PhaseUnwrapper<T> &phaseUnwrapper)
    {
        if (&phaseUnwrapper != this)
        {
            Limiter<T>::operator = (phaseUnwrapper);

            // TODO: Copy members here
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    PhaseUnwrapper<T> &operator = (PhaseUnwrapper<T> &&phaseUnwrapper)
    {
        if (&phaseUnwrapper != this)
        {
            Limiter<T>::operator = (std::move(phaseUnwrapper));

            // TODO: Move members here
        }

        return *this;
    }

    /**
     * Function to apply this phase un-wrapper to a signal using this object's upper and lower limits
     * @param signal the input to be unwrapped
     */
    inline virtual T apply(T signal) override
    {
        return PhaseUnwrapper<T>::apply(signal,
                                        this->m_lowerLimit,
                                        this->m_upperLimit);
    }

    /**
     * Function to apply this phase un-wrapper to a signal using the specified lower and upper limits
     * @param signal     the input to be unwrapped
     * @param lowerLimit the lower limit value
     * @param upperLimit the upper limit value
     */
    static T apply(T signal,
                   T lowerLimit,
                   T upperLimit)
    {
        // perform phase un-wrapping
        auto signum = signal < 0 ? -1.0 : 1.0;
        auto bandWidth = std::fabs(upperLimit - lowerLimit);
        if (bandWidth != 0.0)
        {
            auto numCycles = static_cast<int>(std::floor(std::fabs(signal) / bandWidth));
            if (numCycles != 0)
            {
                signal -= signum * numCycles * bandWidth;
            }
        }

        if (std::fabs(signal) == std::numeric_limits<T>::infinity() ||
            std::fabs(signal) == std::numeric_limits<T>::max())
        {
            signal = signum * upperLimit;
        }
        else
        {
            while (signal < lowerLimit)
            {
                signal += bandWidth;
            }

            while (signal >= upperLimit)
            {
                signal -= bandWidth;
            }
        }

        return signal;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "PhaseUnwrapper";
    }
};

}

}

}

#endif
