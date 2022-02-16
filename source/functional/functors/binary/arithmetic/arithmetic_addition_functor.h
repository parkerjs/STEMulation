#ifndef ARITHMETIC_ADDITION_FUNCTOR_H
#define ARITHMETIC_ADDITION_FUNCTOR_H

#include "arithmetic_binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace arithmetic
{

/**
 * Binary function object class whose call returns the result of adding its two arguments (as returned by
 * operator +)
 */
template<typename T, typename Result = T>
class AdditionFunctor
: public ArithmeticBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual AdditionFunctor<T, Result> *clone(void) const override
    {
        return new AdditionFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static AdditionFunctor<T, Result> *create(void)
    {
        return new AdditionFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs + rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Addition;
    }
};

}

}

}

}

#endif
