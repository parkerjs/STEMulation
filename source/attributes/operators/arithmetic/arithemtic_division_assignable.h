#ifndef DIVISION_ASSIGNABLE_H
#define DIVISION_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class must implement the division assignment operator
 */
template<typename U, typename V>
class DivisionAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~DivisionAssignable(void)
    {

    }

    /**
     * Division assignment operator
     */
    EXPORT_STEM virtual U &operator /= (const V &) = 0;
};

}

}

}

#endif
