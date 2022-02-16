#ifndef CLI_ARGUMENT_PROCESSABLE_H
#define CLI_ARGUMENT_PROCESSABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can process command line argument lists
 */
template<typename Processor>
class CLI_ArgumentProcessable
{
public:

    /**
     * Constructor
     */
    CLI_ArgumentProcessable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~CLI_ArgumentProcessable(void)
    {

    }

    /**
     * Get a pointer to this object's CLI argument processor
     */
    EXPORT_STEM virtual Processor *getCLI_ArgumentProcessor(void) = 0;

    /**
     * Set a pointer to this object's CLI argument processor
     */
    EXPORT_STEM virtual void setCLI_ArgumentProcessor(Processor *pCLI_ArgumentProcessor) = 0;
};

}

}

#endif
