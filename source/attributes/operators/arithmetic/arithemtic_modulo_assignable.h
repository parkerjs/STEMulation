#ifndef MODULO_ASSIGNABLE_H
#define MODULO_ASSIGNABLE_H

#include "export_library.h"

namespace attributes
{

namespace operators
{

namespace arithmetic
{

/**
 * Objects derived from this class must implement the modulo assignment operator
 */
template<typename U, typename V>
class ModuloAssignable
{
public:

    /**
     * Destructor
     */
    virtual ~ModuloAssignable(void)
    {

    }

    /**
     * Modulo assignment operator
     */
    EXPORT_STEM virtual U &operator %= (const V &) = 0;
};

}

}

}

#endif
