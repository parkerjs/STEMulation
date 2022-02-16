#ifndef LIBRARY_INTERFACE_H
#define LIBRARY_INTERFACE_H

#include "priorityPublisher.h"
#include <map>
#include <set>
#include <string>

namespace library
{

/**
 * This class serves as an abstract base class from which application-specific library interfaces may be
 * derived
 */
class LibraryInterface
: public messaging::PriorityPublisher
{
public:

    /**
     * Enumerations
     */
    enum Status { Paused, Running, Stopped, Unknown };

protected:

    /**
     * Constructor
     */
    LibraryInterface(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~LibraryInterface(void) override
    {

    }

public:

    /**
     * Get the name assigned to this object
     */
    inline virtual std::string getName(void) const
    {
        return "LibraryInterface";
    }

    /**
     * Function to pause execution
     */
    virtual void pause(void) = 0;

    /**
     * Function to query the current execution status
     */
    virtual Status queryStatus(void) = 0;

    /**
     * Function to resume execution
     */
    virtual void resume(void) = 0;

    /**
     * Function to perform shutdown
     */
    virtual bool shutdown(void) = 0;

    /**
     * Function to stop execution
     */
    virtual void stop(void) = 0;
};

}

#endif
