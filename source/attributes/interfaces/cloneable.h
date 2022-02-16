#ifndef CLONEABLE_H
#define CLONEABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for cloneable const types
 */
template<typename T>
class Cloneable
{
public:

    /**
     * Constructor
     */
    Cloneable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Cloneable(void)
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual T *clone(void) const = 0;
};

/**
 * This class provides an interface for cloneable non-const types
 */
template<typename T>
class Cloneable<T &>
{
public:

    /**
     * Constructor
     */
    Cloneable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Cloneable(void)
    {

    }

    /**
     * clone() function
     */
    EXPORT_STEM virtual T *clone(void) = 0;
};

}

}

#endif
