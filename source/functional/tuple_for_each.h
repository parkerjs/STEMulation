#ifndef TUPLE_FOR_EACH_H
#define TUPLE_FOR_EACH_H

#include <functional>
#include <tuple>
#include <type_traits>

namespace functional
{

/**
 * Apply the given function object to each element in a tuple
 */
template<std::size_t Index = 0, typename Tuple, typename Function>
inline typename std::enable_if<Index == std::tuple_size<typename std::decay<Tuple>::type>::value, void>::type
for_each(Tuple &&, Function &&)
{
    // do nothing
}

/**
 * Apply the given function object to each element in a tuple
 */
template<std::size_t Index = 0, typename Tuple, typename Function>
inline typename std::enable_if<Index != std::tuple_size<typename std::decay<Tuple>::type>::value, void>::type
for_each(Tuple &&tuple, Function &&function)
{
    function(std::get<Index>(tuple));
    for_each<Index + 1>(std::forward<Tuple>(tuple), std::forward<Function>(function));
}

}

#endif
