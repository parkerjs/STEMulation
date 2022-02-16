#ifndef CONJUGATE_H
#define CONJUGATE_H

#include "is_complex.h"
#include <complex>

namespace math
{

namespace number_systems
{

namespace complex
{

/**
 * A helper class which handles the complex conjugate of both real and complex arguments
 */
struct Conjugate
{
    /**
     * Function to compute the complex conjugate of a complex argument
     */
    template<typename Arg, typename std::enable_if<traits::tests::is_complex<Arg>::value, int>::type = 0>
    inline auto operator () (Arg &&arg)
    {
        return std::conj(std::forward<Arg>(arg));
    };

    /**
     * Function to compute the complex conjugate of a real argument
     */
    template<typename Arg, typename std::enable_if<!traits::tests::is_complex<Arg>::value, int>::type = 0>
    inline auto operator () (Arg &&arg)
    {
        return arg;
    };
};

}

}

}

#endif
