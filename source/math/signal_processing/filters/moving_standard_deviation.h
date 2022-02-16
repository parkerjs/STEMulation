#ifndef MOVING_STANDARD_DEVIATION_H
#define MOVING_STANDARD_DEVIATION_H

#include "moving_variance.h"
#include "standard_deviation.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing moving standard deviations
 */
template<typename T>
class MovingStandardDeviation
: public MovingVariance<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Using declarations
     */
    using MovingVariance<T>::filter;

    /**
     * Constructor
     * @param period          the number of samples in the moving window
     * @param bBiasedEstimate a flag that indicates that the estimate will be biased (false by default)
     */
    MovingStandardDeviation(int period, bool bBiasedEstimate /* = false*/)
    : MovingVariance<T>(period, bBiasedEstimate),
      m_pStandardDeviation(new statistical::StandardDeviation<T>(bBiasedEstimate))
    {

    }

    /**
     * Copy constructor
     */
    MovingStandardDeviation(const MovingStandardDeviation<T> &filter)
    : MovingVariance<T>(filter),
      m_pStandardDeviation(nullptr)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    MovingStandardDeviation(MovingStandardDeviation<T> &&filter)
    : MovingVariance<T>(std::move(filter)),
      m_pStandardDeviation(nullptr)
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~MovingStandardDeviation(void) override
    {
        if (m_pStandardDeviation != nullptr)
            delete m_pStandardDeviation;
    }

    /**
     * Copy assignment operator
     */
    MovingStandardDeviation<T> &operator = (const MovingStandardDeviation<T> &filter)
    {
        if (&filter != this)
        {
            MovingVariance<T>::operator = (filter);

            if (m_pStandardDeviation != nullptr)
            {
                delete m_pStandardDeviation;
                m_pStandardDeviation = nullptr;
            }

            if (filter.m_pStandardDeviation != nullptr)
                m_pStandardDeviation = filter.m_pStandardDeviation->clone();
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    MovingStandardDeviation<T> &operator = (MovingStandardDeviation<T> &&filter)
    {
        if (&filter != this)
        {
            MovingVariance<T>::operator = (std::move(filter));

            if (m_pStandardDeviation != nullptr)
            {
                delete m_pStandardDeviation;
                m_pStandardDeviation = nullptr;
            }

            m_pStandardDeviation = std::move(filter.m_pStandardDeviation);
            filter.m_pStandardDeviation = nullptr;
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual MovingStandardDeviation<T> *clone(void) const override
    {
        return new MovingStandardDeviation<T>(*this);
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
    bool filter(const T *p_xBegin, const T *p_xEnd, T *p_yBegin) final
    {
        bool bSuccess = (this->m_period > 0 && m_pStandardDeviation != nullptr);
        if (bSuccess)
        {
            bool bBufferSamples = (p_xBegin == p_yBegin); // need to buffer samples if x and y are the same
                                                          // vector...
            if (bBufferSamples)
            {
                this->m_buffer.resize(this->m_period);
                std::copy(p_yBegin, p_yBegin + this->m_period, this->m_buffer.begin());
            }

            m_pStandardDeviation->initialize();
            auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
            for (int i = 0; i < size; ++i)
            {
                m_pStandardDeviation->addSample(p_xBegin[i]);
                if (i >= this->m_period)
                {
                    T sample;
                    if (bBufferSamples)
                    {
                        sample = this->m_buffer.front();
                        std::rotate(this->m_buffer.begin(), this->m_buffer.begin() + 1, this->m_buffer.end());
                        this->m_buffer.back() = p_xBegin[i];
                    }
                    else
                        sample = p_xBegin[i - this->m_period];

                    bSuccess &= m_pStandardDeviation->deleteSample(sample);
                }

                if (bSuccess)
                    p_yBegin[i] = m_pStandardDeviation->calculate();
                else
                    break;
            }
        }

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "MovingStandardDeviation";
    }

    /**
     * Enable/disable biased estimate
     */
    inline void setBiasedEstimate(bool bBiasedEstimate) final
    {
        MovingVariance<T>::setBiasedEstimate(bBiasedEstimate);
        if (m_pStandardDeviation != nullptr)
            m_pStandardDeviation->setBiasedEstimate(bBiasedEstimate);
    }

private:

    /**
     * pointer to member standard deviation calculator
     */
    statistical::StandardDeviation<T> *m_pStandardDeviation;
};

}

}

}

#endif
