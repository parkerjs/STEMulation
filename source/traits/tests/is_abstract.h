#ifndef IS_ABSTRACT_H
#define IS_ABSTRACT_H

#include <type_traits>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine if T is an abstract type
 */
template<typename T>
struct is_abstract final
{
    template<typename U> static std::false_type test(U(*)[1]);
    template<typename U> static std::true_type test(...);

    /**
     * Static constants
     */
    static const bool value = std::is_same<decltype(test<T>(0)), std::true_type>::value;
};

}

}

#endif
