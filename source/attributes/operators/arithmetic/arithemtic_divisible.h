#ifndef DIVISIBLE_H
#define DIVISIBLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Base class for division operator overloads
 */
class BaseDivisible { };

/**
 * Objects derived from this class inherit division operator overloads
 */
DefineBinaryOperatorOverloads(Divisible, /)

}

}

}

#endif
