#ifndef MOVING_SUM_H
#define MOVING_SUM_H

#include "digital_filter.h"
#include "sum.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing moving summations
 */
template<typename T>
class MovingSum
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
     * @param period the number of samples in the moving window
     */
    MovingSum(int period)
    : m_pCalculator(new statistical::Sum<T>()),
      m_period(period)
    {

    }

    /**
     * Copy constructor
     */
    MovingSum(const MovingSum<T> &filter)
    : DigitalFilter<T>(filter),
      m_pCalculator(nullptr)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    MovingSum(MovingSum<T> &&filter)
    : DigitalFilter<T>(std::move(filter)),
      m_pCalculator(nullptr)
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~MovingSum(void) override
    {
        if (m_pCalculator != nullptr)
            delete m_pCalculator;
    }

    /**
     * Copy assignment operator
     */
    MovingSum<T> &operator = (const MovingSum<T> &filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (filter);

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
    MovingSum<T> &operator = (MovingSum<T> &&filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (std::move(filter));

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
    inline virtual MovingSum<T> *clone(void) const override
    {
        return new MovingSum<T>(*this);
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
        bool bSuccess = (m_period > 0 && m_pCalculator != nullptr);
        if (bSuccess)
        {
            bool bBufferSamples = (p_xBegin == p_yBegin); // need to buffer samples if x and y are the same
                                                          // vector...
            if (bBufferSamples)
            {
                m_buffer.resize(m_period);
                std::copy(p_yBegin, p_yBegin + m_period, m_buffer.begin());
            }

            m_pCalculator->initialize();
            auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
            for (int i = 0; i < size; ++i)
            {
                m_pCalculator->addSample(p_xBegin[i]);
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

                    bSuccess &= m_pCalculator->deleteSample(sample);
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
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "MovingSum";
    }

    /**
     * Get number of samples in the moving sum
     */
    inline virtual int getPeriod(void) const final
    {
        return m_period;
    }

    /**
     * Set number of samples in the moving sum
     */
    inline virtual void setPeriod(int period) final
    {
        m_period = period;
    }

protected:

    /**
     * buffer vector to hold values when source and destination vectors are the same
     */
    std::vector<T> m_buffer;

    /**
     * a pointer to a statistical calculator object
     */
    statistical::StatisticalCalculator<T> *m_pCalculator;

    /**
     * number of samples in the moving sum
     */
    int m_period;
};

}

}

}

#endif
