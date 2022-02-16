#ifndef SQUARE_H
#define SQUARE_H

#include "waveform.h"

namespace math
{

namespace signal_processing
{

namespace waveforms
{

/**
 * This class implements a square waveform
 */
template<typename T = double>
class Square
: public Waveform<T>
{
public:

    /**
     * Constructor
     * @param amplitude the amplitude
     * @param period    the period of the square wave
     */
    Square(const T &amplitude, const T &period)
    : Waveform<T>("Square", amplitude),
      m_period(period)
    {

    }

    /**
     * Copy constructor
     */
    Square(const Square<T> &waveform)
    {
        operator = (waveform);
    }

    /**
     * Move constructor
     */
    Square(Square<T> &&waveform)
    {
        operator = (std::move(waveform));
    }

    /**
     * Destructor
     */
    virtual ~Square(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Square<T> &operator = (const Square<T> &waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (waveform);

            m_period = waveform.m_period;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Square<T> &operator = (Square<T> &&waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (std::move(waveform));

            m_period = std::move(waveform.m_period);
        }

        return *this;
    }

    /**
     * Function call operator generates step waveform
     */
    inline virtual T operator () (T time)
    {
        return this->m_bias + ((int(2 * time / m_period) % 2) ? -this->m_amplitude : this->m_amplitude);
    }

    /**
     * clone() function
     */
    inline virtual Square<T> *clone(void) const override
    {
        return new Square<T>(*this);
    }

    /**
     * Get waveform angular frequency
     */
    inline T getFrequency(void) const
    {
        return 1 / m_period;
    }

    /**
     * Get waveform period
     */
    inline T getPeriod(void) const
    {
        return m_period;
    }

    /**
     * Set waveform angular frequency
     */
    inline void setFrequency(T frequency)
    {
        m_period = 1 / frequency;
    }

    /**
     * Set waveform period
     */
    inline void setPeriod(T period)
    {
        m_period = period;
    }

protected:

    /**
     * the period
     */
    T m_period;
};

}

}

}

#endif
