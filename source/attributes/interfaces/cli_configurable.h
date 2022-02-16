#ifndef CLI_CONFIGURABLE_H
#define CLI_CONFIGURABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be configured using command-line arguments
 */
class CLI_Configurable
{
public:

    /**
     * Constructor
     */
    CLI_Configurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~CLI_Configurable(void)
    {

    }

    /**
     * Configure function (using command-line arguments)
     */
    EXPORT_STEM virtual bool configure(int /*argc*/, char ** /*argv*/) = 0;
};

}

}

#endif
