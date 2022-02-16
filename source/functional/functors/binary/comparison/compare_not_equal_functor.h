#ifndef COMPARE_NOT_EQUAL_FUNCTOR_H
#define COMPARE_NOT_EQUAL_FUNCTOR_H

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
 * Function object class for inequality comparison
 */
template<typename T, typename Result = bool>
class CompareNotEqualFunctor
: public ComparisonBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual CompareNotEqualFunctor<T, Result> *clone(void) const override
    {
        return new CompareNotEqualFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static CompareNotEqualFunctor<T, Result> *create(void)
    {
        return new CompareNotEqualFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs != rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::NotEqual;
    }
};

}

}

}

}

#endif
