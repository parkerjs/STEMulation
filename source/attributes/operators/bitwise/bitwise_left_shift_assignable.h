#ifndef BITWISE_LEFT_SHIFT_ASSIGNABLE_H
#define BITWISE_LEFT_SHIFT_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Objects derived from this class must implement the left shift assignment operator
 */
template<typename U, typename V>
class LeftShiftAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~LeftShiftAssignable(void)
    {

    }

    /**
     * Bitwise left shift assignment operator
     */
    EXPORT_STEM virtual U &operator <<= (const V &) = 0;
};

}

}

}

#endif
