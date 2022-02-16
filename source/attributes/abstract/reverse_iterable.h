#ifndef REVERSE_ITERABLE_H
#define REVERSE_ITERABLE_H

#include "export_library.h"

namespace attributes
{

namespace abstract
{

/**
 * This class provides an abstract interface for types that can be iterated in reverse order
 */
template<template<typename,
                  bool,
                  typename ...> class Iterator,
         typename T,
         typename ... categories>
class ReverseIterable
{
public:

    /**
     * Typedef declarations
     */
    typedef typename Iterator<const T,
                              true,
                              categories ...>::const_reverse_iterator const_reverse_iterator;
    typedef typename Iterator<T,
                              true,
                              categories ...>::reverse_iterator reverse_iterator;

    /**
     * Destructor
     */
    virtual ~ReverseIterable(void)
    {

    }

    /**
     * crbegin() overload
     */
    EXPORT_STEM virtual const_reverse_iterator
    crbegin(void) const = 0;

    /**
     * crend() overload
     */
    EXPORT_STEM virtual const_reverse_iterator
    crend(void) const = 0;

    /**
     * rbegin() overload
     */
    EXPORT_STEM virtual reverse_iterator
    rbegin(void) = 0;

    /**
     * rbegin() overload
     */
    inline virtual const_reverse_iterator
    rbegin(void) const
    {
        return crbegin();
    }

    /**
     * rend() overload
     */
    EXPORT_STEM virtual reverse_iterator
    rend(void) = 0;

    /**
     * rend() overload
     */
    inline virtual const_reverse_iterator
    rend(void) const
    {
        return crend();
    }
};

}

}

#endif
