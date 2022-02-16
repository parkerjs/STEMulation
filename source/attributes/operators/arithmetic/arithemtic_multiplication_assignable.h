#ifndef MULTIPLICATION_ASSIGNABLE_H
#define MULTIPLICATION_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class must implement the multiplication assignment operator
 */
template<typename U, typename V>
class MultiplicationAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~MultiplicationAssignable(void)
    {

    }

    /**
     * Multiplication assignment operator
     */
    EXPORT_STEM virtual U &operator *= (const V &) = 0;
};

}

}

}

#endif
