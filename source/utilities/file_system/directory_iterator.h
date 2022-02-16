#ifndef DIRECTORY_ITERATOR_H
#define DIRECTORY_ITERATOR_H

#include "input_iterator.h"
#include <functional>
#include <stack>
#include <string>

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define POSIX
#endif

#ifdef _WIN32
#include <windows.h>
#elif defined POSIX
#include <dirent.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4503) // warning about decorated name length exceeded and truncated
#endif

namespace iterators
{

#ifdef _WIN32
typedef HANDLE handle_type;
typedef std::wstring string_type;
#elif defined POSIX
typedef DIR * handle_type;
typedef std::string string_type;
#endif

// forward declarations
struct directory_iterator_tag;

/**
 * This specialization implements a custom directory iterator for the DirectoryTraverser class
 */
template<typename T, bool reverse, typename ... categories>
class Iterator<T, reverse, directory_iterator_tag, categories ...>
: public Iterator<T, reverse, std::input_iterator_tag, directory_iterator_tag, categories ...>
{
public:

    /**
     * Typedef declarations
     */
    typedef Iterator<T, reverse, std::input_iterator_tag, directory_iterator_tag,
                     categories ...> base_iterator;
    typedef typename std::tuple_element<sizeof ... (categories),
                                        std::tuple<directory_iterator_tag, categories ...>>::type category;
    typedef Iterator<const T, reverse, directory_iterator_tag, categories ...> const_iterator;
    typedef typename std::decay<T>::type decay_type;
    typedef Iterator<T, reverse, category> derived_iterator;
    typedef typename base_iterator::difference_type difference_type;
    typedef Iterator<T, reverse, directory_iterator_tag, categories ...> iterator;
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
     */
    Iterator(void)
    : base_iterator(nullptr),
      m_fileFunctor([] (decay_type &, std::stack<string_type> &, std::stack<handle_type> &) { return false; })
    {

    }

    /**
     * Constructor
     * @param path        the directory under which files will be iterated
     * @param fileFunctor a function object that is responsible for finding the next file
     */
    template<typename Functor>
    Iterator(const string_type &path,
             Functor &&fileFunctor)
    : base_iterator(nullptr),
      m_fileFunctor(std::forward<Functor>(fileFunctor))
    {
        m_folders.push(path);
        m_handles.push(nullptr);
    }

    /**
     * Copy constructor
     */
    Iterator(const iterator &it)
    : base_iterator(it),
      m_file(it.m_file),
      m_fileFunctor(it.m_fileFunctor),
      m_folders(it.m_folders),
      m_handles(it.m_handles)
    {
        operator = (it);
    }

    /**
     * Conversion to constant iterator
     */
    template<typename U = typename std::remove_const<T>::type,
             typename std::enable_if<std::is_same<T, const U>::value, int>::type = 0>
    Iterator(const Iterator<U, reverse, directory_iterator_tag, categories ...> &it)
    : base_iterator(it),
      m_file(it.m_file),
      m_fileFunctor(it.m_fileFunctor),
      m_folders(it.m_folders),
      m_handles(it.m_handles)
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

            m_file = it.m_file;
            m_fileFunctor = it.m_fileFunctor;
            m_folders = it.m_folders;
            m_handles = it.m_handles;
        }

        return *this;
    }

    /**
     * Indirection operator
     */
    inline virtual reference operator * (void) final
    {
        return m_file;
    }

    /**
     * Indirection operator
     */
    inline virtual value_type operator * (void) const override
    {
        return m_file;
    }

    /**
     * Structure dereference operator
     */
    inline virtual pointer operator -> (void) const override
    {
        return const_cast<pointer>(&m_file);
    }

    /**
     * Prefix increment operator
     */
    inline virtual iterator &operator ++ (void) override
    {
        m_fileFunctor(m_file, m_folders, m_handles);
        if (m_folders.empty())
            m_file.clear();

        return *this;
    }

    /**
     * Compare equal operator
     */
    inline virtual bool operator == (const derived_iterator &it) const override
    {
        return m_file == static_cast<const iterator &>(it).m_file;
    }

    /**
     * swap function
     */
    inline virtual void swap(derived_iterator &it) override
    {
        base_iterator::swap(it);

        std::swap(m_file, it.m_file);
        std::swap(m_folders, it.m_folders);
        std::swap(m_handles, it.m_handles);
        std::swap(m_fileFunctor, it.m_fileFunctor);
    }

private:

    /**
     * the current file
     */
    std::string m_file;

    /**
     * function object that is responsible for finding the next file
     */
    std::function<bool (decay_type &, std::stack<string_type> &, std::stack<handle_type> &)> m_fileFunctor;

    /**
     * a stack of folders which help to track the directory depth level of the current search
     */
    std::stack<string_type> m_folders;

    /**
     * a stack of handles which correspond to the stack of folders
     */
    std::stack<handle_type> m_handles;
};

}

#endif
