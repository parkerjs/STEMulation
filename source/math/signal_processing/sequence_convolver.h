#ifndef SEQUENCE_CONVOLVER_H
#define SEQUENCE_CONVOLVER_H

#include "cloneable.h"
#include "digital_filter.h"
#include "reflective.h"

namespace math
{

namespace signal_processing
{

/**
 * This class contains algorithms to perform convolution and deconvolution of two digital data sequences
 */
template<typename T>
class SequenceConvolver
: public attributes::interfaces::Cloneable<SequenceConvolver<T>>,
  virtual private attributes::abstract::Reflective,
  public attributes::concrete::StaticMutexMappable<SequenceConvolver<T>, int, std::mutex *>,
  public attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>
{
public:

    /**
     * Constructor
     */
    SequenceConvolver(void)
    : m_pFilter(new filters::DigitalFilter<T>())
    {

    }

    /**
     * Copy constructor
     */
    SequenceConvolver(const SequenceConvolver<T> &sequenceConvolver)
    : m_pFilter(nullptr)
    {
        operator = (sequenceConvolver);
    }

    /**
     * Move constructor
     */
    SequenceConvolver(SequenceConvolver<T> &&sequenceConvolver)
    : m_pFilter(nullptr)
    {
        operator = (std::move(sequenceConvolver));
    }

    /**
     * Destructor
     */
    virtual ~SequenceConvolver(void) override
    {
        if (m_pFilter != nullptr)
            delete m_pFilter;
    }

    /**
     * Copy assignment operator
     */
    SequenceConvolver<T> &operator = (const SequenceConvolver<T> &sequenceConvolver)
    {
        if (&sequenceConvolver != this)
        {
            if (m_pFilter != nullptr)
            {
                delete m_pFilter;
                m_pFilter = nullptr;
            }

            if (sequenceConvolver.m_pFilter != nullptr)
                m_pFilter = sequenceConvolver.m_pFilter->clone();

            // don't copy temporary remainder and result workspace vectors
//          m_remainder = sequenceConvolver.m_remainder;
//          m_result = sequenceConvolver.m_result;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    SequenceConvolver<T> &operator = (SequenceConvolver<T> &&sequenceConvolver)
    {
        if (&sequenceConvolver != this)
        {
            if (m_pFilter != nullptr)
            {
                delete m_pFilter;
                m_pFilter = nullptr;
            }

            m_pFilter = std::move(sequenceConvolver.m_pFilter);
            sequenceConvolver.m_pFilter = nullptr;

            // don't move temporary remainder and result workspace vectors
//          m_remainder = std::move(sequenceConvolver.m_remainder);
//          m_result = std::move(sequenceConvolver.m_result);
        }

        return *this;
    }

    /**
     * clone() function
     */
    virtual SequenceConvolver<T> *clone(void) const override
    {
        return new SequenceConvolver<T>(*this);
    }

    /**
     * Function to perform convolution of two digital data sequences; the function return value contains the
     * result of the convolution
     * @param itLeftBegin  an iterator (or pointer) to the beginning of the left data sequence
     * @param itLeftEnd    an iterator (or pointer) to the element following the end of the range defining the
     *                     left sequence (if there are N elements [0 ... N-1], the iterator (or pointer) refers
     *                     to the element that would follow element N-1)
     * @param itRightBegin an iterator (or pointer) to the beginning of the right data sequence
     * @param itRightEnd   an iterator (or pointer) to the element following the end of the range defining the
     *                     right sequence (if there are M elements [0 ... M-1], the iterator (or pointer)
     *                     refers to the element that would follow element M-1)
     */
    template<typename left_iterator, typename right_iterator>
    std::vector<T> convolve(left_iterator &&itLeftBegin, left_iterator &&itLeftEnd,
                            right_iterator &&itRightBegin, right_iterator &&itRightEnd)
    {
        std::vector<T> result;
        bool bSuccess = convolve(std::forward<left_iterator>(itLeftBegin),
                                 std::forward<left_iterator>(itLeftEnd),
                                 std::forward<right_iterator>(itRightBegin),
                                 std::forward<right_iterator>(itRightEnd),
                                 result);
        if (!bSuccess)
        {
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Convolution failed." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::unlock(0);
        }

        return result;
    }

    /**
     * Function to perform convolution of two digital data sequences
     * @param itLeftBegin  an iterator (or pointer) to the beginning of the left data sequence
     * @param itLeftEnd    an iterator (or pointer) to the element following the end of the range defining the
     *                     left sequence (if there are N elements [0 ... N-1], the iterator (or pointer) refers
     *                     to the element that would follow element N-1)
     * @param itRightBegin an iterator (or pointer) to the beginning of the right data sequence
     * @param itRightEnd   an iterator (or pointer) to the element following the end of the range defining the
     *                     right sequence (if there are M elements [0 ... M-1], the iterator (or pointer)
     *                     refers to the element that would follow element M-1)
     * @param result       upon success, is a vector that will be populated with the result of the convolution
     */
    template<typename left_iterator, typename right_iterator>
    bool convolve(left_iterator &&itLeftBegin, left_iterator &&itLeftEnd, right_iterator &&itRightBegin,
                  right_iterator &&itRightEnd, std::vector<T> &result)
    {
        auto &&sizeLeft = std::distance(itLeftBegin, itLeftEnd);
        auto &&sizeRight = std::distance(itRightBegin, itRightEnd);
        bool bSuccess = (sizeLeft > 0 && sizeRight > 0);
        if (bSuccess)
        {
            auto *pResult = &result;
            if (!result.empty() && (&result[0] == &itLeftBegin[0] || &result[0] == &itRightBegin[0]))
                pResult = &m_result;

            pResult->resize(sizeLeft + sizeRight - 1);

            bSuccess = convolve(std::forward<left_iterator>(itLeftBegin),
                                std::forward<left_iterator>(itLeftEnd),
                                std::forward<right_iterator>(itRightBegin),
                                std::forward<right_iterator>(itRightEnd),
                                pResult->begin());

            if (pResult != &result)
                result = *pResult;
        }
        else
        {
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Sequences must be non-empty." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::unlock(0);
        }

        return bSuccess;
    }

    /**
     * Function to perform convolution of two digital data sequences
     * @param itLeftBegin   an iterator (or pointer) to the beginning of the left data sequence
     * @param itLeftEnd     an iterator (or pointer) to the element following the end of the range defining the
     *                      left sequence (if there are N elements [0 ... N-1], the iterator (or pointer)
     *                      refers to the element that would follow element N-1)
     * @param itRightBegin  an iterator (or pointer) to the beginning of the right data sequence
     * @param itRightEnd    an iterator (or pointer) to the element following the end of the range defining the
     *                      right sequence (if there are M elements [0 ... M-1], the iterator (or pointer)
     *                      refers to the element that would follow element M-1)
     * @param itResultBegin an iterator (or pointer) to the beginning of the vector that will store the result
     *                      of the convolution operation, the size of which should be N+M-1, where N is the
     *                      size of the left sequence and M is the size of the right sequence
     */
    template<typename left_iterator, typename right_iterator, typename result_iterator>
    typename std::enable_if<!std::is_pointer<left_iterator>::value ||
                            !std::is_pointer<right_iterator>::value ||
                            !std::is_pointer<result_iterator>::value, bool>::type
    convolve(left_iterator &&itLeftBegin, left_iterator &&itLeftEnd, right_iterator &&itRightBegin,
             right_iterator &&itRightEnd, result_iterator &&itResult)
    {
        auto &&sizeLeft = std::distance(itLeftBegin, itLeftEnd);
        auto &&sizeRight = std::distance(itRightBegin, itRightEnd);
        bool bSuccess = (sizeLeft > 0 && sizeRight > 0);
        if (bSuccess)
        {
            const T *pLeftBegin = nullptr, *pLeftEnd = nullptr;
            if (sizeLeft > 0)
            {
                pLeftBegin = &itLeftBegin[0];
                pLeftEnd = pLeftBegin + sizeLeft;
            }

            const T *pRightBegin = nullptr, *pRightEnd = nullptr;
            if (sizeRight > 0)
            {
                pRightBegin = &itRightBegin[0];
                pRightEnd = pRightBegin + sizeRight;
            }

            bSuccess = convolve(pLeftBegin, pLeftEnd, pRightBegin, pRightEnd, &itResult[0]);
        }
        else
        {
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Sequences must be non-empty." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::unlock(0);
        }

        return bSuccess;
    }

    /**
     * Function to perform convolution of two digital data sequences
     * @param  x the left data sequence
     * @param  h the right data sequence
     * @return   the result of the convolution
     */
    virtual std::vector<T> convolve(const std::vector<T> &x, const std::vector<T> &h) final
    {
        auto &&hSize = h.size();
        auto &&xSize = x.size();
        std::vector<T> y;
        if (hSize > 0 && xSize > 0)
        {
            y.resize(hSize + xSize - 1, 0.0);

            convolve(x, h, y);
        }

        return y;
    }

    /**
     * Function to perform convolution of two digital data sequences
     * @param      x the left data sequence
     * @param      h the right data sequence
     * @param[out] y the result of the convolution
     * @return       true upon success
     */
    inline virtual bool convolve(const std::vector<T> &x, const std::vector<T> &h, std::vector<T> &y) final
    {
        return convolve(x.begin(), x.end(), h.begin(), h.end(), y.begin());
    }

    /**
     * Function to perform convolution of two digital data sequences
     * @param pLeftBegin   a pointer to the beginning of the range defining the left sequence
     * @param pLeftEnd     a pointer to the element following the end of the range defining the left sequence
     *                     (if there are N elements [0 ... N-1], the pointer refers to the element that would
     *                     follow element N-1)
     * @param pRightBegin  a pointer to the beginning of the range defining the right sequence
     * @param pRightEnd    a pointer to the element following the end of the range defining the right sequence
     *                     (if there are M elements [0 ... M-1], the pointer refers to the element that would
     *                     follow element M-1)
     * @param pResultBegin a pointer to the beginning of the vector that will hold the result of the
     *                     convolution
     */
    bool convolve(const T *pLeftBegin, const T *pLeftEnd, const T *pRightBegin, const T *pRightEnd,
                  T *pResultBegin)
    {
        bool bSuccess = (pLeftBegin != nullptr && pLeftEnd != nullptr && pRightBegin != nullptr &&
                         pRightEnd != nullptr && pResultBegin != nullptr);
        if (bSuccess)
        {
            auto &&sizeLeft = (size_t)std::distance(pLeftBegin, pLeftEnd);
            auto &&sizeRight = (size_t)std::distance(pRightBegin, pRightEnd);
            for (size_t i = 0; i < sizeLeft; ++i)
                for (size_t j = 0; j < sizeRight; ++j)
                    pResultBegin[i + j] += pRightBegin[j] * pLeftBegin[i];
        }
        else
        {
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "At least one input argument is null." << std::endl << std::endl;
            attributes::concrete::StaticSynchronizable<SequenceConvolver<T>>::unlock(0);
        }

        return bSuccess;
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the left data
     * sequence out of the right sequence using long division; the function return value contains the result of
     * the deconvolution. The remainder, if any, is discarded.
     * @param itLeftBegin  an iterator (or pointer) to the beginning of the left data sequence
     * @param itLeftEnd    an iterator (or pointer) to the element following the end of the range defining the
     *                     left sequence (if there are N elements [0 ... N-1], the iterator (or pointer) refers
     *                     to the element that would follow element N-1)
     * @param itRightBegin an iterator (or pointer) to the beginning of the right data sequence
     * @param itRightEnd   an iterator (or pointer) to the element following the end of the range defining the
     *                     right sequence (if there are M elements [0 ... M-1], the iterator (or pointer)
     *                     refers to the element that would follow element M-1)
     */
    template<typename left_iterator, typename right_iterator>
    std::vector<T> deconvolve(left_iterator &&itLeftBegin, left_iterator &&itLeftEnd,
                              right_iterator &&itRightBegin, right_iterator &&itRightEnd)
    {
        std::vector<T> remainder, result;
        bool bSuccess = deconvolve(std::forward<left_iterator>(itLeftBegin),
                                   std::forward<left_iterator>(itLeftEnd),
                                   std::forward<right_iterator>(itRightBegin),
                                   std::forward<right_iterator>(itRightEnd),
                                   result, remainder);
        if (!bSuccess)
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "Deconvolution failed." << std::endl << std::endl;
            this->unlock(0);
        }

        return result;
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the left data
     * sequence out of the right sequence using long division.
     * @param itLeftBegin  an iterator (or pointer) to the beginning of the left data sequence
     * @param itLeftEnd    an iterator (or pointer) to the element following the end of the range defining the
     *                     left sequence (if there are N elements [0 ... N-1], the iterator (or pointer) refers
     *                     to the element that would follow element N-1)
     * @param itRightBegin an iterator (or pointer) to the beginning of the right data sequence
     * @param itRightEnd   an iterator (or pointer) to the element following the end of the range defining the
     *                     right sequence (if there are M elements [0 ... M-1], the iterator (or pointer)
     *                     refers to the element that would follow element M-1)
     * @param result       upon success, is a vector that will be populated with the result of the
     *                     deconvolution
     * @param remainder    upon success, is a vector that will be populated with any remainder that results
     *                     from the long division
     */
    template<typename left_iterator, typename right_iterator>
    bool deconvolve(left_iterator &&itLeftBegin, left_iterator &&itLeftEnd, right_iterator &&itRightBegin,
                    right_iterator &&itRightEnd, std::vector<T> &result, std::vector<T> &remainder)
    {
        auto &&sizeLeft = std::distance(itLeftBegin, itLeftEnd);
        auto &&sizeRight = std::distance(itRightBegin, itRightEnd);
        bool bSuccess = (sizeLeft > 0 && sizeRight > 0);
        if (bSuccess)
        {
            auto *pRemainder = &remainder;
            if (!remainder.empty() && (&remainder[0] == &itLeftBegin[0] || &remainder[0] == &itRightBegin[0]))
                pRemainder = &m_remainder;

            auto *pResult = &result;
            if (!result.empty() && (&result[0] == &itLeftBegin[0] || &result[0] == &itRightBegin[0]))
                pResult = &m_result;

            if (sizeLeft > sizeRight)
            {
                pRemainder->resize(sizeRight);
                pResult->resize(1);
            }
            else
            {
                pRemainder->resize(sizeLeft - 1);
                pResult->resize(1 + sizeRight - sizeLeft);
            }

            bSuccess = deconvolve(std::forward<left_iterator>(itLeftBegin),
                                  std::forward<left_iterator>(itLeftEnd),
                                  std::forward<right_iterator>(itRightBegin),
                                  std::forward<right_iterator>(itRightEnd),
                                  pResult->begin(), pRemainder->begin());

            if (pRemainder != &remainder)
                remainder = *pRemainder;

            if (pResult != &result)
                result = *pResult;
        }
        else
        {
            this->lock(0);
            std::cout << "Warning from " << getQualifiedMethodName(__func__) << ": "
                      << "Sequences must be non-empty." << std::endl << std::endl;
            this->unlock(0);
        }

        return bSuccess;
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the left data
     * sequence out of the right sequence using long division.
     * @param itLeftBegin      an iterator (or pointer) to the beginning of the left data sequence
     * @param itLeftEnd        an iterator (or pointer) to the element following the end of the range defining
     *                         the left sequence (if there are N elements [0 ... N-1], the iterator (or
     *                         pointer) refers to the element that would follow element N-1)
     * @param itRightBegin     an iterator (or pointer) to the beginning of the right data sequence
     * @param itRightEnd       an iterator (or pointer) to the element following the end of the range defining
     *                         the right sequence (if there are M elements [0 ... M-1], the iterator (or
     *                         pointer) refers to the element that would follow element M-1)
     * @param itResultBegin    an iterator (or pointer) to the beginning of the vector that will store the
     *                         result of the deconvolution operation, the size of which should be 1+M-N, where
     *                         N is the size of the left sequence and M is the size of the right sequence. If
     *                         the size of the left sequence is greater than the right sequence, then the size
     *                         of the result vector should be 1.
     * @param itRemainderBegin an iterator (or pointer) to the beginning of the vector that will store the
     *                         remainder resulting from the long division, the size of which should be N-1,
     *                         where N is the size of the left sequence. If the size of the left sequence is
     *                         greater than the right sequence, then the size of the remainder vector should be
     *                         M, where M is the size of the right sequence.
     */
    template<typename left_iterator, typename right_iterator, typename result_iterator,
             typename remainder_iterator>
    typename std::enable_if<!std::is_pointer<left_iterator>::value ||
                            !std::is_pointer<right_iterator>::value ||
                            !std::is_pointer<result_iterator>::value ||
                            !std::is_pointer<remainder_iterator>::value, bool>::type
    deconvolve(left_iterator &&itLeftBegin, left_iterator &&itLeftEnd, right_iterator &&itRightBegin,
               right_iterator &&itRightEnd, result_iterator &&itResult, remainder_iterator &&itRemainder)
    {
        const T *pRightBegin = nullptr, *pRightEnd = nullptr;
        auto &&sizeRight = std::distance(itRightBegin, itRightEnd);
        if (sizeRight > 0)
        {
            pRightBegin = &itRightBegin[0];
            pRightEnd = pRightBegin + sizeRight;
        }

        const T *pLeftBegin = nullptr, *pLeftEnd = nullptr;
        auto &&sizeLeft = std::distance(itLeftBegin, itLeftEnd);
        if (sizeLeft > 0)
        {
            pLeftBegin = &itLeftBegin[0];
            pLeftEnd = pLeftBegin + sizeLeft;
        }

        T *pRemainderBegin = nullptr;
        if (sizeLeft > 1)
            pRemainderBegin = &itRemainder[0];

        return deconvolve(pLeftBegin, pLeftEnd, pRightBegin, pRightEnd, &itResult[0], pRemainderBegin);
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the x data
     * sequence out of the h data sequence using long division and returns the result via the return argument
     * @param x the left data sequence
     * @param h the right data sequence
     */
    inline virtual std::vector<T> deconvolve(const std::vector<T> &x, const std::vector<T> &h) final
    {
        return deconvolve(x.begin(), x.end(), h.begin(), h.end());
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the x data
     * sequence out of the h data sequence using long division.
     * @param      x the left data sequence
     * @param      h the right data sequence
     * @param[out] q the result of the deconvolution
     */
    inline virtual bool deconvolve(const std::vector<T> &x, const std::vector<T> &h, std::vector<T> &q) final
    {
        std::vector<T> r;

        return deconvolve(x.begin(), x.end(), h.begin(), h.end(), q, r);
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the x data
     * sequence out of the h data sequence using long division.
     * @param      x the left data sequence
     * @param      h the right data sequence
     * @param[out] q the result of the deconvolution
     */
    inline virtual bool deconvolve(const std::vector<T> &x, const std::vector<T> &h, std::vector<T> &q,
                                   std::vector<T> &r) final
    {
        return deconvolve(x.begin(), x.end(), h.begin(), h.end(), q, r);
    }

    /**
     * Function to perform deconvolution of two digital data sequences; the function deconvolves the left data
     * sequence out of the right sequence using long division.
     * @param pLeftBegin      a pointer to the beginning of the range defining the left sequence
     * @param pLeftEnd        a pointer to the element following the end of the range defining the left
     *                        sequence (if there are N elements [0 ... N-1], the pointer refers to the element
     *                        that would follow element N-1)
     * @param pRightBegin     a pointer to the beginning of the range defining the right sequence
     * @param pRightEnd       a pointer to the element following the end of the range defining the right
     *                        sequence (if there are M elements [0 ... M-1], the pointer refers to the element
     *                        that would follow element M-1)
     * @param pResultBegin    a pointer to the beginning of the vector that will hold the result of the
     *                        deconvolution
     * @param pRemainderBegin a pointer to the beginning of the vector that will hold the remainder resulting
     *                        from the long division
     */
    virtual bool deconvolve(const T *pLeftBegin, const T *pLeftEnd, const T *pRightBegin, const T *pRightEnd,
                            T *pResultBegin, T *pRemainderBegin) final
    {
        bool bSuccess = (pLeftBegin != nullptr && pLeftEnd != nullptr && pRightBegin != nullptr &&
                         pRightEnd != nullptr && pResultBegin != nullptr);
        if (bSuccess)
        {
            auto &&sizeLeft = std::distance(pLeftBegin, pLeftEnd);
            auto &&sizeRight = std::distance(pRightBegin, pRightEnd);
            if (sizeLeft > sizeRight)
            {
                pResultBegin[0] = 0.0;
                std::copy(pRightBegin, pRightEnd, pRemainderBegin);
            }
            else if (m_pFilter != nullptr)
            {
                // perform deconvolution via digital filter impulse response
                m_pFilter->setCoefficients(pLeftBegin, pLeftEnd, pRightBegin, pRightEnd);
                m_pFilter->clearDelays();

                // perform deconvolution via digital filter impulse response
                auto &&size = 1 + sizeRight - sizeLeft;
                std::fill(pResultBegin + 1, pResultBegin + size, 0.0);
                pResultBegin[0] = 1.0;

                bSuccess = m_pFilter->filter(pResultBegin, pResultBegin + size, pResultBegin);
                if (bSuccess)
                {
                    if (sizeLeft > 1)
                    {
                        bSuccess = (pRemainderBegin != nullptr);
                        if (bSuccess)
                        {
                            auto &&delays = m_pFilter->getDelays();
                            for (int i = 0; i + 1 < sizeLeft; ++i)
                                pRemainderBegin[i] = pLeftBegin[0] * delays[i];
                        }
                    }
                }
                else
                {
                    this->lock(0);
                    std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                              << "Failed to calculate impulse response." << std::endl << std::endl;
                    this->unlock(0);
                }
            }
        }
        else
        {
            this->lock(0);
            std::cout << "Warning from " + getQualifiedMethodName(__func__) + ": "
                      << "At least one input argument is null." << std::endl << std::endl;
            this->unlock(0);
        }

        return bSuccess;
    }

    /**
     * Get the name of this class
     */
    inline virtual std::string getClassName(void) const override
    {
        return "SequenceConvolver";
    }

private:

    /**
     * pointer to a digital filter object
     */
    signal_processing::filters::DigitalFilter<T> *m_pFilter;

    /**
     * temporary workspace vector to hold result of convolution/deconvolution
     */
    std::vector<T> m_result;

    /**
     * temporary workspace vector to hold any remainder that results from polynomial division
     */
    std::vector<T> m_remainder;
};

}

}

#endif
