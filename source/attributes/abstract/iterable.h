#ifndef ITERABLE_H
#define ITERABLE_H

#include "export_library.h"

namespace attributes
{

namespace abstract
{

/**
 * This class provides an abstract interface for types that can be iterated
 */
template<template<typename,
                  bool,
                  typename ...> class Iterator,
         typename T,
         typename ... categories>
class Iterable
{
public:

    /**
     * Typedef declarations
     */
    typedef typename Iterator<const T,
                              false,
                              categories ...>::const_iterator const_iterator;
    typedef typename Iterator<T,
                              false,
                              categories ...>::iterator iterator;

    /**
     * Destructor
     */
    virtual ~Iterable(void)
    {

    }

    /**
     * begin() overload
     */
    EXPORT_STEM virtual iterator
    begin(void) = 0;

    /**
     * begin() overload
     */
    inline virtual const_iterator
    begin(void) const
    {
        return cbegin();
    }

    /**
     * cbegin() overload
     */
    EXPORT_STEM virtual const_iterator
    cbegin(void) const = 0;

    /**
     * cend() overload
     */
    EXPORT_STEM virtual const_iterator
    cend(void) const = 0;

    /**
     * end() overload
     */
    EXPORT_STEM virtual iterator
    end(void) = 0;

    /**
     * end() overload
     */
    inline virtual const_iterator
    end(void) const
    {
        return cend();
    }
};

}

}

#endif
