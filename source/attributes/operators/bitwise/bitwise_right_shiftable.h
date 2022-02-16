#ifndef BITWISE_RIGHT_SHIFTABLE_H
#define BITWISE_RIGHT_SHIFTABLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Base class for bitwise right shift operator overloads
 */
class BaseBitwiseRightShiftable { };

/**
 * Objects derived from this class inherit bitwise right shift operator overloads
 */
DefineBinaryOperatorOverloads(BitwiseRightShiftable, >>)

}

}

}

#endif
