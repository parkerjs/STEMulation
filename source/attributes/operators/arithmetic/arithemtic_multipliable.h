#ifndef MULTIPLIABLE_H
#define MULTIPLIABLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Base class for multiplication operator overloads
 */
class BaseMultipliable { };

/**
 * Objects derived from this class inherit multiplication operator overloads
 */
DefineBinaryOperatorOverloads(Multipliable, *)

}

}

}

#endif
