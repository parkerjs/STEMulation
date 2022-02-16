#ifndef NAMEABLE_H
#define NAMEABLE_H

#include "export_library.h"
#include <string>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for nameable types
 */
class Nameable
{
public:

    /**
     * Constructor
     */
    Nameable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Nameable(void)
    {

    }

    /**
     * Get the name assigned to this object
     */
    EXPORT_STEM virtual std::string getName(void) const = 0;

    /**
     * Set the name assigned to this object
     */
    EXPORT_STEM virtual void setName(const std::string &name) = 0;
};

}

}

#endif
