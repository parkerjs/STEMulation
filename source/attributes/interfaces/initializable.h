#ifndef INITIALIZABLE_H
#define INITIALIZABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can perform initialization
 */
class Initializable
{
public:

    /**
     * Constructor
     */
    Initializable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Initializable(void)
    {

    }

    /**
     * Initialization function
     */
    EXPORT_STEM virtual bool initialize(void) = 0;
};

}

}

#endif
