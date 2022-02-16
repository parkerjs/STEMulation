#ifndef DELTA_H
#define DELTA_H

#include "trigonometry.h"
#include "waveform.h"
#include <cmath>

namespace math
{

namespace signal_processing
{

namespace waveforms
{

/**
 * This class implements an approximation to the Dirac delta function; it is approximated using a Gaussian
 * distribution with mean t and standard deviation equal to a / sqrt(2)
 */
template<typename T = double>
class Delta
: public Waveform<T>
{
public:

    /**
     * Constructor
     * @param a effectively a "normalized" standard deviation (default = 1)
     */
    Delta(const T a = 1)
    : Waveform<T>("Delta function", 0),
      m_a(a),
      m_s(std::sqrt(trigonometry::pi()))
    {

    }

    /**
     * Copy constructor
     */
    Delta(const Delta<T> &waveform)
    : m_s(waveform.m_s)
    {
        operator = (waveform);
    }

    /**
     * Move constructor
     */
    Delta(Delta<T> &&waveform)
    : m_s(std::move(waveform.m_s))
    {
        operator = (std::move(waveform));
    }

    /**
     * Destructor
     */
    virtual ~Delta(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Delta<T> &operator = (const Delta<T> &waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (waveform);

            m_a = waveform.m_a;
//          m_s = waveform.m_s; // this member is const
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Delta<T> &operator = (Delta<T> &&waveform)
    {
        if (&waveform != this)
        {
            Waveform<T>::operator = (std::move(waveform));

            m_a = std::move(waveform.m_a);
//          m_s = std::move(waveform.m_s); // this member is const
        }

        return *this;
    }

    /**
     * Function call operator computes "approximate" unit impulse value at time t
     */
    inline virtual T operator () (T time)
    {
        return this->m_bias + 1 / (m_a * m_s) * std::exp(-time * time / (m_a * m_a));
    }

    /**
     * clone() function
     */
    inline virtual Delta<T> *clone(void) const override
    {
        return new Delta<T>(*this);
    }

private:

    /**
     * Get waveform amplitude (not used)
     */
    inline virtual T getAmplitude(void) const
    {
        return 0;
    }

    /**
     * Set waveform amplitude (not used)
     */
    inline virtual void setAmplitude(T)
    {
        // do nothing
    }

protected:

    /**
     * "normalized" standard deviation
     */
    T m_a;

    /**
     * square root of pi
     */
    const T m_s;
};

}

}

}

#endif
