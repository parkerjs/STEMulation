#ifndef COMPARE_EQUAL_FUNCTOR_H
#define COMPARE_EQUAL_FUNCTOR_H

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
 * Function object class for equality comparison
 */
template<typename T, typename Result = bool>
class CompareEqualFunctor
: public ComparisonBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual CompareEqualFunctor<T, Result> *clone(void) const override
    {
        return new CompareEqualFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static CompareEqualFunctor<T, Result> *create(void)
    {
        return new CompareEqualFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs == rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Equal;
    }
};

}

}

}

}

#endif
