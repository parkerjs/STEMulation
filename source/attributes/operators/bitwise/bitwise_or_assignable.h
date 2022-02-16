#ifndef BITWISE_OR_ASSIGNABLE_H
#define BITWISE_OR_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Objects derived from this class must implement the bitwise OR assignment operator
 */
template<typename U, typename V>
class BitwiseOrAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~BitwiseOrAssignable(void)
    {

    }

    /**
     * Bitwise OR assignment operator
     */
    EXPORT_STEM virtual U &operator |= (const V &) = 0;
};

}

}

}

#endif
