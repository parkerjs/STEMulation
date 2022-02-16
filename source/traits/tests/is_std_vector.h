#ifndef IS_STD_VECTOR_H
#define IS_STD_VECTOR_H

#include <vector>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is an std::vector type
 */
template<typename T>
struct is_std_vector final : public std::false_type { };

/**
 * Type traits struct to determine whether or not T is an std::set type (specialization for T = set::set)
 */
template<typename T>
struct is_std_vector<std::vector<T>> final : public std::true_type { };

}

}

#endif
