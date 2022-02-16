#ifndef BITWISE_AND_ASSIGNABLE_H
#define BITWISE_AND_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace bitwise
{

/**
 * Objects derived from this class must implement the bitwise AND assignment operator
 */
template<typename U, typename V>
class BitwiseAndAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~BitwiseAndAssignable(void)
    {

    }

    /**
     * Bitwise AND assignment operator
     */
    EXPORT_STEM virtual U &operator &= (const V &) = 0;
};

}

}

}

#endif
