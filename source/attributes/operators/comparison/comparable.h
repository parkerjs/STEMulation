#ifndef COMPARISON_ATTRIBUTES_H
#define COMPARISON_ATTRIBUTES_H

#include "compare_equal_attribute.h"
#include "compare_greater_attribute.h"
#include "compare_greater_equal_attribute.h"
#include "compare_less_attribute.h"
#include "compare_less_equal_attribute.h"
#include "compare_not_equal_attribute.h"

namespace attributes
{

namespace operators
{

namespace comparison
{

/**
 * Objects derived from this class inherit comparison operator overloads. The user should at least implement
 * the compare greater and compare equal operator overloads
 */
template<typename T>
class Comparable
: public CompareEqualAttribute<T>,
  public CompareGreaterAttribute<T>,
  public CompareGreaterEqualAttribute<T>,
  public CompareLessAttribute<T>,
  public CompareLessEqualAttribute<T>,
  public CompareNotEqualAttribute<T>
{

};

}

}

}

#endif
