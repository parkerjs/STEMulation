#ifndef ITERATOR_TRAITS_H
#define ITERATOR_TRAITS_H

#include <functional>
#include <iterator>

namespace traits
{

namespace helpers
{

// forward declarations
template<typename> struct iterator_traits;

/**
 * Helper class to infer iterator traits from the template argument (specialization for std::iterator types)
 */
template<typename category, typename T, typename distance, typename ptr, typename ref>
struct iterator_traits<std::iterator<category, T, distance, ptr, ref>> final
{
    typedef typename std::iterator<category, T, distance, ptr, ref>::difference_type difference_type;
    typedef typename std::iterator<category, T, distance, ptr, ref>::iterator_category iterator_category;
    typedef typename std::iterator<category, T, distance, ptr, ref>::pointer pointer;
    typedef typename std::iterator<category, T, distance, ptr, ref>::reference reference;
    typedef typename std::iterator<category, T, distance, ptr, ref>::value_type value_type;
};

/**
 * Helper class to infer iterator traits from the template argument (specialization for
 * containers::iterators::Iterator types)
 */
template<typename T, bool reverse, template<typename, bool, typename ...> class Iterator,
         typename ... categories>
struct iterator_traits<Iterator<T, reverse, categories ...>> final
{
    typedef typename std::iterator<category, T>::difference_type difference_type;
    typedef typename std::iterator<category, T>::iterator_category iterator_category;
    typedef typename std::iterator<category, T>::pointer pointer;
    typedef typename std::iterator<category, T>::reference reference;
    typedef typename std::iterator<category, T>::value_type value_type;
};

}

}

#endif
