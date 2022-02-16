#ifndef FINALIZABLE_H
#define FINALIZABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can perform finalization
 */
class Finalizable
{
public:

    /**
     * Constructor
     */
    Finalizable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Finalizable(void)
    {

    }

    /**
     * Finalization function
     */
    EXPORT_STEM virtual bool finalize(void) = 0;
};

}

}

#endif
