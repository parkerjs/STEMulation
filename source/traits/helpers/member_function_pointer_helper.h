#ifndef MEMBER_FUNCTION_POINTER_HELPER_H
#define MEMBER_FUNCTION_POINTER_HELPER_H

#include "pointer_unwrapper.h"

namespace traits
{

namespace helpers
{

/**
 * This helper class provides a member function typedef of the indicated class type, return type and function
 * signature
 */
template<typename Class,
         typename Return,
         typename ... Args>
struct member_function_pointer_helper
{
    /**
     * Typedef declarations
     */
    typedef typename traits::helpers::pointer_unwrapper<Class>::type type;
    typedef Return (type::*const_function)(Args ...) const;
    typedef Return (type::*function)(Args ...);
};

}

}

#endif
