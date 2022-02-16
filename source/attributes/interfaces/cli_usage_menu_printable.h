#ifndef CLI_USAGE_MENU_PRINTABLE_H
#define CLI_USAGE_MENU_PRINTABLE_H

#include "export_library.h"

#include <iostream>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that implement a printable command-line usage help menu
 */
class CLI_UsageMenuPrintable
{
public:

    /**
     * Constructor
     */
    CLI_UsageMenuPrintable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~CLI_UsageMenuPrintable(void)
    {

    }

    /**
     * Function to print the usage help menu to the specified stream
     * @param stream a reference to an std::ostream object
     */
    EXPORT_STEM virtual void printUsage(std::ostream &stream) const = 0;
};

}

}

#endif
