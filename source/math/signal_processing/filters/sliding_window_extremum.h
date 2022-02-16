#ifndef SLIDING_WINDOW_EXTREMUM_H
#define SLIDING_WINDOW_EXTREMUM_H

#include "digital_filter.h"
#include <deque>

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing the minimum and maximum values within a trailing sliding window
 */
template<typename T>
class SlidingWindowExtremum
: public DigitalFilter<T>,
  virtual private attributes::abstract::Reflective
{
public:

    /**
     * Enumerations
     */
    enum class ExtremeType { Maximum, Minimum };

    /**
     * Using declarations
     */
    using DigitalFilter<T>::filter;

    /**
     * Constructor
     * @param period the number of samples in the moving window
     * @param type   the type of the desired sliding window extreme (maximum or minimum)
     */
    SlidingWindowExtremum(int period, ExtremeType type)
    : m_period(period)
    {
        switch (type)
        {
            default:
            case ExtremeType::Maximum:
            m_comparator = std::greater<T>();
            break;

            case ExtremeType::Minimum:
            m_comparator = std::less<T>();
        }
    }

    /**
     * Copy constructor
     */
    SlidingWindowExtremum(const SlidingWindowExtremum<T> &filter)
    : DigitalFilter<T>(filter)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    SlidingWindowExtremum(SlidingWindowExtremum<T> &&filter)
    : DigitalFilter<T>(std::move(filter))
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~SlidingWindowExtremum(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    SlidingWindowExtremum<T> &operator = (const SlidingWindowExtremum<T> &filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (filter);

            m_comparator = filter.m_comparator;
            m_period = filter.m_period;
            m_window = filter.m_window;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    SlidingWindowExtremum<T> &operator = (SlidingWindowExtremum<T> &&filter)
    {
        if (&filter != this)
        {
            DigitalFilter<T>::operator = (std::move(filter));

            m_comparator = std::move(filter.m_comparator);
            m_period = std::move(filter.m_period);
            m_window = std::move(filter.m_window);
        }

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual SlidingWindowExtremum<T> *clone(void) const override
    {
        return new SlidingWindowExtremum<T>(*this);
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
        bool bSuccess = (m_period > 0);
        if (bSuccess)
        {
            auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
            std::copy(p_xBegin, p_xEnd, p_yBegin);

            // pair<int, int> represents the pair (array[i], i)
            m_window.clear();
            for (int i = 0; i < size; ++i)
            {
                while (!m_window.empty() && m_comparator(p_yBegin[i], m_window.back().first))
                    m_window.pop_back();

                m_window.emplace_back(p_yBegin[i], i);
                while (m_window.front().second + m_period <= i)
                    m_window.pop_front();

                p_yBegin[i] = m_window.front().first;
            }

            // apply filter delays, coefficients with the following call
            bSuccess = DigitalFilter<T>::filter(p_yBegin, p_yBegin + size, p_yBegin);
        }

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "SlidingWindowExtremum";
    }

    /**
     * Get the number of samples in the moving window
     */
    inline virtual int getPeriod(void) const final
    {
        return m_period;
    }

    /**
     * Set the number of samples in the moving window
     */
    inline virtual void setPeriod(int period) final
    {
        m_period = period;
    }

protected:

    /**
     * comparison functor
     */
    std::function<bool (const T &, const T &)> m_comparator;

    /**
     * the number of samples in the moving window
     */
    int m_period;

    /**
     * member workspace dequeue
     */
    std::deque<std::pair<T, int>> m_window;
};

}

}

}

#endif
