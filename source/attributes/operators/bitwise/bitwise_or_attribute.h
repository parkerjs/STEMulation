#ifndef BITWISE_OR_ATTRIBUTE_H
#define BITWISE_OR_ATTRIBUTE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Base class for bitwise OR operator overloads
 */
class BaseBitwiseOrAttribute { };

/**
 * Objects derived from this class inherit bitwise OR operator overloads
 */
DefineBinaryOperatorOverloads(BitwiseOrAttribute, |)

}

}

}

#endif
