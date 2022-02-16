#ifndef INHERITED_ITERATOR_H
#define INHERITED_ITERATOR_H

namespace iterators
{

// forward declarations
template<typename, bool, typename ...> class Iterator;
struct inherited_iterator_tag;

/**
 * This specialization implements an iterator which inherits its definitions for const_iterator and iterator
 * from those defined within type T
 */
template<typename T>
class Iterator<T, false, inherited_iterator_tag>
{
public:

    /**
     * Typedef declarations
     */
    typedef typename T::const_iterator const_iterator;
    typedef typename T::iterator iterator;
};

/**
 * This specialization implements an iterator which inherits its definitions for const_reverse_iterator and
 * reverse_iterator from those defined within type T
 */
template<typename T>
class Iterator<T, true, inherited_iterator_tag>
{
public:

    /**
     * Typedef declarations
     */
    typedef typename T::const_reverse_iterator const_reverse_iterator;
    typedef typename T::reverse_iterator reverse_iterator;
};

}

#endif
