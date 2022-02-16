#ifndef BITWISE_XOR_ASSIGNABLE_H
#define BITWISE_XOR_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Objects derived from this class must implement the bitwise exclusive XOR assignment operator
 */
template<typename U, typename V>
class BitwiseXorAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~BitwiseXorAssignable(void)
    {

    }

    /**
     * Bitwise XOR assignment operator
     */
    EXPORT_STEM virtual U &operator ^= (const V &) = 0;
};

}

}

}

#endif
