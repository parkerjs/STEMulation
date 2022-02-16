#ifndef FORWARD_ITERATOR_H
#define FORWARD_ITERATOR_H

#include "input_iterator.h"

namespace iterators
{

/**
 * This specialization implements a generic stl-style forward iterator
 */
template<typename T, bool reverse, typename ... categories>
class Iterator<T, reverse, std::forward_iterator_tag, categories ...>
: public Iterator<T, reverse, std::input_iterator_tag, std::forward_iterator_tag, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef Iterator<T, reverse, std::input_iterator_tag, std::forward_iterator_tag,
                     categories ...> base_iterator;
    typedef typename std::tuple_element<sizeof ... (categories),
                                        std::tuple<std::forward_iterator_tag, categories ...>>::type category;
    typedef Iterator<const T, reverse, std::forward_iterator_tag, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, reverse, category> derived_iterator;
    typedef typename base_iterator::difference_type difference_type;
    typedef Iterator<T, reverse, std::forward_iterator_tag, categories ...> iterator;
    typedef typename base_iterator::iterator_category iterator_category;
    typedef typename base_iterator::pointer pointer;
    typedef typename base_iterator::reference reference;
    typedef typename base_iterator::value_type value_type;

    /**
     * Constructor
     * @param ptr a pointer to the initial element
     */
    template<typename std::enable_if<(std::is_same<category, std::forward_iterator_tag>::value && !reverse) ||
                                     !std::is_same<category, std::forward_iterator_tag>::value, int>::type = 0>
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
    Iterator(const Iterator<U, reverse, std::forward_iterator_tag, categories ...> &it)
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
     * Indirection operator
     */
    inline virtual reference operator * (void)
    {
        return *this->m_ptr;
    }
};

}

#endif
