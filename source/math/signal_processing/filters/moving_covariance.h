#ifndef MOVING_COVARIANCE_H
#define MOVING_COVARIANCE_H

#include "covariance.h"
#include "digital_filter.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing moving covariance
 */
template<typename T>
class MovingCovariance
: protected DigitalFilter<T>
{
public:

    /**
     * Typedef declarations
     */
    using DigitalFilter<T>::filter;

    /**
     * Constructor
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    MovingCovariance(int period, bool bBiasedEstimate = false)
    : m_bBiasedEstimate(bBiasedEstimate),
      m_pCalculator(new Statistical::Covariance<T>(bBiasedEstimate)),
      m_period(period)
    {

    }

    /**
     * Copy constructor
     */
    MovingCovariance(const MovingCovariance<T> &filter)
    : DigitalFilter<T>(filter),
      m_pCalculator(nullptr)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    MovingCovariance(MovingCovariance<T> &&filter)
    : DigitalFilter<T>(std::move(filter)),
      m_pCalculator(nullptr)
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~MovingCovariance(void) override
    {
        if (m_pCalculator != nullptr)
            delete m_pCalculator;
    }

    /**
     * Copy assignment operator
     */
    MovingCovariance<T> &operator = (const MovingCovariance<T> &filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (filter);

            m_bBiasedEstimate = filter.m_bBiasedEstimate;
            m_buffer = filter.m_buffer;

            if (m_pCalculator != nullptr)
            {
                delete m_pCalculator;
                m_pCalculator = nullptr;
            }

            if (filter.m_pCalculator != nullptr)
                m_pCalculator = filter.m_pCalculator->clone();

            m_period = filter.m_period;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    MovingCovariance<T> &operator = (MovingCovariance<T> &&filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (std::move(filter));

            m_bBiasedEstimate = std::move(filter.m_bBiasedEstimate);
            m_buffer = std::move(filter.m_buffer);

            if (m_pCalculator != nullptr)
            {
                delete m_pCalculator;
                m_pCalculator = nullptr;
            }

            m_pCalculator = std::move(filter.m_pCalculator);
            filter.m_pCalculator = nullptr;

            m_period = std::move(filter.m_period);
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual MovingCovariance<T> *clone(void) const override
    {
        return new MovingCovariance<T>(*this);
    }

    /**
     * This function calculates the moving covariance of two data sequences.
     * @param      p_xBegin a pointer to the beginning of the x data sequence
     * @param      p_xEnd   a pointer to the element following the end of the range that defines the x data
     *                      sequence (if there are N elements [0 ... N-1], the pointer refers to the element
     *                      that would follow element N-1)
     * @param[in]  p_yBegin a pointer to a range containing the y data sequence
     * @param[out] p_yBegin points to the calculated moving covariance values
     */
    virtual bool filter(const T *p_xBegin, const T *p_xEnd, T *p_yBegin)
    {
        bool bSuccess = (m_pCalculator != nullptr);
        if (bSuccess)
        {
            m_buffer.resize(m_period);
            std::copy(p_yBegin, p_yBegin + m_period, m_buffer.begin());

            m_pCalculator->initialize();
            auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
            for (int i = 0; i < size; ++i)
            {
                m_pCalculator->addSample(p_xBegin[i], p_yBegin[i]);
                if (i >= m_period)
                {
                    T sample = m_buffer.front();
                    std::rotate(m_buffer.begin(), m_buffer.begin() + 1, m_buffer.end());
                    m_buffer.back() = p_yBegin[i];

                    bSuccess &= m_pCalculator->deleteSample(p_xBegin[i - m_period],
                                                            p_yBegin[i - m_period]);
                }

                if (bSuccess)
                    p_yBegin[i] = m_pCalculator->calculate();
                else
                    break;
            }
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
     * Get number of samples in the moving average
     */
    inline virtual int getPeriod(void) const final
    {
        return m_period;
    }

    /**
     * Enable/disable biased estimate
     */
    inline virtual void setBiasedEstimate(bool bBiasedEstimate) final
    {
        m_bBiasedEstimate = bBiasedEstimate;
        if (m_pCalculator != nullptr)
            m_pCalculator->setBiasedEstimate(bBiasedEstimate);
    }

    /**
     * Set number of samples in the moving average
     */
    inline virtual void setPeriod(int period) final
    {
        m_period = period;
    }

protected:

    /**
     * flag to indicate that the estimate will be biased
     */
    bool m_bBiasedEstimate;

    /**
     * buffer vector to hold values when source and destination vectors are the same
     */
    std::vector<T> m_buffer;

    /**
     * pointer to member covariance calculator
     */
    Statistical::Covariance<T> *m_pCalculator;

    /**
     * number of samples in the moving average
     */
    int m_period;
};

}

}

}

#endif
