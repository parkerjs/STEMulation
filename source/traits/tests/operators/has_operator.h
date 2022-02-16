#ifndef HAS_OPERATOR_H
#define HAS_OPERATOR_H

#include "fallback.h"

namespace traits
{

namespace tests
{

namespace operators
{

/* *INDENT-OFF* */

/**
 * This macro defines a class template to check whether or not the binary operator overload exists
 */
#define DefineBinaryOperatorOverloadTest(Operation, Symbol)                                                  \
namespace operator_tests                                                                                     \
{                                                                                                            \
                                                                                                             \
using namespace fallback_operators;                                                                          \
                                                                                                             \
template<typename U, typename V>                                                                             \
struct has_ ## Operation ## _operator                                                                        \
{                                                                                                            \
private:                                                                                                     \
                                                                                                             \
    static const typename std::decay<U>::type &x;                                                            \
    static const typename std::decay<V>::type &y;                                                            \
                                                                                                             \
public:                                                                                                      \
                                                                                                             \
    static const bool value = std::is_same<decltype(check(x Symbol y)), std::true_type>::value;              \
};                                                                                                           \
                                                                                                             \
}                                                                                                            \
                                                                                                             \
template<typename U, typename V = U>                                                                         \
struct has_ ## Operation ## _operator final                                                                  \
: public operator_tests::has_ ## Operation ## _operator<U, V>                                                \
{                                                                                                            \
                                                                                                             \
};

/* *INDENT-ON* */

}

}

}

#endif
