#ifndef MODULABLE_H
#define MODULABLE_H

#include "operator.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Base class for modulo operator overloads
 */
class BaseModulable { };

/**
 * Objects derived from this class inherit modulo operator overloads
 */
DefineBinaryOperatorOverloads(Modulable, %)

}

}

}

#endif
