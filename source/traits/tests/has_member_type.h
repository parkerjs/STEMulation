#ifndef HAS_MEMBER_TYPE_H
#define HAS_MEMBER_TYPE_H

#include <type_traits>

namespace traits
{

namespace tests
{

/* *INDENT-OFF* */

/**
 * This macro defines a class template that checks for the existence of a member type in a given class
 */
#define DefineMemberTypeCheck(member_type)                                                                   \
template<typename Type>                                                                                      \
struct has_ ## member_type final                                                                             \
{                                                                                                            \
    template<typename U> static typename U::member_type test(int);                                           \
    template<typename> static void test(...);                                                                \
                                                                                                             \
    static const bool value = !std::is_void<decltype(test<typename std::decay<Type>::type>(0))>::value;      \
};

/* *INDENT-ON* */

}

}

#endif
