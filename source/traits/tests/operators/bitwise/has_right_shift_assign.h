#ifndef HAS_BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_H
#define HAS_BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_H

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
 * This class determines whether or not the >>= operator is defined for the given template arguments
 */
DefineBinaryOperatorOverloadTest(bitwise_right_shift_assignment, >>=)

}

}

}

}

#endif
