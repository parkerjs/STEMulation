#ifndef BITWISE_OR_FUNCTOR_H
#define BITWISE_OR_FUNCTOR_H

#include "bitwise_binary_functor.h"

namespace functional
{

namespace functors
{

namespace binary
{

namespace bitwise
{

/**
 * Binary function object class whose call returns the result of applying the bitwise
 *                          "or" operation between its two arguments (as returned by operator |)
 */
template<typename T, typename Result = T>
class BitwiseOrFunctor
: public BitwiseBinaryFunctor<T, Result>
{
public:

    /**
     * Function call operator
     */
    inline virtual Result operator () (const T &lhs, const T &rhs) override
    {
        return const_cast<const BitwiseOrFunctor &>(*this).evaluate(lhs, rhs);
    }

    /**
     * clone() function
     */
    inline virtual BitwiseOrFunctor<T, Result> *clone(void) const override
    {
        return new BitwiseOrFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static BitwiseOrFunctor<T, Result> *create(void)
    {
        return new BitwiseOrFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return const_cast<BitwiseOrFunctor<T, Result> *>(this)->evaluate(lhs, rhs, 0);
    }

protected:

    /**
     * Evaluation function
     */
    template<typename U> inline static auto evaluate(const U &lhs, const U &rhs, int) ->
    typename std::enable_if<std::is_integral<U>::value, Result>::type
    {
        return lhs | rhs;
    }

    /**
     * Evaluation function
     */
    template<typename U> inline static auto evaluate(const U &lhs, const U &rhs, int) ->
    typename std::enable_if<std::is_floating_point<U>::value, Result>::type
    {
        return static_cast<long>(lhs) | static_cast<long>(rhs);
    }

public:

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::BitwiseOr;
    }
};

}

}

}

}

#endif
