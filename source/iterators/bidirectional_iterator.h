#ifndef BIDIRECTIONAL_ITERATOR_H
#define BIDIRECTIONAL_ITERATOR_H

#include "forward_iterator.h"

namespace iterators
{

/**
 * This specialization implements a generic stl-style bidirectional iterator
 */
template<typename T, bool reverse, typename ... categories>
class Iterator<T, reverse, std::bidirectional_iterator_tag, categories ...>
: public Iterator<T, reverse, std::forward_iterator_tag, std::bidirectional_iterator_tag, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef Iterator<T, reverse, std::forward_iterator_tag, std::bidirectional_iterator_tag,
                     categories ...> base_iterator;
    typedef typename std::tuple_element<sizeof ... (categories), std::tuple<std::bidirectional_iterator_tag,
                                        categories ...>>::type category;
    typedef Iterator<const T, reverse, std::bidirectional_iterator_tag, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, reverse, category> derived_iterator;
    typedef typename base_iterator::difference_type difference_type;
    typedef Iterator<T, reverse, std::bidirectional_iterator_tag, categories ...> iterator;
    typedef typename base_iterator::iterator_category iterator_category;
    typedef typename base_iterator::pointer pointer;
    typedef typename base_iterator::reference reference;
    typedef typename base_iterator::value_type value_type;

    /**
     * Constructor
     * @param ptr a pointer to the initial element
     */
    Iterator(pointer ptr = nullptr)
    : base_iterator(ptr)
    {

    }

    /**
     * Copy constructor
     */
    Iterator(const iterator &it)
    : base_iterator(it)
    {

    }

    /**
     * Conversion to constant iterator
     */
    template<typename U = typename std::remove_const<T>::type,
             typename std::enable_if<std::is_same<T, const U>::value, int>::type = 0>
    Iterator(const Iterator<U, reverse, std::bidirectional_iterator_tag, categories ...> &it)
    : base_iterator(it)
    {

    }

    /**
     * Destructor
     */
    virtual ~Iterator(void) override
    {

    }

    /**
     * Copy assignment operator
     */
    iterator &operator = (const iterator &it)
    {
        base_iterator::operator = (it);

        return *this;
    }

    /**
     * Prefix decrement operator
     */
    inline virtual derived_iterator &operator -- (void)
    {
        if (reverse)
            this->m_ptr++;
        else
            this->m_ptr--;

        return *this;
    }

    /**
     * Suffix decrement operator
     */
    inline virtual derived_iterator operator -- (int)
    {
        derived_iterator it(*this);

        operator -- ();

        return it;
    }
};

}

#endif
