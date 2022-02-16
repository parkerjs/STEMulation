#ifndef LOGICAL_OR_FUNCTOR_H
#define LOGICAL_OR_FUNCTOR_H

#include "logical_binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace logical
{

/**
 * Binary function object class whose call returns the result of the logical "or"
 *                          operation between its two arguments (as returned by operator ||)
 */
template<typename T, typename Result = bool>
class LogicalOrFunctor
: public LogicalBinaryFunctor<T, Result>
{
public:

    /**
     * clone() function
     */
    inline virtual LogicalOrFunctor<T, Result> *clone(void) const override
    {
        return new LogicalOrFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static LogicalOrFunctor<T, Result> *create(void)
    {
        return new LogicalOrFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return lhs || rhs;
    }

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::LogicalOr;
    }
};

}

}

}

}

#endif
