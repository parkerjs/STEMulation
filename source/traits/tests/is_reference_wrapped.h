#ifndef IS_REFERENCE_WRAPPER_H
#define IS_REFERENCE_WRAPPER_H

#include <type_traits>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not the type T is wrapped in an std::reference_wrapper object
 */
template<typename T>
struct is_reference_wrapped final
{
    /**
     * Static constants
     */
    static const bool value = false;
};

/**
 * Type traits struct to determine whether or not the type T is wrapped in an std::reference_wrapper object
 */
template<typename T>
struct is_reference_wrapped<std::reference_wrapper<T>> final
{
    /**
     * Static constants
     */
    static const bool value = true;
};

}

}

#endif
