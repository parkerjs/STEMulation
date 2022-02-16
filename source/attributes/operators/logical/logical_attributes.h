#ifndef LOGICAL_ATTRIBUTES_H
#define LOGICAL_ATTRIBUTES_H

#include "logical_and_attribute.h"
#include "logical_or_attribute.h"

namespace attributes
{

namespace operators
{

namespace logical
{

/**
 * Objects derived from this class inherit logical operator overloads
 */
template<typename T>
class LogicalAttributes
: public LogicalAndAttribute<T>,
  public LogicalOrAttribute<T>
{

};

}

}

}

#endif
