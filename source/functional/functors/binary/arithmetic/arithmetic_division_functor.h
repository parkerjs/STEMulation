#ifndef ARITHMETIC_DIVISION_FUNCTOR_H
#define ARITHMETIC_DIVISION_FUNCTOR_H

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
 * Binary function object class whose call returns the result of dividing its first argument by the second (as
 * returned by operator /)
 */
template<typename T, typename Result = T>
class DivisionFunctor
: public ArithmeticBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual DivisionFunctor<T, Result> *clone(void) const override
    {
        return new DivisionFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static DivisionFunctor<T, Result> *create(void)
    {
        return new DivisionFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs / rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Division;
    }
};

}

}

}

}

#endif
