#ifndef IS_ASSOCIATIVE_CONTAINER_H
#define IS_ASSOCIATIVE_CONTAINER_H

#include "has_member_function.h"
#include "has_member_type.h"

namespace traits
{

namespace tests
{

namespace associative_container_details
{

/**
 * Macro definitions
 */
DefineMemberTypeCheck(key_type);
DefineMemberTypeCheck(iterator);
DefineMemberTypeCheck(mapped_type)
DefineMemberTypeCheck(value_type);

}

/**
 * Type traits struct to determine whether or not T is an associative container that defines begin(), cbegin(),
 * cend(), clear(), empty(), end(), erase(), find(), insert(), size() as well as typedefs for key_type,
 * iterator, and value_type
 */
template<typename T, bool = std::is_class<typename std::decay<T>::type>::value &&
                            associative_container_details::has_key_type<typename std::decay<T>::type>::value &&
                            associative_container_details::has_iterator<typename std::decay<T>::type>::value &&
                            associative_container_details::has_mapped_type<typename std::decay<T>::type>::value &&
                            associative_container_details::has_value_type<typename std::decay<T>::type>::value>
struct is_associative_container final
{
    /**
     * Typedef declarations
     */
    typedef typename std::decay<T>::type type;
    typedef typename type::key_type key_type;
    typedef typename type::mapped_type mapped_type;
    typedef typename type::value_type value_type;
    typedef typename type::iterator iterator;

    /**
     * Macro definitions
     */
    DefineMemberFunctionCheck(begin);
    DefineMemberFunctionCheck(cbegin);
    DefineMemberFunctionCheck(cend);
    DefineMemberFunctionCheck(clear);
    DefineMemberFunctionCheck(empty);
    DefineMemberFunctionCheck(end);
    DefineMemberFunctionCheck(erase, std::declval<iterator>());
    DefineMemberFunctionCheck(find, std::declval<key_type>());
    DefineMemberFunctionCheck(insert, std::declval<value_type>());
    DefineMemberFunctionCheck(size);

    /**
     * Static constants
     */
    static const bool value = has_member_begin<type>::value && has_member_cbegin<type>::value &&
                              has_member_cend<type>::value && has_member_clear<type>::value &&
                              has_member_empty<type>::value && has_member_end<type>::value &&
                              has_member_erase<type>::value && has_member_find<type>::value &&
                              has_member_insert<type>::value && has_member_size<type>::value;
};

/**
 * Type traits struct to determine whether or not T is an associative container (specialization for non-class
 * or non-associative container types)
 */
template<typename T>
struct is_associative_container<T, false> final
{
    /**
     * Static constants
     */
    static const bool value = false;
};

}

}

#endif
