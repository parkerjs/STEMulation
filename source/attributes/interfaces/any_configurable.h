#ifndef ANY_CONFIGURABLE_H
#define ANY_CONFIGURABLE_H

#include "any.h"
#include "export_library.h"
#include <vector>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be configured using a vector of Any objects
 */
class AnyConfigurable
{
public:

    /**
     * Constructor
     */
    AnyConfigurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~AnyConfigurable(void)
    {

    }

    /**
     * Configure function (using a vector of Any objects)
     */
    EXPORT_STEM virtual bool configure(const std::vector<functional::Any> & /*anys*/) = 0;
};

}

}

#endif
