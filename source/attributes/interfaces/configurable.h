#ifndef CONFIGURABLE_H
#define CONFIGURABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be configured with objects of type T
 */
template<typename T = void>
class Configurable
{
public:

    /**
     * Constructor
     */
    Configurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Configurable(void)
    {

    }

    /**
     * Configure function
     */
    EXPORT_STEM virtual bool configure(T arg) = 0;
};

/**
 * This class provides an interface for types that can be configured without arguments
 */
template<>
class Configurable<void>
{
public:

    /**
     * Constructor
     */
    Configurable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Configurable(void)
    {

    }

    /**
     * Configure function
     */
    EXPORT_STEM virtual bool configure(void) = 0;
};

}

}

#endif
