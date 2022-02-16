#ifndef ITERATIVE_DIFFERENTIATOR_H
#define ITERATIVE_DIFFERENTIATOR_H

namespace math
{

namespace differentiators
{

/**
 * This class provides an abstract base for sub-classes that compute numerical derivatives iteratively
 */
template<typename T>
class IterativeDifferentiator
{
public:

    /**
     * Constructor
     * @param interval   the interval over which the iteration will occur
     * @param iterations the maximum number of iterations allowed to compute the derivative within the error
     *                   tolerance
     * @param tolerance  the error tolerance
     */
    IterativeDifferentiator(T interval = 1.0,
                            std::size_t iterations = 15,
                            T tolerance = 1.0e-2)
    : m_interval(interval),
      m_iterations(iterations),
      m_tolerance(tolerance)
    {

    }

    /**
     * Copy constructor
     */
    IterativeDifferentiator(const IterativeDifferentiator<T> &differentiator)
    {
        operator = (differentiator);
    }

    /**
     * Move constructor
     */
    IterativeDifferentiator(IterativeDifferentiator<T> &&differentiator)
    {
        operator = (std::move(differentiator));
    }

    /**
     * Destructor
     */
    virtual ~IterativeDifferentiator(void)
    {

    }

    /**
     * Copy assignment operator
     */
    IterativeDifferentiator<T> &operator = (const IterativeDifferentiator<T> &differentiator)
    {
        if (&differentiator != this)
        {
            m_interval = differentiator.m_interval;
            m_iterations = differentiator.m_iterations;
            m_tolerance = differentiator.m_tolerance;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    IterativeDifferentiator<T> &operator = (IterativeDifferentiator<T> &&differentiator)
    {
        if (&differentiator != this)
        {
            m_interval = std::move(differentiator.m_interval);
            m_iterations = std::move(differentiator.m_iterations);
            m_tolerance = std::move(differentiator.m_tolerance);
        }

        return *this;
    }

    /**
     * Set the interval over which the iteration occurs
     */
    inline virtual T getInterval(void) const final
    {
        return m_interval;
    }

    /**
     * Get the maximum number of iterations allowed to compute the derivative within the error tolerance
     */
    inline virtual std::size_t getMaxIterations(void) const final
    {
        return m_iterations;
    }

    /**
     * Get the error tolerance
     */
    inline virtual T getTolerance(void) const final
    {
        return m_tolerance;
    }

    /**
     * Set the interval over which the iteration occurs
     */
    inline virtual void setInterval(T interval) final
    {
        m_interval = interval;
    }

    /**
     * Set the maximum number of iterations allowed to compute the derivative within the error tolerance
     */
    inline virtual void setMaxIterations(std::size_t iterations) final
    {
        m_iterations = iterations;
    }

    /**
     * Set the error tolerance
     */
    inline virtual void setTolerance(T tolerance) final
    {
        m_tolerance = tolerance;
    }

protected:

    /**
     * the interval over which the iteration will occur
     */
    T m_interval;

    /**
     * the maximum number of iterations allowed to compute the derivative within the error tolerance
     */
    std::size_t m_iterations;

    /**
     * the error tolerance
     */
    T m_tolerance;
};

}

}

#endif
