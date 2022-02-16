#ifndef UNARY_MINUS_FUNCTOR_H
#define UNARY_MINUS_FUNCTOR_H

#include "unary_functor.h"

namespace functional
{

namespace functors
{

namespace unary
{

/**
 * Unary function object class whose call returns the result of negating its argument (as returned by the unary
 * operator -)
 */
template<typename T, typename Result = T>
class UnaryMinusFunctor
: public UnaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual UnaryMinusFunctor<T, Result> *clone(void) const override
    {
        return new UnaryMinusFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static UnaryMinusFunctor<T, Result> *create(void)
    {
        return new UnaryMinusFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &rhs) const override
    {
        return -rhs;
    }

    /**
     * Function to get the unary functor type
     */
    inline virtual UnaryFunctorType getType(void) const override
    {
        return UnaryFunctorType::Enum::Minus;
    }
};

}

}

}

#endif
