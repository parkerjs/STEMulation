#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "cloneable.h"
#include <string>

namespace math
{

namespace signal_processing
{

namespace waveforms
{

/**
 * This class implements an abstract base class for derived classes which describe time-
 * varying waveforms
 */
template<typename T = double>
class Waveform
: public attributes::interfaces::Cloneable<Waveform<T>>
{
protected:

    /**
     * Constructor
     */
    Waveform(void)
    {

    }

    /**
     * Constructor
     * @param name      the name of the waveform
     * @param amplitude the waveform amplitude
     */
    Waveform(const std::string &name, T amplitude = (T)1)
    : m_amplitude(amplitude),
      m_bias(0),
      m_name(name)
    {

    }

    /**
     * Copy constructor
     */
    Waveform(const Waveform<T> &waveform)
    {
        operator = (waveform);
    }

    /**
     * Move constructor
     */
    Waveform(Waveform<T> &&waveform)
    {
        operator = (std::move(waveform));
    }

public:

    /**
     * Destructor
     */
    virtual ~Waveform(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Waveform<T> &operator = (const Waveform<T> &waveform)
    {
        if (&waveform != this)
        {
            m_amplitude = waveform.m_amplitude;
            m_bias = waveform.m_bias;
            m_name = waveform.m_name;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Waveform<T> &operator = (Waveform<T> &&waveform)
    {
        if (&waveform != this)
        {
            m_amplitude = std::move(waveform.m_amplitude);
            m_bias = std::move(waveform.m_bias);
            m_name = std::move(waveform.m_name);
        }

        return *this;
    }

    /**
     * Compute the waveform value at time t
     */
    virtual T operator () (T time) = 0;

    /**
     * clone() function
     */
    virtual Waveform<T> *clone(void) const override = 0;

    /**
     * Get waveform amplitude
     */
    inline virtual T getAmplitude(void) const
    {
        return m_amplitude;
    }

    /**
     * Get waveform bias
     */
    inline T getBias(void) const
    {
        return m_bias;
    }

    /**
     * Get waveform name
     */
    inline std::string getName(void) const
    {
        return m_name;
    }

    /**
     * Set waveform amplitude
     */
    inline virtual void setAmplitude(T amplitude)
    {
        m_amplitude = amplitude;
    }

    /**
     * Set waveform bias
     */
    inline void setBias(T bias)
    {
        m_bias = bias;
    }

    /**
     * Set waveform name
     */
    inline void setName(const std::string &name)
    {
        m_name = name;
    }

protected:

    /**
     * waveform amplitude
     */
    T m_amplitude;

    /**
     * waveform bias
     */
    T m_bias;

    /**
     * waveform name
     */
    std::string m_name;
};

}

}

}

#endif
