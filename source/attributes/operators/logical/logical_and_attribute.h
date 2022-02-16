#ifndef LOGICAL_AND_ATTRIBUTE_H
#define LOGICAL_AND_ATTRIBUTE_H

#include <type_traits>

namespace attributes
{

namespace operators
{

namespace logical
{

/**
 * Base class for logical AND operator overloads
 */
class BaseLogicalAndAttribute { };

/**
 * Objects derived from this class inherit logical AND operator overloads
 */
DefineBinaryOperatorOverloads(LogicalAndAttribute, &&)

}

}

}

#endif
