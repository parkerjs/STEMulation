#ifndef BITWISE_XOR_ATTRIBUTE_H
#define BITWISE_XOR_ATTRIBUTE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Base class for bitwise XOR operator overloads
 */
class BaseBitwiseXorAttribute { };

/**
 * Objects derived from this class inherit bitwise XOR operator overloads
 */
DefineBinaryOperatorOverloads(BitwiseXorAttribute, ^)

}

}

}

#endif
