#ifndef COVARIANCE_H
#define COVARIANCE_H

#include "statistical_calculator.h"
#include <cmath>

namespace math
{

namespace statistical
{

/**
 * Class for computing the covariance of two collections of samples using a variation of Welford's algorithm
 */
template<typename T>
class Covariance
: public StatisticalCalculator<std::pair<T, T>, T>
{
public:

    /**
     * Constructor
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    Covariance(bool bBiasedEstimate = false)
    : m_bBiasedEstimate(bBiasedEstimate)
    {
        Covariance<T>::initialize();
    }

    /**
     * Copy constructor
     */
    Covariance(const Covariance<T> &covariance)
    : StatisticalCalculator<std::pair<T, T>, T>(covariance),
      m_bBiasedEstimate(false)
    {
        operator = (covariance);
    }

    /**
     * Move constructor
     */
    Covariance(Covariance<T> &&covariance)
    : StatisticalCalculator<std::pair<T, T>, T>(std::move(covariance)),
      m_bBiasedEstimate(false)
    {
        operator = (std::move(covariance));
    }

    /**
     * Destructor
     */
    virtual ~Covariance(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    Covariance<T> &operator = (const Covariance<T> &covariance)
    {
        if (&covariance != this)
        {
            StatisticalCalculator<std::pair<T, T>, T>::operator = (covariance);

            m_bBiasedEstimate = covariance.m_bBiasedEstimate;
            m_mean[0] = covariance.m_mean[0];
            m_mean[1] = covariance.m_mean[1];
            m_sum = covariance.m_sum;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Covariance<T> &operator = (Covariance<T> &&covariance)
    {
        if (&covariance != this)
        {
            StatisticalCalculator<std::pair<T, T>, T>::operator = (std::move(covariance));

            m_bBiasedEstimate = std::move(covariance.m_bBiasedEstimate);
            m_mean[0] = std::move(covariance.m_mean[0]);
            m_mean[1] = std::move(covariance.m_mean[1]);
            m_sum = std::move(covariance.m_sum);
        }

        return *this;
    }

    /**
     * Function to add a sample pair to the collection
     */
    inline virtual void addSample(const std::pair<T, T> &sample) override
    {
        addSample(sample.first, sample.second);
    }

    /**
     * Function to add a sample pair to the collection
     */
    inline virtual void addSample(const T &x, const T &y)
    {
        ++this->m_numSamples;
        T delta_x = (x - m_mean[0]) / this->m_numSamples;
        m_mean[0] += delta_x;
        T delta_y = (y - m_mean[1]) / this->m_numSamples;
        m_mean[1] += delta_y;
        m_sum += (this->m_numSamples - 1) * delta_x * delta_y - m_sum / this->m_numSamples;
    }

    /**
     * Function to calculate the result
     */
    inline virtual T calculate(void) const override
    {
        if (m_bBiasedEstimate)
            return m_sum;
        else
            return this->m_numSamples > 1 ? this->m_numSamples / (this->m_numSamples - 1.0) * m_sum : 0.0;
    }

    /**
     * clone() function
     */
    inline virtual Covariance<T> *clone(void) const override
    {
        return new Covariance<T>(*this);
    }

    /**
     * Function to delete a sample pair from the collection. Function returns successful if there is at least
     * one sample in the collection to be removed
     */
    inline virtual bool deleteSample(const std::pair<T, T> &sample) override
    {
        return deleteSample(sample.first, sample.second);
    }

    /**
     * Function to delete a sample pair from the collection. Function returns successful if there is at least
     * one sample in the collection to be removed
     */
    inline virtual bool deleteSample(const T &x, const T &y)
    {
        bool bSuccess = true;
        if (this->m_numSamples == 0)
            bSuccess = false;
        else if (this->m_numSamples == 1)
        {
            bSuccess = (std::fabs(x - m_mean[0]) < 1.0e-15 && std::fabs(y -m_mean[1]) < 1.0e-15);
            if (bSuccess)
                initialize();
        }
        else if (this->m_numSamples > 1)
        {
            --this->m_numSamples;
            T delta_x = (x - m_mean[0]) / this->m_numSamples;
            m_mean[0] -= delta_x;
            T delta_y = (y - m_mean[1]) / this->m_numSamples;
            m_mean[1] -= delta_y;
            m_sum = (this->m_numSamples + 1) * (m_sum / this->m_numSamples - delta_x * delta_y);
        }

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
        m_mean[0] = 0.0;
        m_mean[1] = 0.0;
        this->m_numSamples = 0;
        m_sum = 0.0;

        return true;
    }

    /**
     * Enable/disable biased estimate
     */
    inline virtual void setBiasedEstimate(bool bBiasedEstimate)
    {
        m_bBiasedEstimate = bBiasedEstimate;
    }

protected:

    /**
     * flag to indicate that the estimate will be biased
     */
    bool m_bBiasedEstimate;

    /**
     * sample means
     */
    T m_mean[2];

    /**
     * running sum of samples
     */
    T m_sum;
};

}

}

#endif
