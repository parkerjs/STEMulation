#ifndef ARITHMETIC_MODULO_FUNCTOR_H
#define ARITHMETIC_MODULO_FUNCTOR_H

#include "arithmetic_binary_functor.h"
#include <cmath>

namespace functional
{

namespace functors
{

namespace binary
{

namespace arithmetic
{

/**
 * Binary function object class whose call returns the result of the modulus operation between its two
 * arguments (as returned by operator %)
 */
template<typename T, typename Result = T>
class ModuloFunctor
: public ArithmeticBinaryFunctor<T, Result>
{
public:

    /**
     * Function call operator
     */
    inline virtual Result operator () (const T &lhs, const T &rhs) override
    {
        return const_cast<const ModuloFunctor &>(*this).evaluate(lhs, rhs);
    }

    /**
     * clone() function
     */
    inline virtual ModuloFunctor<T, Result> *clone(void) const override
    {
        return new ModuloFunctor<T, Result>(*this);
    }

    /**
     * create() function
     */
    inline static ModuloFunctor<T, Result> *create(void)
    {
        return new ModuloFunctor<T, Result>();
    }

    /**
     * Evaluation function
     */
    inline virtual Result evaluate(const T &lhs, const T &rhs) const override
    {
        return const_cast<ModuloFunctor *>(this)->evaluate(lhs, rhs, 0);
    }

private:

    /**
     * Evaluation function
     */
    template<typename U> inline static auto evaluate(const U &lhs, const U &rhs, int) ->
    typename std::enable_if<std::is_integral<U>::value, Result>::type
    {
        return lhs % rhs;
    }

    /**
     * Evaluation function
     */
    template<typename U> inline static auto evaluate(const U &lhs, const U &rhs, int) ->
    typename std::enable_if<std::is_floating_point<U>::value, Result>::type
    {
        return std::fmod(lhs, rhs);
    }

public:

    /**
     * Function to get the binary functor type
     */
    inline virtual BinaryFunctorType getType(void) const override
    {
        return BinaryFunctorType::Enum::Modulo;
    }
};

}

}

}

}

#endif
