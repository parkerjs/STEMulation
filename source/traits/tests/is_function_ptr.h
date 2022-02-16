#ifndef IS_FUNCTION_PTR_H
#define IS_FUNCTION_PTR_H

#include <type_traits>

namespace traits
{

namespace tests
{

/**
 * Type traits struct to determine whether or not T is a function pointer
 */
template<typename T>
struct is_function_ptr final
{
    /**
     * Typedef declarations
     */
    typedef typename std::decay<T>::type type;

    /**
     * Static constants
     */
    static const bool value = std::is_pointer<type>::value &&
                              std::is_function<typename std::remove_pointer<type>::type>::value;
};

}

}

#endif
