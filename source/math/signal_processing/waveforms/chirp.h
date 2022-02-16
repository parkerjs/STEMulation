#ifndef CHIRP_H
#define CHIRP_H

#include "trigonometry.h"
#include "waveform.h"

namespace math
{

namespace signal_processing
{

namespace waveforms
{

/**
 * This class implements a chirp waveform. The "Frequency" template parameter is used to pass in a frequency
 * generator function object that is used to compute the current frequency as a function of the starting
 * frequency f0, chirp rate and time, respectively.
 */
template<typename Frequency, typename T = double>
class Chirp
: public Waveform<T>
{
public:

    /**
     * Constructor
     * @param amplitude the chirp amplitude
     * @param frequency the frequency generating function
     * @param f0        the starting frequency
     */
    Chirp(T amplitude, Frequency &frequency, T f0)
    : Waveform<T>("Chirp", amplitude),
      m_f(0),
      m_f0(f0),
      m_frequency(frequency)
    {

    }

    /**
     * Copy constructor
     */
    Chirp(const Chirp<Frequency, T> &waveform)
    : m_frequency(waveform.m_frequency)
    {
        operator = (waveform);
    }

    /**
     * Move constructor
     */
    Chirp(Chirp<Frequency, T> &&waveform)
    : m_frequency(std::move(waveform.m_frequency))
    {
        operator = (std::move(waveform));
    }

    /**
     * Destructor
     */
    virtual ~Chirp(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Chirp<Frequency, T> &operator = (const Chirp<Frequency, T> &waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (waveform);

            m_f = waveform.m_f;
            m_frequency = waveform.m_frequency;
            m_f0 = waveform.m_f0;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Chirp<Frequency, T> &operator = (Chirp<Frequency, T> &&waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (std::move(waveform));

            m_f = std::move(waveform.m_f);
            m_frequency = std::move(waveform.m_frequency);
            m_f0 = std::move(waveform.m_f0);
        }

        return *this;
    }

    /**
     * Function call operator generates the chirp waveform
     */
    inline virtual T operator () (T time)
    {
        m_f = m_frequency(m_f0, time);

        return this->m_bias + this->m_amplitude * std::sin(2 * trigonometry::pi() * m_f * time);
    }

    /**
     * clone() function
     */
    inline virtual Chirp<Frequency, T> *clone(void) const override
    {
        return new Chirp<Frequency, T>(*this);
    }

    /**
     * Get current waveform frequency
     */
    inline T getCurrentFrequency(void) const
    {
        return m_f;
    }

    /**
     * Get waveform starting frequency
     */
    inline T getStartingFrequency(void) const
    {
        return m_f0;
    }

protected:

    /**
     * current frequency
     */
    T m_f;

    /**
     * frequency function
     */
    Frequency &m_frequency;

    /**
     * starting frequency
     */
    T m_f0;
};

}

}

}

#endif
