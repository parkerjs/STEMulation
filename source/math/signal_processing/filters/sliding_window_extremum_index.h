#ifndef SLIDING_WINDOW_EXTREMUM_INDEX_H
#define SLIDING_WINDOW_EXTREMUM_INDEX_H

#include "sliding_window_extremum.h"

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * Class for computing the indices at which the minimum and maximum values occur within a trailing sliding
 * window
 */
template<typename T>
class SlidingWindowExtremumIndex
: virtual private attributes::abstract::Reflective,
  public SlidingWindowExtremum<T>
{
public:

    /**
     * Using declarations
     */
    using SlidingWindowExtremum<T>::filter;

    /**
     * Forwarding constructor
     */
    template<typename ... Args> SlidingWindowExtremumIndex(Args && ... args)
    : SlidingWindowExtremum<T>(std::forward<Args>(args) ...)
    {

    }

    /**
     * Destructor
     */
    virtual ~SlidingWindowExtremumIndex(void) override
    {

    }

    /**
     * Forwarding assignment operator
     */
    template<typename R>
    SlidingWindowExtremumIndex<T> &operator = (R &&rhs)
    {
        SlidingWindowExtremum<T>::operator = (std::forward<R>(rhs));

        return *this;
    }

    /**
     * clone() function
     */
    inline virtual SlidingWindowExtremumIndex<T> *clone(void) const override
    {
        return new SlidingWindowExtremumIndex<T>(*this);
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
        auto &&size = static_cast<int>(std::distance(p_xBegin, p_xEnd));
        bool bSuccess = (this->m_period > 0 && size > 0);
        if (bSuccess)
        {
            T lastExtreme = p_xBegin[0];
            int ageOfLastExtreme = 0;
            for (int i = 0; i < size; ++i, ++ageOfLastExtreme)
            {
                int period = std::min(i + 1, this->m_period);
                bool bExtremeIsTooOld = (ageOfLastExtreme >= period);
                if (this->m_comparator(p_xBegin[i], lastExtreme) || bExtremeIsTooOld)
                {
                    ageOfLastExtreme = 0;
                    lastExtreme = p_xBegin[i];
                    if (bExtremeIsTooOld)
                    {
                        for (int j = i - 1; j > i - period; --j)
                        {
                            if (this->m_comparator(p_xBegin[j], lastExtreme))
                            {
                                lastExtreme = p_xBegin[j];
                                ageOfLastExtreme = i - j;
                            }
                        }
                    }
                }

                p_yBegin[i] = i - ageOfLastExtreme;
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
        return "SlidingWindowExtremumIndex";
    }
};

}

}

}

#endif
