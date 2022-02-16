#ifndef BITWISE_NOT_FUNCTOR_H
#define BITWISE_NOT_FUNCTOR_H

#include "unary_functor.h"

namespace functional
{

namespace functors
{

namespace unary
{

/**
 * Unary function object class whose call returns the result of applying the bitwise
 *                           "not" operation to its right-hand argument (as returned by the unary operator ~)
 */
template<typename T, typename Result = T>
class BitwiseNotFunctor
: public UnaryFunctor<T, Result>
{
public:

    /**
     * Function call operator
     */
    inline virtual Result operator () (const T &rhs) override
    {
        return const_cast<const BitwiseNotFunctor &>(*this).evaluate(rhs);
    }

    /**
     * clone() function
     */
    inline virtual BitwiseNotFunctor<T, Result> *clone(void) const override
    {
        return new BitwiseNotFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static BitwiseNotFunctor<T, Result> *create(void)
    {
        return new BitwiseNotFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &rhs) const override
    {
        return const_cast<BitwiseNotFunctor<T, Result> *>(this)->evaluate(rhs, 0);
    }

protected:

    /**
     * Evaluation function
     */
    template<typename U> inline static auto evaluate(const U &rhs, int) ->
    typename std::enable_if<std::is_integral<U>::value, Result>::type
    {
        return ~rhs;
    }

    /**
     * Evaluation function
     */
    template<typename U> inline static auto evaluate(const U &rhs, int) ->
    typename std::enable_if<std::is_floating_point<U>::value, Result>::type
    {
        return ~static_cast<long>(rhs);
    }

public:

    /**
     * Function to get the unary functor type
     */
    inline virtual UnaryFunctorType getType(void) const override
    {
        return UnaryFunctorType::Enum::BitwiseNot;
    }
};

}

}

}

#endif
