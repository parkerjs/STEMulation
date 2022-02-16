#ifndef COMPARE_LESS_ATTRIBUTE_H
#define COMPARE_LESS_ATTRIBUTE_H

#include <type_traits>
#include <utility>

namespace attributes
{

namespace operators
{

namespace comparison
{

/**
 * Base class for compare less operator overloads
 */
class BaseCompareLessAttribute { };

/**
 * Objects derived from this class inherit compare less operator overloads
 */
template<typename T>
class CompareLessAttribute
: public BaseCompareLessAttribute
{
public:

    /**
     * Compare less operator
     */
    template<typename U, typename V>
    inline friend typename std::enable_if<std::is_base_of<BaseCompareLessAttribute, typename
                                          std::remove_reference<U>::type>::value &&
                                          std::is_same<typename std::decay<U>::type, T>::value, bool>::type
    operator < (U &&lhs, V &&rhs)
    {
        return !std::forward<U>(lhs).operator == (std::forward<V>(rhs)) &&
               !std::forward<U>(lhs).operator > (std::forward<V>(rhs));
    }

    /**
     * Compare less operator
     */
    template<typename U, typename V>
    inline friend typename std::enable_if<std::is_base_of<BaseCompareLessAttribute, typename
                                          std::remove_reference<V>::type>::value &&
                                         !std::is_base_of<BaseCompareLessAttribute, typename
                                          std::remove_reference<U>::type>::value &&
                                          std::is_same<typename std::decay<V>::type, T>::value &&
                                         !std::is_same<typename std::decay<U>::type, T>::value, bool>::type
    operator < (U &&lhs, V &&rhs)
    {
        return !(rhs.operator == (std::forward<U>(lhs))) && !(std::forward<U>(lhs) > (rhs));
    }
};

}

}

}

#endif
