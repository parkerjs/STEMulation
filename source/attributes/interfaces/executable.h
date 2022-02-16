#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for executable types
 */
class Executable
{
public:

    /**
     * Constructor
     */
    Executable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Executable(void)
    {

    }

    /**
     * Execution function
     */
    EXPORT_STEM virtual bool execute(void) = 0;
};

}

}

#endif
