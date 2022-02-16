#ifndef TOKEN_ITERATOR_H
#define TOKEN_ITERATOR_H

#include "input_iterator.h"
#include <cstring>
#include <functional>

namespace iterators
{

// forward declarations
struct token_iterator_tag;

/**
 * This specialization implements a custom token iterator for the Tokenizer class
 */
template<typename T, typename ... categories>
class Iterator<T, false, token_iterator_tag, categories ...>
: public Iterator<T, false, std::input_iterator_tag, token_iterator_tag, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef Iterator<T, false, std::input_iterator_tag, token_iterator_tag, categories ...> base_iterator;
    typedef typename std::tuple_element<sizeof ... (categories),
                                        std::tuple<token_iterator_tag, categories ...>>::type category;
    typedef Iterator<const T, false, token_iterator_tag, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, false, category> derived_iterator;
    typedef typename base_iterator::difference_type difference_type;
    typedef Iterator<T, false, token_iterator_tag, categories ...> iterator;
    typedef typename base_iterator::iterator_category iterator_category;
    typedef typename base_iterator::pointer pointer;
    typedef typename base_iterator::reference reference;
    typedef typename base_iterator::value_type value_type;

    /**
     * Friend declarations
     */
    friend const_iterator;

    /**
     * Using declarations
     */
    using base_iterator::operator ++;

    /**
     * Constructor
     * @param string a reference to the string to be tokenized
     */
    Iterator(const reference string)
    : base_iterator(nullptr),
      m_pBuffer(string.c_str() + string.size()),
      m_pLastBufferPosition(m_pBuffer),
      m_tokenFunctor([] (const char *&, decay_type &) { return -1; })
    {

    }

    /**
     * Constructor
     * @param string       a reference to the string to be tokenized
     * @param tokenFunctor a function object responsible for tokenizing the string
     */
    template<typename Functor>
    Iterator(const reference &string, Functor &&tokenFunctor)
    : base_iterator(nullptr),
      m_pBuffer(string.c_str()),
      m_pLastBufferPosition(m_pBuffer),
      m_tokenFunctor(std::forward<Functor>(tokenFunctor))
    {

    }

    /**
     * Copy constructor
     */
    Iterator(const iterator &it)
    : base_iterator(it),
      m_pBuffer(it.m_pBuffer),
      m_pLastBufferPosition(it.m_pLastBufferPosition),
      m_token(it.m_token),
      m_tokenFunctor(it.m_tokenFunctor)
    {
        operator = (it);
    }

    /**
     * Conversion to constant iterator
     */
    template<typename U = typename std::remove_const<T>::type,
             typename std::enable_if<std::is_same<T, const U>::value, int>::type = 0>
    Iterator(const Iterator<U, false, token_iterator_tag, categories ...> &it)
    : base_iterator(it),
      m_pBuffer(it.m_pBuffer),
      m_pLastBufferPosition(it.m_pLastBufferPosition),
      m_token(it.m_token),
      m_tokenFunctor(it.m_tokenFunctor)
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
        if (&it != this)
        {
            base_iterator::operator = (it);

            m_pBuffer = it.m_pBuffer;
            m_token = it.m_token;
            m_tokenFunctor = it.m_tokenFunctor;
        }

        return *this;
    }

    /**
     * Indirection operator
     */
    inline virtual reference operator * (void) final
    {
        return m_token;
    }

    /**
     * Indirection operator
     */
    inline virtual value_type operator * (void) const override
    {
        return m_token;
    }

    /**
     * Structure dereference operator
     */
    inline virtual pointer operator -> (void) const override
    {
        return const_cast<pointer>(&m_token);
    }

    /**
     * Prefix increment operator
     */
    inline virtual iterator &operator ++ (void) override
    {
        m_pBuffer = m_pLastBufferPosition;

        decay_type token;
        int result = 0;
        do
        {
            result = m_tokenFunctor(m_pLastBufferPosition, token);
            if (result == 0)
                m_token = std::move(token);
        }
        while (result > 0);

        if (result < 0)
            m_pBuffer += std::strlen(m_pBuffer);

        return *this;
    }

    /**
     * Compare equal operator
     */
    inline virtual bool operator == (const iterator &it) const override
    {
        return std::strlen(m_pBuffer) == std::strlen(it.m_pBuffer);
    }

    /**
     * swap function
     */
    inline virtual void swap(iterator &it) override final
    {
        base_iterator::swap(it);

        std::swap(m_pBuffer, it.m_pBuffer);
        std::swap(m_token, it.m_token);
        std::swap(m_tokenFunctor, it.m_tokenFunctor);
    }

private:

    /**
     * a pointer to the current position in the buffer
     */
    const char *m_pBuffer;

    /**
     * a pointer to the previous buffer position
     */
    const char *m_pLastBufferPosition;

    /**
     * the current token extracted from the buffer
     */
    decay_type m_token;

    /**
     * an instance of the function object responsible for tokenizing the string
     */
    std::function<int (const char *&, decay_type &)> m_tokenFunctor;
};

}

#endif
