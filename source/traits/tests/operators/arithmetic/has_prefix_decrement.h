#ifndef HAS_PREFIX_DECREMENT_OPERATOR_H
#define HAS_PREFIX_DECREMENT_OPERATOR_H

#include "has_operator.h"

namespace traits
{

namespace tests
{

namespace operators
{

namespace arithmetic
{

namespace operator_tests
{

using namespace fallback_operators;

/**
 * This class determines whether or not the prefix decrement operator is defined for the given template
 * argument
 */
template<typename T>
class has_prefix_decrement_operator
{
    /**
     * a helper reference to decay type T
     */
    static const typename std::decay<T>::type &x;

public:

    /**
     * Compile-time constants
     */
    static const bool value = std::is_same<decltype(check(operator -- (x, int))), std::true_type>::value;
};

}

/**
 * This class determines whether or not the prefix decrement operator is defined for the given template
 * argument
 */
template<typename T>
struct has_prefix_decrement_operator final
: public operator_tests::has_prefix_decrement_operator<T>
{

};

}

}

}

}

#endif
