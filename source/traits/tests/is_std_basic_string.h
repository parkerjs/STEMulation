#ifndef IS_STD_BASIC_STRING_H
#define IS_STD_BASIC_STRING_H

#include <string>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is an std::basic_string type
 */
template<typename T>
struct is_std_basic_string final : public std::false_type { };

/**
 * Type traits struct to determine whether or not T is an std::basic_string type (specialization for T =
 * std::basic_string<CharT, Traits, Allocator>
 */
template<typename CharT, typename Traits, typename Allocator>
struct is_std_basic_string<std::basic_string<CharT, Traits, Allocator>> final : public std::true_type { };

}

}

#endif
