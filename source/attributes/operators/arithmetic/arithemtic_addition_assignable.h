#ifndef ADDITION_ASSIGNABLE_H
#define ADDITION_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class must implement the addition assignment operator
 */
template<typename U, typename V>
class AdditionAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~AdditionAssignable(void)
    {

    }

    /**
     * Addition assignment operator
     */
    EXPORT_STEM virtual U &operator += (const V &) = 0;
};

}

}

}

#endif
