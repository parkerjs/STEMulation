#ifndef SUBTRACTION_ASSIGNABLE_H
#define SUBTRACTION_ASSIGNABLE_H

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
class SubtractionAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~SubtractionAssignable(void)
    {

    }

    /**
     * Subtraction assignment operator
     */
    EXPORT_STEM virtual U &operator -= (const V &) = 0;
};

}

}

}

#endif
