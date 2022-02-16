#ifndef IS_COMPLEX_H
#define IS_COMPLEX_H

#include <complex>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is an std::complex type
 */
template<typename T>
struct is_complex final
{
    /**
     * Static constants
     */
    static const bool value = false;
};

/**
 * Type traits struct to determine whether or not T is an std::complex type (specialization for T =
 * std::complex<U>
 */
template<typename T>
struct is_complex<std::complex<T>> final
{
    /**
     * Static constants
     */
    static const bool value = true;
};

}

}

#endif
