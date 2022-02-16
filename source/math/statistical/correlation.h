#ifndef CORRELATION_H
#define CORRELATION_H

#include "covariance.h"
#include "standard_deviation.h"

namespace math
{

namespace statistical
{

/**
 * Class for computing the correlation of two data series
 */
template<typename T>
class Correlation
: public Covariance<T>
{
public:

    /**
     * Using declarations
     */
    using Covariance<T>::addSample;
    using Covariance<T>::deleteSample;

    /**
     * Constructor
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    Correlation(bool bBiasedEstimate = false)
    : Covariance<T>(bBiasedEstimate),
      m_pStandardDevX(new StandardDeviation<T>()),
      m_pStandardDevY(new StandardDeviation<T>())
    {
        Correlation<T>::initialize();
    }

    /**
     * Copy constructor
     */
    Correlation(const Correlation<T> &correlation)
    : Covariance<T>(correlation),
      m_pStandardDevX(nullptr),
      m_pStandardDevY(nullptr)
    {
        operator = (correlation);
    }

    /**
     * Move constructor
     */
    Correlation(Correlation<T> &&correlation)
    : Covariance<T>(std::move(correlation)),
      m_pStandardDevX(nullptr),
      m_pStandardDevY(nullptr)
    {
        operator = (std::move(correlation));
    }

    /**
     * Destructor
     */
    virtual ~Correlation(void) override
    {
        if (m_pStandardDevX != nullptr)
            delete m_pStandardDevX;

        if (m_pStandardDevY != nullptr)
            delete m_pStandardDevY;
    }

private:

    /**
     * Copy assignment operator
     */
    Correlation<T> &operator = (const Correlation<T> &correlation)
    {
        if (&correlation != this)
        {
            Covariance<T>::operator = (correlation);

            if (m_pStandardDevX != nullptr)
            {
                delete m_pStandardDevX;
                m_pStandardDevX = nullptr;
            }

            if (correlation.m_pStandardDevX != nullptr)
                m_pStandardDevX = correlation.m_pStandardDevX->clone();

            if (m_pStandardDevY != nullptr)
            {
                delete m_pStandardDevY;
                m_pStandardDevY = nullptr;
            }

            if (correlation.m_pStandardDevY != nullptr)
                m_pStandardDevY = correlation.m_pStandardDevY->clone();
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Correlation<T> &operator = (Correlation<T> &&correlation)
    {
        if (&correlation != this)
        {
            Covariance<T>::operator = (std::move(correlation));

            if (m_pStandardDevX != nullptr)
            {
                delete m_pStandardDevX;
                m_pStandardDevX = nullptr;
            }

            m_pStandardDevX = std::move(correlation.m_pStandardDevX);
            correlation.m_pStandardDevX = nullptr;

            if (m_pStandardDevY != nullptr)
            {
                delete m_pStandardDevY;
                m_pStandardDevY = nullptr;
            }

            m_pStandardDevY = std::move(correlation.m_pStandardDevY);
            correlation.m_pStandardDevY = nullptr;
        }

        return *this;
    }

public:

    /**
     * Function to add a sample pair to the collection
     */
    inline virtual void addSample(const T &x, const T &y) override
    {
        Covariance<T>::addSample(x, y);

        if (m_pStandardDevX != nullptr)
            m_pStandardDevX->addSample(x);

        if (m_pStandardDevY != nullptr)
            m_pStandardDevY->addSample(y);
    }

    /**
     * Function to calculate the result
     */
    inline virtual T calculate(void) const override
    {
        T result = 0.0;
        if (m_pStandardDevX != nullptr && m_pStandardDevY != nullptr)
        {
            auto &&cov_xy = Covariance<T>::calculate();
            auto &&stdDev_x = m_pStandardDevX->calculate();
            auto &&stdDev_y = m_pStandardDevY->calculate();

            if (stdDev_x != 0.0 && stdDev_y != 0.0)
                result = cov_xy / stdDev_x / stdDev_y;
        }

        return result;
    }

    /**
     * clone() function
     */
    inline virtual Correlation<T> *clone(void) const override
    {
        return new Correlation<T>(*this);
    }

    /**
     * Function to delete a sample pair from the collection. Function returns successful if there is at least
     * one sample in the collection to be removed
     */
    inline virtual bool deleteSample(const T &x, const T &y) override
    {
        bool bSuccess = (m_pStandardDevX != nullptr && m_pStandardDevY != nullptr);
        if (bSuccess)
        {
            bSuccess = Covariance<T>::deleteSample(x, y);
            if (bSuccess)
            {
                bSuccess &= m_pStandardDevX->deleteSample(x);
                bSuccess &= m_pStandardDevY->deleteSample(y);
            }
        }

        return bSuccess;
    }

    /**
     * Initialization function
     */
    inline virtual bool initialize(void) override
    {
        bool bSuccess = Covariance<T>::initialize();
        if (bSuccess)
        {
            bSuccess = (m_pStandardDevX != nullptr);
            if (bSuccess)
                bSuccess = m_pStandardDevX->initialize();

            bSuccess = (m_pStandardDevY != nullptr);
            if (bSuccess)
                bSuccess = m_pStandardDevY->initialize();
        }

        return bSuccess;
    }

    /**
     * Enable/disable biased estimate
     */
    inline void setBiasedEstimate(bool bBiasedEstimate) override
    {
        Covariance<T>::setBiasedEstimate(bBiasedEstimate);
        if (m_pStandardDevX != nullptr)
            m_pStandardDevX->setBiasedEstimate(bBiasedEstimate);

        if (m_pStandardDevY != nullptr)
            m_pStandardDevY->setBiasedEstimate(bBiasedEstimate);
    }

private:

    /**
     * standard deviation for x samples
     */
    StandardDeviation<T> *m_pStandardDevX;

    /**
     * standard deviation for y samples
     */
    StandardDeviation<T> *m_pStandardDevY;
};

}

}

#endif
