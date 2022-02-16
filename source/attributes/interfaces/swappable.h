#ifndef SWAPPABLE_H
#define SWAPPABLE_H

#include "export_library.h"

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for swappable types
 */
template<typename T>
class Swappable
{
public:

    /**
     * Constructor
     */
    Swappable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Swappable(void)
    {

    }

    /**
     * Swap function
     */
    EXPORT_STEM virtual void swap(T &swappable) = 0;
};

}

}

#endif
