#ifndef SETUPABLE_H
#define SETUPABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can perform setup
 */
class Setupable
{
public:

    /**
     * Constructor
     */
    Setupable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Setupable(void)
    {

    }

    /**
     * Setup function
     */
    EXPORT_STEM virtual bool setup(void) = 0;
};

}

}

#endif
