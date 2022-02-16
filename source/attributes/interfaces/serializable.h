#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "export_library.h"
#include <iostream>

namespace attributes
{

namespace interfaces
{

/**
 * This class provides an interface for types that can perform serialization
 */
class Serializable
{
public:

    /**
     * Constructor
     */
    Serializable(void)
    {

    }

    /**
     * Destructor
     */
    virtual ~Serializable(void)
    {

    }

    /**
     * Function to deserialize this object's data
     */
    EXPORT_STEM virtual std::istream &deserialize(std::istream &stream) = 0;

    /**
     * Function to serialize this object's data
     */
    EXPORT_STEM virtual std::ostream &serialize(std::ostream &stream) const = 0;
};

}

}

#endif
