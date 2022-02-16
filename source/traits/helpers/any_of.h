#ifndef ANY_OF_H
#define ANY_OF_H

#include <type_traits>

namespace traits
{

namespace helpers
{

/**
 * Helper class to implement a static, compile-time version of std::any_of (deprecate this once c++17 and fold
 * expressions come along)
 */
template<bool ... values>
struct any_of : std::false_type { };

/**
 * Helper class to implement a static, compile-time version of std::any_of (specialization for true types)
 */
template<bool ... values>
struct any_of<true, values ...> : std::true_type { };

/**
 * Helper class to implement a static, compile-time version of std::any_of (specialization for false types)
 */
template<bool ... values>
struct any_of<false, values ...> : any_of<values ...> { };

}

}

#endif
