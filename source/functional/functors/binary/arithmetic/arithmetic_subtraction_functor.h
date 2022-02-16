#ifndef ARITHMETIC_SUBTRACTION_FUNCTOR_H
#define ARITHMETIC_SUBTRACTION_FUNCTOR_H

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
 * Binary function object class whose call returns the result of subtracting its second argument from its first
 * argument (as returned by the binary operator -)
 */
template<typename T, typename Result = T>
class SubtractionFunctor
: public ArithmeticBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual SubtractionFunctor<T, Result> *clone(void) const override
    {
        return new SubtractionFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static SubtractionFunctor<T, Result> *create(void)
    {
        return new SubtractionFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs - rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Subtraction;
    }
};

}

}

}

}

#endif
