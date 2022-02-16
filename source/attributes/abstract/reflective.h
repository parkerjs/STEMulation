#ifndef REFLECTIVE_H
#define REFLECTIVE_H

#include <string>

#include "export_library.h"

namespace attributes
{

namespace abstract
{

// macro definitions
#define getQualifiedStaticMethodName(method, \
                                     qualifier) getQualifiedMethodName(method, \
                                                                       #qualifier)

/**
 * This class provides an abstract interface for types that can perform reflection/introspection
 */
class Reflective
{
protected:

    /**
     * Constructor
     */
    Reflective(void)
    {

    }

    /**
     * Copy constructor
     */
    Reflective(const Reflective &reflective)
    {
        operator = (reflective);
    }

    /**
     * Move constructor
     */
    Reflective(Reflective &&reflective)
    {
        operator = (std::move(reflective));
    }

public:

    /**
     * Destructor
     */
    virtual ~Reflective(void)
    {

    }

protected:

    /**
     * Copy assignment operator
     */
    Reflective &
    operator = (const Reflective &reflective)
    {
        if (&reflective != this)
        {
            // TODO: add members here
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Reflective &
    operator = (Reflective &&reflective)
    {
        if (&reflective != this)
        {
            // TODO: add members here
        }

        return *this;
    }

    /**
     * Get the name of this class
     */
    EXPORT_STEM virtual std::string
    getClassName(void) const = 0;

    /**
     * Get the class-qualified name of the associated method
     */
    inline virtual std::string
    getQualifiedMethodName(const std::string &method) const final
    {
        return getClassName() + "::"
                              + method
                              + "()";
    }

    /**
     * Get the class-qualified name of the associated method
     */
    inline static std::string
    getQualifiedMethodName(const std::string &method,
                           const std::string &qualifier)
    {
        return qualifier + "::"
                         + method
                         + "()";
    }
};

}

}

#endif
