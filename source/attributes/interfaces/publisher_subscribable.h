#ifndef PUBLISHER_SUBSCRIBABLE_H
#define PUBLISHER_SUBSCRIBABLE_H

#include "export_library.h"
#include <string>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for subscriber classes in the publisher/
 *                               subscriber design pattern
 */
template<typename Publisher>
class PublisherSubscribable
{
public:

    /**
     * Constructor
     */
    PublisherSubscribable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~PublisherSubscribable(void)
    {

    }

    /**
     * Get the Publisher to which this object subscribes
     */
    EXPORT_STEM virtual Publisher *getPublisher(void) const = 0;

    /**
     * Set the Publisher to which this object subscribes
     */
    EXPORT_STEM virtual void setPublisher(Publisher *pPublisher) = 0;
};

}

}

#endif
