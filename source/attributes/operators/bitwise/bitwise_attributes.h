#ifndef BITWISE_ATTRIBUTES_H
#define BITWISE_ATTRIBUTES_H

#include "bitwise_and_attribute.h"
#include "bitwise_left_shiftable.h"
#include "bitwise_or_attribute.h"
#include "bitwise_right_shiftable.h"
#include "bitwise_xor_attribute.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Objects derived from this class inherit bitwise operator overloads
 */
template<typename T>
class BitwiseAttributes
: public BitwiseAndAttribute<T>,
  public BitwiseLeftShiftable<T>,
  public BitwiseOrAttribute<T>,
  public BitwiseRightShiftable<T>,
  public BitwiseXorAttribute<T>
{

};

}

}

}

#endif
