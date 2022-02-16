#ifndef UPDATEABLE_H
#define UPDATEABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be updated as a function of time
 */
class Updateable
{
public:

    /**
     * Constructor
     */
    Updateable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Updateable(void)
    {

    }

    /**
     * Update function
     */
    EXPORT_STEM virtual bool update(double time) = 0;
};

}

}

#endif
