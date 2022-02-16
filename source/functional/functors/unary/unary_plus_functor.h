#ifndef UNARY_PLUS_FUNCTOR_H
#define UNARY_PLUS_FUNCTOR_H

#include "unary_functor.h"

namespace functional
{

namespace functors
{

namespace unary
{

/**
 * Unary function object class whose call returns the result of applying the unary plus operator to its
 * argument (as returned by the unary operator +)
 */
template<typename T, typename Result = T>
class UnaryPlusFunctor
: public UnaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual UnaryPlusFunctor<T, Result> *clone(void) const override
    {
        return new UnaryPlusFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static UnaryPlusFunctor<T, Result> *create(void)
    {
        return new UnaryPlusFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &rhs) const override
    {
        return +rhs;
    }

    /**
     * Function to get the unary functor type
     */
    inline virtual UnaryFunctorType getType(void) const override
    {
        return UnaryFunctorType::Enum::Plus;
    }
};

}

}

}

#endif
