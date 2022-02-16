#ifndef LOGICAL_OR_ATTRIBUTE_H
#define LOGICAL_OR_ATTRIBUTE_H

#include <type_traits>

namespace attributes
{

namespace operators
{

namespace logical
{

/**
 * Base class for logical OR operator overloads
 */
class BaseLogicalOrAttribute { };

/**
 * Objects derived from this class inherit logical OR operator overloads
 */
DefineBinaryOperatorOverloads(LogicalOrAttribute, ||)

}

}

}

#endif
