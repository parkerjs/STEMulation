#ifndef IS_PAIR_ASSOCIATIVE_CONTAINER_H
#define IS_PAIR_ASSOCIATIVE_CONTAINER_H

#include "is_associative_container.h"
#include <utility>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is a pair-associative container that defines begin(),
 * cbegin(), cend(), clear(), empty(), end(), erase(), find(), insert(), size() as well as typedefs for
 * key_type and value_type; value_type should be an std::pair
 */
template<typename T, bool = is_associative_container<typename std::decay<T>::type>::value>
struct is_pair_associative_container final
{
    /**
     * Typedef declarations
     */
    typedef typename std::decay<T>::type type;
    typedef typename type::value_type value_type;

    /**
     * Type traits struct to determine whether or not T is an std::pair
     */
    template<typename> struct is_pair { static const bool value = false; };
    template<typename U, typename V> struct is_pair<std::pair<U, V>> { static const bool value = true; };

    /**
     * Static constants
     */
    static const bool value = is_pair<value_type>::value;
};

/**
 * Type traits struct to determine whether or not T is an associative container (specialization for non-class
 * or non-associative container types)
 */
template<typename T>
struct is_pair_associative_container<T, false> final
{
    /**
     * Static constants
     */
    static const bool value = false;
};

}

}

#endif
