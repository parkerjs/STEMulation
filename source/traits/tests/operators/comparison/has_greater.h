#ifndef HAS_COMPARE_GREATER_OPERATOR_H
#define HAS_COMPARE_GREATER_OPERATOR_H

#include "has_operator.h"

namespace traits
{

namespace tests
{

namespace operators
{

namespace comparison
{

/**
 * This class determines whether or not the > operator is defined for the given template arguments
 */
DefineBinaryOperatorOverloadTest(compare_greater, >)

}

}

}

}

#endif
