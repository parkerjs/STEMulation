#ifndef IS_RESIZEABLE_SEQUENCE_CONTAINER_H
#define IS_RESIZEABLE_SEQUENCE_CONTAINER_H

#include "is_sequence_container.h"

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is a resizeable sequence container that defines begin(),
 * cbegin(), cend(), empty(), end(), insert(), resize() as well as a typedef for value_type; A typedef for
 * key_type is associated with associative containers and should not be defined for sequence containers
 */
template<typename T, bool = is_sequence_container<typename std::decay<T>::type>::value>
struct is_resizeable_sequence_container final
{
    /**
     * Typedef declarations
     */
    typedef typename std::decay<T>::type type;
    typedef typename type::value_type value_type;
    typedef typename type::iterator iterator;

    /**
     * Macro definitions
     */
    DefineMemberFunctionCheck(assign);
    DefineMemberFunctionCheck(insert, std::declval<iterator>(), std::declval<value_type>());
    DefineMemberFunctionCheck(resize, 1);

    /**
     * Static constants
     */
    static const bool value = has_member_insert<type>::value && has_member_resize<type>::value;
};

/**
 * Type traits struct to determine whether or not T is a resizeable sequence container (specialization for
 * non-class or fixed-size sequence container types)
 */
template<typename T>
struct is_resizeable_sequence_container<T, false> final
{
    /**
     * Static constants
     */
    static const bool value = false;
};

}

}

#endif
