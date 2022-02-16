#ifndef LOGICAL_NOT_FUNCTOR_H
#define LOGICAL_NOT_FUNCTOR_H

#include "unary_functor.h"

namespace functional
{

namespace functors
{

namespace unary
{

/**
 * Unary function object class whose call returns the result of applying the logical
 *                           "not" operation to its right-hand argument (as returned by the unary operator !)
 */
template<typename T, typename Result = T>
class LogicalNotFunctor
: public UnaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual LogicalNotFunctor<T, Result> *clone(void) const override
    {
        return new LogicalNotFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static LogicalNotFunctor<T, Result> *create(void)
    {
        return new LogicalNotFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &rhs) const override
    {
        return !rhs;
    }

    /**
     * Function to get the unary functor type
     */
    inline virtual UnaryFunctorType getType(void) const override
    {
        return UnaryFunctorType::Enum::LogicalNot;
    }

    /**
     * Function to determine whether or not the current object is a logical operation
     */
    inline virtual bool isLogical(void) const override
    {
        return true;
    }
};

}

}

}

#endif
