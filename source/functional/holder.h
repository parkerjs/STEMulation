#ifndef HOLDER_H
#define HOLDER_H

#include "cloneable.h"
#include <typeinfo>
#include <utility>

namespace functional
{

/**
 * This class serves as an abstract base container class that facilitates the type erasure idiom in classes
 * from which it is derived
 */
class Holder
: public attributes::interfaces::Cloneable<Holder>
{
protected:

    /**
     * Constructor
     */
    Holder(void)
    {

    }

public:

    /**
     * Destructor
     */
    virtual ~Holder(void) override
    {

    }

    /**
     * Return this object's type_info
     */
    EXPORT_STEM virtual const std::type_info &type(void) const = 0;
};

/**
 * This class provides a generic concrete implementation of the Holder class and serves as a basic
 * implementation for classes that implement the type erasure idiom
 */
template<typename T>
class Held
{
public:

    /**
     * Constructor
     */
    template<typename U>
    Held(U &&value)
    : m_held(std::forward<U>(value))
    {

    }

    /**
     * Destructor
     */
    virtual ~Held(void)
    {

    }

    /**
     * Return this object's type_info
     */
    EXPORT_STEM virtual const std::type_info &type(void) const = 0;

    /**
     * the data
     */
    T m_held;
};

}

#endif
