#ifndef SUM_H
#define SUM_H

#include "statistical_calculator.h"

namespace math
{

namespace statistical
{

/**
 * Class for computing the sum of a collection of samples
 */
template<typename T>
class Sum
: public StatisticalCalculator<T>
{
public:

    /**
     * Constructor
     */
    Sum(void)
    {
        Sum<T>::initialize();
    }

    /**
     * Copy constructor
     */
    Sum(const Sum &sum)
    : StatisticalCalculator<T>(sum)
    {
        operator = (sum);
    }

    /**
     * Move constructor
     */
    Sum(Sum &&sum)
    : StatisticalCalculator<T>(std::move(sum))
    {
        operator = (std::move(sum));
    }

    /**
     * Destructor
     */
    virtual ~Sum(void) override
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    Sum<T> &operator = (const Sum<T> &sum)
    {
        if (&sum != this)
        {
            StatisticalCalculator<T>::operator = (sum);

            m_sum = sum.m_sum;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Sum<T> &operator = (Sum<T> &&sum)
    {
        if (&sum != this)
        {
            StatisticalCalculator<T>::operator = (std::move(sum));

            m_sum = std::move(sum.m_sum);
        }

        return *this;
    }

public:

    /**
     * Function to add a sample to the collection
     */
    inline virtual void addSample(const T &x) override
    {
        ++this->m_numSamples;
        m_sum += x;
    }

    /**
     * Function to calculate the result
     */
    inline virtual T calculate(void) const override
    {
        return this->m_numSamples > 0 ? m_sum : T(0.0);
    }

    /**
     * clone() function
     */
    inline virtual Sum<T> *clone(void) const override
    {
        return new Sum<T>(*this);
    }

    /**
     * Function to delete a sample from the collection. Function returns successful if there is at least one
     * sample in the collection to be removed
     */
    inline virtual bool deleteSample(const T &x) override
    {
        bool bSuccess = true;
        if (this->m_numSamples == 0)
            bSuccess = false;
        else if (this->m_numSamples == 1)
        {
            bSuccess = (x == m_sum);
            if (bSuccess)
                initialize();
        }
        else if (this->m_numSamples > 1)
        {
            --this->m_numSamples;
            m_sum -= x;
        }

        return bSuccess;
    }

    /**
     * Initialization function
     */
    inline virtual bool initialize(void) override
    {
        this->m_numSamples = 0;
        m_sum = T(0.0);

        return bSuccess;
    }

protected:

    /**
     * the sum of all samples in the collection
     */
    T m_sum;
};

}

}

#endif
