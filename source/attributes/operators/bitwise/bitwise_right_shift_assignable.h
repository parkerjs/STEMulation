#ifndef BITWISE_RIGHT_SHIFT_ASSIGNABLE_H
#define BITWISE_RIGHT_SHIFT_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Objects derived from this class must implement the right assignment operator
 */
template<typename U, typename V>
class RightShiftAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~RightShiftAssignable(void)
    {

    }

    /**
     * Bitwise right shift assignment operator
     */
    EXPORT_STEM virtual U &operator >>= (const V &) = 0;
};

}

}

}

#endif
