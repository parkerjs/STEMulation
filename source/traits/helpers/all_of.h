#ifndef ALL_OF_H
#define ALL_OF_H

#include <type_traits>

namespace traits
{

namespace helpers
{

/**
 * Helper class to implement a static, compile-time version of std::all_of (deprecate this once c++17 and fold
 * expressions come along)
 */
template<bool ... values>
struct all_of : std::true_type { };

/**
 * Helper class to implement a static, compile-time version of std::all_of (specialization for true types)
 */
template<bool ... values>
struct all_of<true, values ...> : all_of<values ...> { };

/**
 * Helper class to implement a static, compile-time version of std::all_of (specialization for false types)
 */
template<bool ... values>
struct all_of<false, values ...> : std::false_type { };

}

}

#endif
