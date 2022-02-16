#ifndef STATIC_VARIABLE_MAP_REGISTRABLE_H
#define STATIC_VARIABLE_MAP_REGISTRABLE_H

#include "variable_registry.h"

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a variable registry, statically shared across all instances of the
 * same type - along with a concrete, static interface to use and manipulate the registry
 */
template<typename T>
class StaticVariableMapRegistrable
{
public:

    /**
     * Constructor
     */
    StaticVariableMapRegistrable(void)
    {

    }

private:

    /**
     * Copy constructor
     */
    StaticVariableMapRegistrable(const StaticVariableMapRegistrable &) = delete;

    /**
     * Move constructor
     */
    StaticVariableMapRegistrable(StaticVariableMapRegistrable &&) = delete;

public:

    /**
     * Destructor
     */
    virtual ~StaticVariableMapRegistrable(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    StaticVariableMapRegistrable &operator = (const StaticVariableMapRegistrable &) = delete;

    /**
     * Move assignment operator
     */
    StaticVariableMapRegistrable &operator = (StaticVariableMapRegistrable &&) = delete;

public:

    /**
     * Get this object's variable registry
     */
    inline static utilities::VariableRegistry &getStaticVariableRegistry(void)
    {
        static utilities::VariableRegistry registry; // static variable registry

        return registry;
    }

    /**
     * Set this object's variable registry
     */
    inline static void setStaticVariableRegistry(const utilities::VariableRegistry &registry)
    {
        getStaticVariableRegistry() = registry;
    }
};

}

}

#endif
