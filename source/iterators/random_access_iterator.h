#ifndef RANDOM_ACCESS_ITERATOR_H
#define RANDOM_ACCESS_ITERATOR_H

#include "bidirectional_iterator.h"

namespace iterators
{

/**
 * This specialization implements a generic stl-style random-access iterator
 */
template<typename T, bool reverse, typename ... categories>
class Iterator<T, reverse, std::random_access_iterator_tag, categories ...>
: public Iterator<T, reverse, std::bidirectional_iterator_tag, std::random_access_iterator_tag, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef Iterator<T, reverse, std::bidirectional_iterator_tag, std::random_access_iterator_tag,
                     categories ...> base_iterator;
    typedef typename std::tuple_element<sizeof ... (categories), std::tuple<std::random_access_iterator_tag,
                                        categories ...>>::type category;
    typedef Iterator<const T, reverse, std::random_access_iterator_tag, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, reverse, category> derived_iterator;
    typedef typename base_iterator::difference_type difference_type;
    typedef Iterator<T, reverse, std::random_access_iterator_tag, categories ...> iterator;
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
    Iterator(const Iterator<U, reverse, std::random_access_iterator_tag, categories ...> &it)
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
     * Addition operator
     */
    inline friend derived_iterator operator + (int offset, const derived_iterator &it)
    {
        derived_iterator result(it);

        result += offset;

        return result;
    }

    /**
     * Addition operator
     */
    inline friend derived_iterator operator + (const derived_iterator &it, int offset)
    {
        derived_iterator result(it);

        result += offset;

        return result;
    }

    /**
     * Addition assignment operator
     */
    inline virtual derived_iterator &operator += (int offset)
    {
        if (reverse)
            this->m_ptr -= offset;
        else
            this->m_ptr += offset;

        return *this;
    }

    /**
     * Subtraction operator
     */
    inline virtual difference_type operator - (const derived_iterator &it) const
    {
        return std::distance(this->m_ptr, it.m_ptr);
    }

    /**
     * Subtraction operator
     */
    inline friend derived_iterator operator - (const derived_iterator &it, int offset)
    {
        derived_iterator result(it);

        result -= offset;

        return result;
    }

    /**
     * Subtraction assignment operator
     */
    inline virtual derived_iterator &operator -= (int offset)
    {
        if (reverse)
            this->m_ptr += offset;
        else
            this->m_ptr -= offset;

        return *this;
    }

    /**
     * Subscript operator
     */
    inline virtual reference operator [] (int offset)
    {
        return this->m_ptr[reverse ? -offset : offset];
    }

    /**
     * Compare greater operator
     */
    inline virtual bool operator > (const derived_iterator &it) const
    {
        if (reverse)
            return this->m_ptr < it.m_ptr;
        else
            return this->m_ptr > it.m_ptr;
    }

    /**
     * Compare greater equal operator
     */
    inline virtual bool operator >= (const derived_iterator &it) const
    {
        return !operator < (it);
    }

    /**
     * Compare less operator
     */
    inline virtual bool operator < (const derived_iterator &it) const
    {
        if (reverse)
            return this->m_ptr > it.m_ptr;
        else
            return this->m_ptr < it.m_ptr;
    }

    /**
     * Compare less equal operator
     */
    inline virtual bool operator <= (const derived_iterator &it) const
    {
        return !operator > (it);
    }
};

}

#endif
