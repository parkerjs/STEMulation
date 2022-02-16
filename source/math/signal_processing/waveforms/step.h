#ifndef STEP_H
#define STEP_H

#include "waveform.h"

namespace math
{

namespace signal_processing
{

namespace waveforms
{

/**
 * This class implements a step waveform
 */
template<typename T>
class Step
: public Waveform<T>
{
public:

    /**
     * Constructor
     * @param time      the time at which the step function switches on
     * @param amplitude the waveform amplitude
     */
    Step(const T &time, const T &amplitude = 1)
    : Waveform<T>("Step", amplitude),
      m_time(time)
    {

    }

    /**
     * Copy constructor
     */
    Step(const Step<T> &waveform)
    {
        operator = (waveform);
    }

    /**
     * Move constructor
     */
    Step(Step &&waveform)
    {
        operator = (std::move(waveform));
    }

    /**
     * Destructor
     */
    virtual ~Step(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Step<T> &operator = (const Step<T> &waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (waveform);

            m_time = waveform.m_time;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Step<T> &operator = (Step<T> &&waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (std::move(waveform));

            m_time = std::move(waveform.m_time);
        }

        return *this;
    }

    /**
     * Function call operator generates step waveform
     */
    inline virtual T operator () (T time)
    {
        return this->m_bias + this->m_amplitude * ((time >= m_time) ? 1 : 0);
    }

    /**
     * clone() function
     */
    inline virtual Step<T> *clone(void) const override
    {
        return new Step<T>(*this);
    }

protected:

    /**
     * time at which the step function switches on
     */
    T m_time;
};

}

}

}

#endif
