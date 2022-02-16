#ifndef ARITHMETIC_ADDABLE_H
#define ARITHMETIC_ADDABLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Base class for addition operator overloads
 */
class BaseAddable { };

/**
 * Objects derived from this class inherit addition operator overloads
 */
DefineBinaryOperatorOverloads(Addable, +)

}

}

}

#endif
