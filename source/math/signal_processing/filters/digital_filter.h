#ifndef FILTER_H
#define FILTER_H

#include "cloneable.h"
#include "reflective.h"
#include "static_mutex_mappable.h"
#include "static_synchronizable.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <vector>

namespace math
{

namespace signal_processing
{

namespace filters
{

/**
 * This class implements algorithms to perform infinite impulse response (IIR) or finite impulse response (FIR)
 * digital filtering
 */
template<typename T>
class DigitalFilter
: public attributes::interfaces::Cloneable<DigitalFilter<T>>,
  virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticMutexMappable<DigitalFilter<T>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<DigitalFilter<T>>
{
public:

    /**
     * Constructor
     */
    DigitalFilter(void)
    : m_denCoeffs({ static_cast<T>(0.0) }),
      m_numCoeffs({ static_cast<T>(1.0) })
    {

    }

    /**
     * Constructor
     * @param a, b are the coefficient vectors that will be used to construct the filter such that
       \verbatim
          a[0] * y[n - 1] = b[0] * x[n - 1] + b[1] * x[n - 2] + ... + b[nb] * x[n - nb - 1]
                          - a[1] * y[n - 2] - ... - a[na] * y[n - na - 1]
       \endverbatim
     *
     */
    DigitalFilter(const std::vector<T> &a, const std::vector<T> &b)
    {
        setCoefficients(a, b);
    }

    /**
     * Copy constructor
     */
    DigitalFilter(const DigitalFilter<T> &filter)
    {
        operator = (filter);
    }

    /**
     * Move constructor
     */
    DigitalFilter(DigitalFilter<T> &&filter)
    {
        operator = (std::move(filter));
    }

    /**
     * Destructor
     */
    virtual ~DigitalFilter(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    DigitalFilter<T> &operator = (const DigitalFilter<T> &filter)
    {
        if (&filter != this)
        {
            m_delays = filter.m_delays;
            m_denCoeffs = filter.m_denCoeffs;
            m_numCoeffs = filter.m_numCoeffs;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    DigitalFilter<T> &operator = (DigitalFilter<T> &&filter)
    {
        if (&filter != this)
        {
            m_delays = std::move(filter.m_delays);
            m_denCoeffs = std::move(filter.m_denCoeffs);
            m_numCoeffs = std::move(filter.m_numCoeffs);
        }

        return *this;
    }

    /**
     * Function to clear this object's filter delays
     */
    inline virtual void clearDelays(void) final
    {
        std::fill(m_delays.begin(), m_delays.end(), 0.0);
    }

    /**
     * clone() function
     */
    inline virtual DigitalFilter<T> *clone(void) const override
    {
        return new DigitalFilter<T>(*this);
    }

    /**
     * Digital signal filtering function (see description in overloads that follow)
     * @param it_xBegin an iterator (or pointer) to the beginning of the input data sequence to be filtered
     * @param it_xEnd   a pointer to the element following the end of the range defining the input data
     *                  sequence (if there are N elements [0 ... N-1], the iterator (or pointer) refers to the
     *                  element that would follow element N-1)
     * @param it_yBegin an iterator (or pointer) to the beginning of a range that will store the values arising
     *                  from the filtration process
     */
    template<typename input_iterator, typename output_iterator> inline typename
    std::enable_if<!std::is_pointer<input_iterator>::value || !std::is_pointer<output_iterator>::value, bool>::type
    filter(input_iterator it_xBegin, input_iterator it_xEnd, output_iterator it_yBegin)
    {
        return filter(&it_xBegin[0], &it_xBegin[0] + std::distance(it_xBegin, it_xEnd), &it_yBegin[0]);
    }

    /**
     * Digital signal filtering function (see description in overloads that follow)
     * @param[in]  x a vector containing the input signal
     * @param[out] y a vector containing the filtered output signal (will be resized if necessary)
     */
    inline virtual bool filter(const std::vector<T> &x, std::vector<T> &y)
    {
        auto &&size = x.size();
        if (size != y.size())
            y.resize(size);

        return filter(&x.begin()[0], &x.begin()[0] + size, &y.begin()[0]);
    }

    /**
     * Digital signal filtering function. Returns the solution to the following linear, time-invariant
     * difference equation:
       \verbatim
         N                       M
        ---                     ---
        \   a(k + 1) y(n - k) + \   b(k + 1) x(n - k) = 0 for 1 <= n <= length(x)
        /                       /
        ---                     ---
       k = 0                   k = 0

       where N = length(a) - 1 and M = length(b) - 1. An equivalent form of this equation is:

                N                       M
               ---                     ---
       y(n) =  \   c(k + 1) y(n - k) + \   d(k + 1) x(n - k) for 1 <= n <= length(x)
               /                       /
               ---                     ---
              k = 1                   k = 0

       where c = a / a(1) and d = b / a(1).

       In terms of the z-transform, y is the result of passing the discrete-time signal x through a system
       characterized by the following rational system function:

                     M
                    ---
                    \   d(k + 1) z^(-k)
                    /
                    ---
                   k = 0
         H(z) = -----------------------
                     N
                    ---
                1 + \   c(k + 1) z^(-k)
                    /
                    ---
                   k = 1
       \endverbatim
     *
     * The particular algorithm employed is known as a transposed Direct Form II implementation. Original
     * implementation in Matlab written by Tony Richardson <amr@mpl.ucsd.edu> June 1994
     * @param p_xBegin a pointer to the beginning of the input data sequence to be filtered
     * @param p_xEnd   a pointer to the element following the end of the range defining the input data sequence
     *                 (if there are N elements [0 ... N-1], the pointer refers to the element that would
     *                 follow element N-1)
     * @param p_yBegin a pointer to the beginning of a range that will store the values arising from the
     *                 filtration process
     */
    virtual bool filter(const T *p_xBegin, const T *p_xEnd, T *p_yBegin)
    {
        auto &&n = m_denCoeffs.size();
        bool bSuccess = (n != 0 && n == m_numCoeffs.size());
        if (bSuccess)
        {
            auto &&p = (size_t)std::distance(p_xBegin, p_xEnd);
            auto &&q = m_delays.size();

            // perform digital filtering of the input signal
            for (size_t i = 0; i < p; ++i)
            {
                auto xi = p_xBegin[i];
                p_yBegin[i] = m_numCoeffs[0] * xi;
                if (q > 0)
                {
                    p_yBegin[i] += m_delays[0];
                    for (size_t j = 1; j < q; ++j)
                        m_delays[j - 1] = m_delays[j] + m_numCoeffs[j] * xi - m_denCoeffs[j] * p_yBegin[i];

                    m_delays[q - 1] = m_numCoeffs[q] * xi - m_denCoeffs[q] * p_yBegin[i];
                }
            }
        }
        else
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "Invalid filter numerator and/or denominator coefficients."
                      << std::endl << std::endl;
            this->unlock(0);
        }

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "DigitalFilter";
    }

    /**
     * Get filter numerator and denominator coefficients
     * @param[out] a populated with a vector of numerator coefficients
     * @param[out] b populated with a vector of denominator coefficients
     */
    inline virtual void getCoefficients(std::vector<T> &a, std::vector<T> &b) const final
    {
        b.resize(m_numCoeffs.size());
        a.resize(m_denCoeffs.size());

        std::copy(m_denCoeffs.cbegin(), m_denCoeffs.cend(), a.begin());
        std::copy(m_numCoeffs.cbegin(), m_numCoeffs.cend(), b.begin());
    }

    /**
     * Get current state of filter delays
     * @param[out] delays populated with a vector of filter delays
     */
    inline virtual void getDelays(std::vector<T> &delays) final
    {
        delays.resize(m_delays.size());

        std::copy(m_delays.cbegin(), m_delays.cend(), delays.begin());
    }

    /**
     * Get current state of filter delays
     */
    inline virtual const std::vector<T> &getDelays(void) const final
    {
        return m_delays;
    }

    /**
     * Get order of the digital filter
     */
    inline virtual long getOrder(void) const final
    {
        return long(m_numCoeffs.size()) - 1;
    }

    /**
     * Set filter numerator and denominator coefficients
     * @param p_aBegin a pointer to the beginning of a range of numerator coefficients bounded by [p_aBegin,
     *                 p_aEnd)
     * @param p_aEnd   a pointer to the end of a range of numerator coefficients bounded by [p_aBegin, p_aEnd)
     * @param p_bBegin a pointer to the beginning of a range of denominator coefficients bounded by [p_bBegin,
     *                 p_bEnd)
     * @param p_bEnd   a pointer to the end of a range of denominator coefficients bounded by [p_bBegin,
     *                 p_bEnd)
     */
    virtual bool setCoefficients(const T *p_aBegin, const T *p_aEnd, const T *p_bBegin, const T *p_bEnd) final
    {
        m_denCoeffs.resize(std::distance(p_aBegin, p_aEnd));
        std::copy(p_aBegin, p_aEnd, m_denCoeffs.begin());

        m_numCoeffs.resize(std::distance(p_bBegin, p_bEnd));
        std::copy(p_bBegin, p_bEnd, m_numCoeffs.begin());

        return setCoefficients(std::move(m_denCoeffs), std::move(m_numCoeffs));
    }

    /**
     * Set filter numerator and denominator coefficients
     * @param a a vector of numerator coefficients
     * @param b a vector of denominator coefficients
     */
    virtual bool setCoefficients(std::vector<T> &&a, std::vector<T> &&b) final
    {
        if (&a != &m_denCoeffs)
            m_denCoeffs = std::move(a);

        if (&b != &m_numCoeffs)
            m_numCoeffs = std::move(b);

        auto &&order = getOrder();
        if (order != static_cast<long>(m_delays.size()))
            m_delays.resize(order);

        auto norm = m_denCoeffs[0];
        bool bSuccess = false;
        if (norm == 0.0)
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "First element in denominator polynomial must be non-zero."
                      << std::endl << std::endl;
            this->unlock(0);
        }
        else
        {
            auto size = std::max(m_denCoeffs.size(), m_numCoeffs.size());

            // it's convenient to pad the coefficient vectors to the same length
            m_denCoeffs.resize(size, 0.0);
            m_numCoeffs.resize(size, 0.0);
            m_delays.resize(size - 1, 0.0);

            // normalize the coefficient vectors
            if (norm != 1.0)
            {

                auto divides = [norm] (const T &x) { return x / norm; };
                std::transform(m_denCoeffs.begin(), m_denCoeffs.end(), m_denCoeffs.begin(), divides);
                std::transform(m_numCoeffs.begin(), m_numCoeffs.end(), m_numCoeffs.begin(), divides);
            }

            bSuccess = true;
        }

        return bSuccess;
    }

    /**
     * Set filter numerator and denominator coefficients
     * @param a a vector of numerator coefficients
     * @param b a vector of denominator coefficients
     */
    inline virtual bool setCoefficients(const std::vector<T> &a, const std::vector<T> &b) final
    {
        return setCoefficients(std::vector<T>(a), std::vector<T>(b));
    }

    /**
     * Set initial filter delays
     * @param delays a vector of filter delays
     */
    virtual bool setDelays(std::vector<T> &&delays) final
    {
        if (!delays.empty()) // initial condition was not specified
        {
            if (static_cast<long>(delays.size()) != getOrder())
            {
                this->lock(0);
                std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                          << "Size of filter delay vector should be one less than "
                          << "the maximum length of the numerator and denominator "
                          << "coefficient vectors." << std::endl << std::endl;
                this->unlock(0);

                std::fill(m_delays.begin(), m_delays.end(), 0.0);

                return false;
            }
            else if (&delays != &m_delays) // set the filter delays using the specified initial condition
                std::copy(delays.cbegin(), delays.cend(), m_delays.begin());
        }

        return true;
    }

    /**
     * Set initial filter delays
     * @param delays a vector of filter delays
     */
    inline virtual bool setDelays(const std::vector<T> &delays) final
    {
        return setDelays(std::vector<T>(delays));
    }

protected:

    /**
     * vector of filter delays
     */
    std::vector<T> m_delays;

    /**
     * denominator coefficients
     */
    std::vector<T> m_denCoeffs;

    /**
     * numerator coefficients
     */
    std::vector<T> m_numCoeffs;
};

}

}

}

#endif
