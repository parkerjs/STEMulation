#ifndef REFERENCE_UNWRAPPER_H
#define REFERENCE_UNWRAPPER_H

#include <type_traits>

namespace traits
{

namespace helpers
{

/**
 * Helper class to infer the type stored within an std::reference_wrapper object
 */
template<typename T>
struct reference_unwrapper
{
    /**
     * Typedef declarations
     */
    typedef T type;
};

/**
 * Helper class to infer the type stored within an std::reference_wrapper object
 */
template<typename T>
struct reference_unwrapper<std::reference_wrapper<T>>
{
    /**
     * Typedef declarations
     */
    typedef T type;
};

}

}

#endif
