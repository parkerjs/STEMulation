#ifndef VARIABLE_MAP_REGISTRABLE_H
#define VARIABLE_MAP_REGISTRABLE_H

#include "variable_registry.h"

namespace attributes
{

namespace concrete
{

/**
 * This class provides derived types with a variable registry - along with a concrete interface to use and
 * manipulate the registry
 */
class VariableMapRegistrable
{
public:

    /**
     * Constructor
     * @param keyComparator a function object which defines a custom key comparator
     */
    template<typename KeyComparator = std::less<std::string>>
    VariableMapRegistrable(KeyComparator &&keyComparator = KeyComparator())
    : m_registry(std::forward<KeyComparator>(keyComparator))
    {

    }

private:

    /**
     * Copy constructor
     */
    VariableMapRegistrable(const VariableMapRegistrable &registrable) = delete;

    /**
     * Move constructor
     */
    VariableMapRegistrable(VariableMapRegistrable &&registrable) = delete;

public:

    /**
     * Destructor
     */
    virtual ~VariableMapRegistrable(void)
    {

    }

private:

    /**
     * Copy assignment operator
     */
    VariableMapRegistrable &operator = (const VariableMapRegistrable &registrable) = delete;

    /**
     * Move assignment operator
     */
    VariableMapRegistrable &operator = (VariableMapRegistrable &&registrable) = delete;

public:

    /**
     * Get this object's variable registry
     */
    inline virtual utilities::VariableRegistry &getVariableRegistry(void)
    {
        return m_registry;
    }

    /**
     * Get this object's variable registry
     */
    inline virtual utilities::VariableRegistry getVariableRegistry(void) const
    {
        return m_registry;
    }

    /**
     * Set this object's variable registry
     */
    inline virtual void setVariableRegistry(const utilities::VariableRegistry &registry)
    {
        m_registry = registry;
    }

protected:

    /**
     * a variable registry
     */
    utilities::VariableRegistry m_registry;
};

}

}

#endif
