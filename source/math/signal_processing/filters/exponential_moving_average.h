#ifndef EXPONENTIAL_MOVING_AVERAGE_H
#define EXPONENTIAL_MOVING_AVERAGE_H

#include "moving_average.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing exponential moving averages
 */
template<typename T>
class ExponentialMovingAverage
: public MovingAverage<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Using declarations
     */
    using MovingAverage<T>::filter;

    /**
     * Constructor
     * @param period the number of samples in the moving window
     */
    ExponentialMovingAverage(int period)
    : MovingAverage<T>(period),
      m_pAverage(new statistical::Average<T>())
    {
        m_weighting = 2.0 / (this->m_period + 1.0);
    }

    /**
     * Copy constructor
     */
    ExponentialMovingAverage(const ExponentialMovingAverage<T> &filter)
    : MovingAverage<T>(filter),
      m_pAverage(nullptr)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    ExponentialMovingAverage(ExponentialMovingAverage<T> &&filter)
    : MovingAverage<T>(std::move(filter)),
      m_pAverage(nullptr)
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~ExponentialMovingAverage(void) override
    {
        if (m_pAverage != nullptr)
            delete m_pAverage;
    }

    /**
     * Copy assignment operator
     */
    ExponentialMovingAverage<T> &operator = (const ExponentialMovingAverage<T> &filter)
    {
        if (&filter != this)
        {
            MovingAverage<T>::operator = (filter);

            if (m_pAverage != nullptr)
            {
                delete m_pAverage;
                m_pAverage = nullptr;
            }

            if (filter.m_pAverage != nullptr)
                m_pAverage = filter.m_pAverage->clone();

            m_weighting = filter.m_weighting;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    ExponentialMovingAverage<T> &operator = (ExponentialMovingAverage<T> &&filter)
    {
        if (&filter != this)
        {
            MovingAverage<T>::operator = (std::move(filter));

            if (m_pAverage != nullptr)
            {
                delete m_pAverage;
                m_pAverage = nullptr;
            }

            m_pAverage = std::move(filter.m_pAverage);
            filter.m_pAverage = nullptr;

            m_weighting = std::move(filter.m_weighting);
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual ExponentialMovingAverage<T> *clone(void) const override
    {
        return new ExponentialMovingAverage<T>(*this);
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
        bool bSuccess = (this->m_period > 0 && m_pAverage != nullptr);
        if (bSuccess)
        {
            m_pAverage->initialize();
            auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
            int period = std::min(this->m_period, size);
            for (int i = 0; i < period; ++i)
            {
                m_pAverage->addSample(p_xBegin[i]);
                p_yBegin[i] = m_pAverage->calculate();
            }

            for (int i = period; i < size; ++i)
                p_yBegin[i] = p_yBegin[i - 1] + m_weighting * (p_xBegin[i] - p_yBegin[i - 1]);
        }

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "ExponentialMovingAverage";
    }

    /**
     * Get exponential weighting
     */
    inline virtual T getWeighting(void) const final
    {
        return m_weighting;
    }

    /**
     * Set exponential weighting
     */
    inline virtual void setWeighting(const T &weighting) final
    {
        m_weighting = weighting;
    }

protected:

    /**
     * pointer to member averaging calculator
     */
    statistical::Average<T> *m_pAverage;

    /**
     * exponential weighting
     */
    T m_weighting;
};

}

}

}

#endif
