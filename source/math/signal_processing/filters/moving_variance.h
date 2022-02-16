#ifndef MOVING_VARIANCE_H
#define MOVING_VARIANCE_H

#include "digital_filter.h"
#include "statistical_variance.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing moving variances
 */
template<typename T>
class MovingVariance
: protected DigitalFilter<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Using declarations
     */
    using DigitalFilter<T>::filter;

    /**
     * Constructor
     * @param period          the number of samples in the moving window
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    MovingVariance(int period, bool bBiasedEstimate /* = false*/)
    : m_bBiasedEstimate(bBiasedEstimate),
      m_period(period),
      m_pVarianceCalculator(new statistical::StatisticalVariance<T>(bBiasedEstimate))
    {

    }

    /**
     * Copy constructor
     */
    MovingVariance(const MovingVariance<T> &filter)
    : DigitalFilter<T>(filter),
      m_pVarianceCalculator(nullptr)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    MovingVariance(MovingVariance<T> &&filter)
    : DigitalFilter<T>(std::move(filter)),
      m_pVarianceCalculator(nullptr)
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~MovingVariance(void) override
    {
        if (m_pVarianceCalculator != nullptr)
            delete m_pVarianceCalculator;
    }

    /**
     * Copy assignment operator
     */
    MovingVariance<T> &operator = (const MovingVariance<T> &filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (filter);

            m_bBiasedEstimate = filter.m_bBiasedEstimate;
            m_buffer = filter.m_buffer;
            m_period = filter.m_period;

            if (m_pVarianceCalculator != nullptr)
            {
                delete m_pVarianceCalculator;
                m_pVarianceCalculator = nullptr;
            }

            if (filter.m_pVarianceCalculator != nullptr)
                m_pVarianceCalculator = filter.m_pVarianceCalculator->clone();
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    MovingVariance<T> &operator = (MovingVariance<T> &&filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (std::move(filter));

            m_bBiasedEstimate = std::move(filter.m_bBiasedEstimate);
            m_buffer = std::move(filter.m_buffer);
            m_period = std::move(filter.m_period);

            if (m_pVarianceCalculator != nullptr)
            {
                delete m_pVarianceCalculator;
                m_pVarianceCalculator = nullptr;
            }

            m_pVarianceCalculator = std::move(filter.m_pVarianceCalculator);
            filter.m_pVarianceCalculator = nullptr;
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual MovingVariance<T> *clone(void) const override
    {
        return new MovingVariance<T>(*this);
    }

    /**
     * Digital signal filtering function
     * @param p_xBegin a pointer to the beginning of the input data sequence to be filtered
     * @param p_xEnd   a pointer to the element following the end of the range defining the input data sequence
     *                 (if there are N elements [0 ... N-1], the pointer refers to the element that would
     *                 follow element N-1)
     * @param p_yBegin a pointer to the beginning of a range that will store the values arising from the
     *                 filtration process
     */
    virtual bool filter(const T *p_xBegin, const T *p_xEnd, T *p_yBegin) override
    {
        bool bSuccess = (m_pVarianceCalculator != nullptr);
        if (bSuccess)
        {
            bool bBufferSamples = (p_xBegin == p_yBegin); // need to buffer samples if x and y are the same
                                                          // vector...
            if (bBufferSamples)
            {
                m_buffer.resize(m_period);
                std::copy(p_yBegin, p_yBegin + m_period, m_buffer.begin());
            }

            m_pVarianceCalculator->initialize();
            auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
            for (int i = 0; i < size; ++i)
            {
                m_pVarianceCalculator->addSample(p_xBegin[i]);
                if (i >= m_period)
                {
                    T sample;
                    if (bBufferSamples)
                    {
                        sample = m_buffer.front();
                        std::rotate(m_buffer.begin(), m_buffer.begin() + 1, m_buffer.end());
                        m_buffer.back() = p_xBegin[i];
                    }
                    else
                        sample = p_xBegin[i - m_period];

                    bSuccess &= m_pVarianceCalculator->deleteSample(sample);
                }

                if (bSuccess)
                    p_yBegin[i] = m_pVarianceCalculator->calculate();
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
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "MovingVariance";
    }

    /**
     * Get number of samples in the moving window
     */
    inline virtual int getPeriod(void) const final
    {
        return m_period;
    }

    /**
     * Enable/disable biased estimate
     */
    inline virtual void setBiasedEstimate(bool bBiasedEstimate)
    {
        m_bBiasedEstimate = bBiasedEstimate;
        if (m_pVarianceCalculator != nullptr)
            m_pVarianceCalculator->setBiasedEstimate(bBiasedEstimate);
    }

    /**
     * Set number of samples in the moving window
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
     * number of samples in the moving window
     */
    int m_period;

    /**
     * pointer to member variance calculator
     */
    statistical::StatisticalVariance<T> *m_pVarianceCalculator;
};

}

}

}

#endif
