#ifndef ARITHMETIC_MULTIPLICATION_FUNCTOR_H
#define ARITHMETIC_MULTIPLICATION_FUNCTOR_H

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
 * Binary function object class whose call returns the result of multiplying its two arguments (as returned by
 * operator *)
 */
template<typename T, typename Result = T>
class MultiplicationFunctor
: public ArithmeticBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual MultiplicationFunctor<T, Result> *clone(void) const override
    {
        return new MultiplicationFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static MultiplicationFunctor<T, Result> *create(void)
    {
        return new MultiplicationFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs * rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Multiplication;
    }
};

}

}

}

}

#endif
