#ifndef STATISTICAL_VARIANCE_H
#define STATISTICAL_VARIANCE_H

#include "statistical_calculator.h"
#include <cmath>

namespace math
{

namespace statistical
{

/**
 * Class for computing the variance of a collection of samples using Welford's algorithm
 */
template<typename T>
class StatisticalVariance
: public StatisticalCalculator<T>
{
public:

    /**
     * Constructor
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    StatisticalVariance(bool bBiasedEstimate = false)
    : m_bBiasedEstimate(bBiasedEstimate)
    {
        StatisticalVariance<T>::initialize();
    }

    /**
     * Copy constructor
     */
    StatisticalVariance(const StatisticalVariance<T> &variance)
    : StatisticalCalculator<T>(variance),
      m_bBiasedEstimate(false)
    {
        operator = (variance);
    }

    /**
     * Move constructor
     */
    StatisticalVariance(StatisticalVariance<T> &&variance)
    : StatisticalCalculator<T>(std::move(variance)),
      m_bBiasedEstimate(false)
    {
        operator = (std::move(variance));
    }

    /**
     * Destructor
     */
    virtual ~StatisticalVariance(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    StatisticalVariance<T> &operator = (const StatisticalVariance<T> &variance)
    {
        if (&variance != this)
        {
            StatisticalCalculator<T>::operator = (variance);

            m_bBiasedEstimate = variance.m_bBiasedEstimate;
            m_mean = variance.m_mean;
            m_sum = variance.m_sum;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    StatisticalVariance<T> &operator = (StatisticalVariance<T> &&variance)
    {
        if (&variance != this)
        {
            StatisticalCalculator<T>::operator = (std::move(variance));

            m_bBiasedEstimate = std::move(variance.m_bBiasedEstimate);
            m_mean = std::move(variance.m_mean);
            m_sum = std::move(variance.m_sum);
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
        T delta(x - m_mean);
        m_mean += delta / this->m_numSamples;
        m_sum += delta * (x - m_mean);
    }

    /**
     * Function to calculate the result
     */
    inline virtual T calculate(void) const override
    {
        long numSamples = this->m_numSamples - 1;
        if (m_bBiasedEstimate)
            ++numSamples;

        return this->m_numSamples > 1 ? m_sum / numSamples : 0.0;
    }

    /**
     * clone() function
     */
    inline virtual StatisticalVariance<T> *clone(void) const override
    {
        return new StatisticalVariance<T>(*this);
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
            bSuccess = (std::fabs(x - m_sum) < 1.0e-15);
            if (bSuccess)
                initialize();
        }
        else if (this->m_numSamples > 1)
        {
            --this->m_numSamples;
            T delta(x - m_mean);
            m_mean -= delta / this->m_numSamples;
            m_sum -= delta * (x - m_mean);
            if (m_sum < 0.0)
                m_sum = 0.0; // this happens mostly due to numerical roundoff, so we have
        }                    // to set this to zero since variance cannot be less than zero

        return bSuccess;
    }

    /**
     * Query whether or not biased estimate is enabled/disabled
     */
    inline virtual bool getBiasedEstimate(void) const final
    {
        return m_bBiasedEstimate;
    }

    /**
     * Initialization function
     */
    inline virtual bool initialize(void) override
    {
        m_mean = 0.0;
        this->m_numSamples = 0;
        m_sum = 0.0;

        return true;
    }

    /**
     * Enable/disable biased estimate
     */
    inline virtual void setBiasedEstimate(bool bBiasedEstimate) final
    {
        m_bBiasedEstimate = bBiasedEstimate;
    }

protected:

    /**
     * flag to indicate that the estimate will be biased
     */
    bool m_bBiasedEstimate;

    /**
     * sample mean
     */
    T m_mean;

    /**
     * running sum of samples
     */
    T m_sum;
};

}

}

#endif
