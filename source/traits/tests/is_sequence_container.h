#ifndef IS_SEQUENCE_CONTAINER_H
#define IS_SEQUENCE_CONTAINER_H

#include "has_member_function.h"
#include "has_member_type.h"

namespace traits
{

namespace tests
{

namespace sequence_container_details
{

/**
 * Macro definitions
 */
DefineMemberTypeCheck(key_type);
DefineMemberTypeCheck(iterator);
DefineMemberTypeCheck(value_type);

}

/**
 * Type traits struct to determine whether or not T is a sequence container that defines begin(), cbegin(),
 * cend(), empty(), end() as well typedef for both iterator and value_type; A typedef for key_type is
 * associated with associative containers and should not be defined for sequence containers
 */
template<typename T, bool = std::is_class<typename std::decay<T>::type>::value &&
                            sequence_container_details::has_value_type<typename std::decay<T>::type>::value &&
                            !sequence_container_details::has_key_type<typename std::decay<T>::type>::value &&
                            sequence_container_details::has_iterator<typename std::decay<T>::type>::value>
struct is_sequence_container final
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
    DefineMemberFunctionCheck(begin);
    DefineMemberFunctionCheck(cbegin);
    DefineMemberFunctionCheck(cend);
    DefineMemberFunctionCheck(empty);
    DefineMemberFunctionCheck(end);
    DefineMemberTypeCheck(key_type);
    DefineMemberTypeCheck(value_type);

    /**
     * Static constants
     */
    static const bool value = has_member_begin<type>::value && has_member_cbegin<type>::value &&
                              has_member_cend<type>::value && has_member_empty<type>::value &&
                              has_member_end<type>::value;
};

/**
 * Type traits struct to determine whether or not T is a sequence container (specialization for non-class or
 * associative container types)
 */
template<typename T>
struct is_sequence_container<T, false> final
{
    /**
     * Static constants
     */
    static const bool value = false;
};

}

}

#endif
