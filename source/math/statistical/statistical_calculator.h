#ifndef STATISTICAL_CALCULATOR_H
#define STATISTICAL_CALCULATOR_H

#include "cloneable.h"
#include "initializable.h"
#include <vector>

namespace math
{

namespace statistical
{

/**
 * Abstract base for classes that perform statistical calculations on a collection of samples
 */
template<typename Arg, typename Result = Arg>
class StatisticalCalculator
: public attributes::interfaces::Cloneable<StatisticalCalculator<Arg, Result>>,
  public attributes::interfaces::Initializable
{
public:

    /**
     * Constructor
     */
    StatisticalCalculator(void)
    : m_numSamples(0)
    {

    }

    /**
     * Copy constructor
     */
    StatisticalCalculator(const StatisticalCalculator<Arg, Result> &calculator)
    : m_numSamples(0)
    {
        operator = (calculator);
    }

    /**
     * Move constructor
     */
    StatisticalCalculator(StatisticalCalculator<Arg, Result> &&calculator)
    : m_numSamples(0)
    {
        operator = (std::move(calculator));
    }

    /**
     * Destructor
     */
    virtual ~StatisticalCalculator(void) override
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    StatisticalCalculator<Arg, Result> &operator = (const StatisticalCalculator<Arg, Result> &calculator)
    {
        if (&calculator != this)
        {
            m_numSamples = calculator.m_numSamples;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    StatisticalCalculator<Arg, Result> &operator = (StatisticalCalculator<Arg, Result> &&calculator)
    {
        if (&calculator != this)
        {
            m_numSamples = std::move(calculator.m_numSamples);
        }

        return *this;
    }

public:

    /**
     * Function to add a sample to the collection
     */
    virtual void addSample(const Arg &x) = 0;

    /**
     * Function to add samples to the collection
     */
    inline virtual void addSamples(const std::vector<Arg> &samples)
    {
        for (auto &&sample : samples)
            addSample(sample);
    }

    /**
     * Function to calculate the result
     */
    virtual Result calculate(void) const = 0;

    /**
     * Function to delete a sample from the collection. Function returns successful if there is at least one
     * sample in the collection to be removed
     */
    virtual bool deleteSample(const Arg &x) = 0;

    /**
     * Function to delete samples from the collection. Function returns successful if the number of samples to
     * be removed does not exceed the number of samples contained within the collection
     */
    inline virtual bool deleteSamples(const std::vector<Arg> &samples)
    {
        bool bSuccess = true;
        for (auto &&sample : samples)
            bSuccess &= deleteSample(sample);

        return bSuccess;
    }

    /**
     * Get the number of samples in this collection
     */
    inline virtual long getNumSamples(void) const final
    {
        return m_numSamples;
    }

protected:

    /**
     * the number of samples in the collection
     */
    long m_numSamples;
};

}

}

#endif
