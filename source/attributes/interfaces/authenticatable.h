#ifndef AUTHENTICATABLE_H
#define AUTHENTICATABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that require arbitrary forms of authentication
 */
template<typename Authentication>
class Authenticatable
{
public:

    /**
     * Constructor
     */
    Authenticatable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Authenticatable(void)
    {

    }

    /**
     * Retrieve this object's authentication credential(s)
     */
    EXPORT_STEM virtual Authentication getAuthentication(void) = 0;

    /**
     * Set this object's authentication credential(s)
     */
    EXPORT_STEM virtual bool setAuthentication(const Authentication &authentication) = 0;
};

}

}

#endif
