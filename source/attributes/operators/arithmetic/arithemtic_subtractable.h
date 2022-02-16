#ifndef SUBTRACTABLE_H
#define SUBTRACTABLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Base class for subtraction operator overloads
 */
class BaseSubtractable { };

/**
 * Objects derived from this class inherit subtraction operator overloads
 */
DefineBinaryOperatorOverloads(Subtractable, -)

}

}

}

#endif
