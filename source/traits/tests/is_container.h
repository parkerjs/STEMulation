#ifndef IS_CONTAINER_H
#define IS_CONTAINER_H

#include "is_associative_container.h"
#include "is_sequence_container.h"

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is an stl-similar container
 */
template<typename T>
struct is_container final
{
    /**
     * Static constants
     */
    static const bool value = is_associative_container<typename std::decay<T>::type>::value ||
                              is_sequence_container<typename std::decay<T>::type>::value;
};

}

}

#endif
