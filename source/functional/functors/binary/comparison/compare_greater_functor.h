#ifndef COMPARE_GREATER_FUNCTOR_H
#define COMPARE_GREATER_FUNCTOR_H

#include "comparison_binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace comparison
{

/**
 * Function object class for greater-than comparison
 */
template<typename T, typename Result = bool>
class CompareGreaterFunctor
: public ComparisonBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual CompareGreaterFunctor<T, Result> *clone(void) const override
    {
        return new CompareGreaterFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static CompareGreaterFunctor<T, Result> *create(void)
    {
        return new CompareGreaterFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs > rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Greater;
    }
};

}

}

}

}

#endif
