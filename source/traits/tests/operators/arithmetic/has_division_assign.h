#ifndef HAS_DIVISION_ASSIGNMENT_OPERATOR_H
#define HAS_DIVISION_ASSIGNMENT_OPERATOR_H

#include "has_operator.h"

namespace traits
{

namespace tests
{

namespace operators
{

namespace arithmetic
{

/**
 * This class determines whether or not the /= operator is defined for the given template arguments
 */
DefineBinaryOperatorOverloadTest(division_assignment, /=)

}

}

}

}

#endif
