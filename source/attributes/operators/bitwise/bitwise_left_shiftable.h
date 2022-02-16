#ifndef BITWISE_LEFT_SHIFTABLE_H
#define BITWISE_LEFT_SHIFTABLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Base class for bitwise left shift operator overloads
 */
class BaseBitwiseLeftShiftable { };

/**
 * Objects derived from this class inherit bitwise left shift operator overloads
 */
DefineBinaryOperatorOverloads(BitwiseLeftShiftable, <<)

}

}

}

#endif
