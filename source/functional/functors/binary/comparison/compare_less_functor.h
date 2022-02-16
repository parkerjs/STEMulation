#ifndef COMPARE_LESS_FUNCTOR_H
#define COMPARE_LESS_FUNCTOR_H

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
 * Function object class for less-than comparison
 */
template<typename T, typename Result = bool>
class CompareLessFunctor
: public ComparisonBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual CompareLessFunctor<T, Result> *clone(void) const override
    {
        return new CompareLessFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static CompareLessFunctor<T, Result> *create(void)
    {
        return new CompareLessFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs < rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Less;
    }
};

}

}

}

}

#endif
