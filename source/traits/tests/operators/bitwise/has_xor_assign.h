#ifndef HAS_BITWISE_XOR_ASSIGNMENT_OPERATOR_H
#define HAS_BITWISE_XOR_ASSIGNMENT_OPERATOR_H

#include "has_operator.h"

namespace traits
{

namespace tests
{

namespace operators
{

namespace bitwise
{

/**
 * This class determines whether or not the ^= operator is defined for the given template arguments
 */
DefineBinaryOperatorOverloadTest(bitwise_xor_assignment, ^=)

}

}

}

}

#endif
