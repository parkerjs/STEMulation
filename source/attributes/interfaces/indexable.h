#ifndef INDEXABLE_H
#define INDEXABLE_H

#include "export_library.h"
#include <string>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can be indexed
 */
template<typename Index>
class Indexable
{
public:

    /**
     * Constructor
     */
    Indexable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Indexable(void)
    {

    }

    /**
     * Get this object's index
     */
    EXPORT_STEM virtual Index getIndex(void) const = 0;

    /**
     * Set this object's index
     */
    EXPORT_STEM virtual void setIndex(Index index) = 0;
};

}

}

#endif
