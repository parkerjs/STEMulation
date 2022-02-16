#ifndef BITWISE_AND_ATTRIBUTE_H
#define BITWISE_AND_ATTRIBUTE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Base class for bitwise AND operator overloads
 */
class BaseBitwiseAndAttribute { };

/**
 * Objects derived from this class inherit bitwise AND operator overloads
 */
DefineBinaryOperatorOverloads(BitwiseAndAttribute, &)

}

}

}

#endif
