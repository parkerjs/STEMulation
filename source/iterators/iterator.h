#ifndef ITERATOR_H
#define ITERATOR_H

#include <iterator>
#include <tuple>

namespace iterators
{

// forward declarations
template<typename, bool, typename ... categories> class Iterator;

/**
 * This class serves as a base class for stl-style derived iterators
 */
template<typename T, bool reverse, typename ... categories>
class Iterator<T, reverse, void, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef typename std::tuple_element<sizeof ... (categories), std::tuple<void, categories ...>>::type category;
    typedef Iterator<const T, reverse, void, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, reverse, category> derived_iterator;
    typedef typename std::iterator<category, T>::difference_type difference_type;
    typedef Iterator<T, reverse, void, categories ...> iterator;
    typedef typename std::iterator<category, T>::iterator_category iterator_category;
    typedef typename std::iterator<category, T>::pointer pointer;
    typedef typename std::iterator<category, T>::reference reference;
    typedef typename std::iterator<category, T>::value_type value_type;

    /**
     * Friend declarations
     */
    friend const_iterator;

protected:

    /**
     * Constructor
     * @param ptr a pointer to the initial element
     */
    Iterator(pointer ptr)
    : m_ptr(ptr)
    {

    }

public:

    /**
     * Copy constructor
     */
    Iterator(const iterator &it)
    : m_ptr(it.m_ptr)
    {
        operator = (it);
    }

    /**
     * Conversion to constant iterator
     */
    template<typename U = typename std::remove_const<T>::type,
             typename std::enable_if<std::is_same<T, const U>::value, int>::type = 0>
    Iterator(const Iterator<U, reverse, void, categories ...> &it)
    : m_ptr(it.m_ptr)
    {

    }

    /**
     * Destructor
     */
    virtual ~Iterator(void)
    {

    }

    /**
     * Copy assignment operator
     */
    iterator &operator = (const iterator &it)
    {
        if (&it != this)
        {
            m_ptr = it.m_ptr;
        }

        return *this;
    }

protected:

    /**
     * Conversion to derived_iterator reference
     */
    operator derived_iterator & (void)
    {
        return static_cast<derived_iterator &>(*this);
    }

    /**
     * Conversion to const derived_iterator
     */
    operator derived_iterator (void) const
    {
        return static_cast<const derived_iterator &>(*this);
    }

public:

    /**
     * Prefix increment operator
     */
    inline virtual derived_iterator &operator ++ (void)
    {
        if (reverse)
            m_ptr--;
        else
            m_ptr++;

        return *this;
    }

    /**
     * Suffix increment operator
     */
    inline virtual derived_iterator operator ++ (int)
    {
        derived_iterator it(*this);

        operator ++ ();

        return it;
    }

    /**
     * swap function
     */
    inline virtual void swap(derived_iterator &it)
    {
        std::swap(m_ptr, it.m_ptr);
    }

protected:

    /**
     * a pointer to track the iterator position
     */
    pointer m_ptr;
};

}

#endif
