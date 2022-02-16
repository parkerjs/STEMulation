#ifndef SINUSOID_H
#define SINUSOID_H

#include "trigonometry.h"
#include "waveform.h"

namespace math
{

namespace signal_processing
{

namespace waveforms
{

/**
 * This class implements a sinusoidal waveform
 */
template<typename T = double>
class Sinusoid
: public Waveform<T>
{
public:

    /**
     * Constructor
     * @param omega     the sinusoid angular frequency in rads / s
     * @param amplitude the waveform amplitude
     */
    Sinusoid(T omega = 1, T amplitude = 1)
    : Waveform<T>("Sinusoid", amplitude),
      m_omega(omega)
    {

    }

    /**
     * Copy constructor
     */
    Sinusoid(const Sinusoid<T> &waveform)
    {
        operator = (waveform);
    }

    /**
     * Move constructor
     */
    Sinusoid(Sinusoid<T> &&waveform)
    {
        operator = (std::move(waveform));
    }

    /**
     * Destructor
     */
    virtual ~Sinusoid(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Sinusoid<T> &operator = (const Sinusoid<T> &waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (waveform);

            m_omega = waveform.m_omega;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Sinusoid<T> &operator = (Sinusoid<T> &&waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (std::move(waveform));

            m_omega = std::move(waveform.m_omega);
        }

        return *this;
    }

    /**
     * Function call operator generates step waveform
     */
    inline virtual T operator () (T time)
    {
        return this->m_bias + this->m_amplitude * std::sin(m_omega * time);
    }

    /**
     * clone() function
     */
    inline virtual Sinusoid<T> *clone(void) const override
    {
        return new Sinusoid<T>(*this);
    }

    /**
     * Get waveform angular frequency
     */
    inline T getFrequency(void) const
    {
        return m_omega;
    }

    /**
     * Get waveform period
     */
    inline T getPeriod(void) const
    {
        return 2 * trigonometry::pi() / m_omega;
    }

    /**
     * Set waveform angular frequency
     */
    inline void setFrequency(T omega)
    {
        m_omega = omega;
    }

    /**
     * Set waveform period
     */
    inline void setPeriod(T period)
    {
        m_omega = 2 * trigonometry::pi() / period;
    }

protected:

    /**
     * angular frequency in rads / s
     */
    T m_omega;
};

}

}

}

#endif
