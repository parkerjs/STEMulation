#ifndef COMPARE_GREATER_ATTRIBUTE_H
#define COMPARE_GREATER_ATTRIBUTE_H

#include <type_traits>
#include <utility>

namespace attributes
{

namespace operators
{

namespace comparison
{

/**
 * Base class for compare greater operator overloads
 */
class BaseCompareGreaterAttribute { };

/**
 * Objects derived from this class inherit compare greater operator overloads
 */
template<typename T>
class CompareGreaterAttribute
: public BaseCompareGreaterAttribute
{
public:

    /**
     * Compare greater operator
     */
    template<typename U, typename V>
    inline friend typename std::enable_if<std::is_base_of<BaseCompareGreaterAttribute, typename
                                          std::remove_reference<U>::type>::value &&
                                          std::is_same<typename std::decay<U>::type, T>::value, bool>::type
    operator > (U &&lhs, V &&rhs)
    {
        return std::forward<U>(lhs).operator > (std::forward<V>(rhs));
    }

    /**
     * Compare greater operator
     */
    template<typename U, typename V>
    inline friend typename std::enable_if<std::is_base_of<BaseCompareGreaterAttribute, typename
                                          std::remove_reference<V>::type>::value &&
                                         !std::is_base_of<BaseCompareGreaterAttribute, typename
                                          std::remove_reference<U>::type>::value &&
                                          std::is_same<typename std::decay<V>::type, T>::value &&
                                         !std::is_same<typename std::decay<U>::type, T>::value, bool>::type
    operator > (U &&lhs, V &&rhs)
    {
        return !rhs.operator == (std::forward<U>(lhs)) && !rhs.operator > (std::forward<U>(lhs));
    }
};

}

}

}

#endif
