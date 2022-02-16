#ifndef HAS_LOGICAL_AND_OPERATOR_H
#define HAS_LOGICAL_AND_OPERATOR_H

#include "has_operator.h"

namespace traits
{

namespace tests
{

namespace operators
{

namespace logical
{

/**
 * This class determines whether or not the logical && operator is defined for the given template arguments
 */
DefineBinaryOperatorOverloadTest(logical_and, &&)

}

}

}

}

#endif
