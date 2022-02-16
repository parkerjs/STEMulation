#ifndef HAS_SUBTRACTION_OPERATOR_H
#define HAS_SUBTRACTION_OPERATOR_H

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
 * This class determines whether or not the - operator is defined for the given template arguments
 */
DefineBinaryOperatorOverloadTest(subtraction, -)

}

}

}

}

#endif
