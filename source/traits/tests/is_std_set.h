#ifndef IS_STD_SET_H
#define IS_STD_SET_H

#include <set>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is an std::set type
 */
template<typename T>
struct is_std_set final : public std::false_type { };

/**
 * Type traits struct to determine whether or not T is an std::set type (specialization for T = set::set)
 */
template<typename T>
struct is_std_set<std::set<T>> final : public std::true_type { };

}

}

#endif
