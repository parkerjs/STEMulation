#ifndef HAS_MEMBER_FUNCTION_H
#define HAS_MEMBER_FUNCTION_H

#include <type_traits>

namespace traits
{

namespace tests
{

/* *INDENT-OFF* */

/**
 * This macro defines a class template that checks for the existence of a member function in a given class
 */
#define DefineMemberFunctionCheck(method, ...)                                                               \
template<typename U>                                                                                         \
struct has_member_ ## method final                                                                           \
{                                                                                                            \
    typedef typename std::decay<U>::type V;                                                                  \
    static const typename std::decay<U>::type &x;                                                            \
                                                                                                             \
    static auto test(V v) -> decltype(v.method(__VA_ARGS__), std::true_type()) { return std::true_type(); }  \
    static std::false_type test(...) { return std::false_type(); }                                           \
                                                                                                             \
    static const bool value = std::is_same<decltype(test(x)), std::true_type>::value;                        \
};

/* *INDENT-ON* */

}

}

#endif
