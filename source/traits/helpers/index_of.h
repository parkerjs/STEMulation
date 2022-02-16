#ifndef INDEX_OF_H
#define INDEX_OF_H

#include <tuple>
#include <type_traits>

namespace traits
{

namespace helpers
{

/**
 * Helper class to return the index of the first occurrence of a specified type in a tuple
 */
template<typename T, typename Tuple>
struct index_of;

/**
 * Helper class to return the index of the first occurrence of a specified type in a tuple (specialization for
 * single-element tuples)
 */
template<typename T, typename ... Types>
struct index_of<T, std::tuple<T, Types ...>>
{
    static const std::size_t value = 0;
};

/**
 * Helper class to return the index of the first occurrence of a specified type in a tuple (specialization for
 * multi-element tuples)
 */
template<typename T, typename U, typename ... Types>
struct index_of<T, std::tuple<U, Types ...>>
{
    static const std::size_t value = 1 + index_of<T, std::tuple<Types ...>>::value;
};

}

}

#endif
