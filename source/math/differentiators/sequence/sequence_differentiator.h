#ifndef SEQUENCE_DIFFERENTIATOR_H
#define SEQUENCE_DIFFERENTIATOR_H

namespace math
{

namespace differentiators
{

namespace sequence
{

/**
 * This class provides an abstract base for sub-classes to implement algorithms to compute numerical
 * derivatives of discrete sequences
 */
template<typename T>
class SequenceDifferentiator
{
public:

    /**
     * Constructor
     * @param n the finite difference order
     */
    SequenceDifferentiator(std::size_t n = 4)
    : m_n(n)
    {

    }

    /**
     * Copy constructor
     */
    SequenceDifferentiator(const SequenceDifferentiator<T> &differentiator)
    {
        operator = (differentiator);
    }

    /**
     * Move constructor
     */
    SequenceDifferentiator(SequenceDifferentiator<T> &&differentiator)
    {
        operator = (std::move(differentiator));
    }

    /**
     * Destructor
     */
    virtual ~SequenceDifferentiator(void)
    {

    }

    /**
     * Copy assignment operator
     */
    SequenceDifferentiator<T> &operator = (const SequenceDifferentiator<T> &differentiator)
    {
        if (&differentiator != this)
        {
            m_n = differentiator.m_n;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    SequenceDifferentiator<T> &operator = (SequenceDifferentiator<T> &&differentiator)
    {
        if (&differentiator != this)
        {
            m_n = std::move(differentiator.m_n);
        }

        return *this;
    }

    /**
     * Get the finite difference order
     */
    inline virtual std::size_t getFiniteDifferenceOrder(void) const final
    {
        return m_n;
    }

    /**
     * Set the finite difference order
     */
    inline virtual void setFiniteDifferenceOrder(std::size_t n) final
    {
        m_n = n;
    }

protected:

    /**
     * finite difference order
     */
    std::size_t m_n;
};

}

}

}

#endif
