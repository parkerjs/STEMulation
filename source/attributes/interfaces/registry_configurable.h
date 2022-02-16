#ifndef REGISTRY_CONFIGURABLE_H
#define REGISTRY_CONFIGURABLE_H

#include "export_library.h"
#include "variable_registry.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be configured using a variable registry
 */
class RegistryConfigurable
{
public:

    /**
     * Constructor
     */
    RegistryConfigurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~RegistryConfigurable(void)
    {

    }

    /**
     * Configure function (using variable registry)
     */
    EXPORT_STEM virtual bool configure(utilities::VariableRegistry & /*registry*/) = 0;
};

}

}

#endif
