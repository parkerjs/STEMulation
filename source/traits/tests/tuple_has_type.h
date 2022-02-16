#ifndef TUPLE_HAS_TYPE_H
#define TUPLE_HAS_TYPE_H

#include <tuple>
#include <type_traits>

namespace traits
{

namespace tests
{

// forward declarations
template<typename T, typename Tuple> struct tuple_has_type;

/**
 * Type traits test to determine if a tuple contains the specified type
 */
template<typename T, typename Arg, typename ... Args>
struct tuple_has_type<T, std::tuple<Arg, Args ...>>
{
    /**
     * Static constants
     */
    constexpr static bool value = tuple_has_type<T, std::tuple<Args ...>>::value;
};

/**
 * Type traits test to determine if a tuple contains the specified type (specialization for cases that test
 * true)
 */
template<typename T>
struct tuple_has_type<T, std::tuple<>> : std::false_type { };

/**
 * Type traits test to determine if a tuple contains the specified type (specialization for cases that test
 * false)
 */
template<typename T, typename ... Args>
struct tuple_has_type<T, std::tuple<T, Args ...>> : public std::true_type { };

}

}

#endif
