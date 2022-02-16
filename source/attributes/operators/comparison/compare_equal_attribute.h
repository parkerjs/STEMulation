#ifndef COMPARE_EQUAL_ATTRIBUTE_H
#define COMPARE_EQUAL_ATTRIBUTE_H

#include <type_traits>
#include <utility>

namespace attributes
{

namespace operators
{

namespace comparison
{

/**
 * Base class for compare equal operator overloads
 */
class BaseCompareEqualAttribute { };

/**
 * Objects derived from this class inherit compare equal operator overloads
 */
template<typename T>
class CompareEqualAttribute
: public BaseCompareEqualAttribute
{
public:

    /**
     * Compare equal operator
     */
    template<typename U, typename V>
    inline friend typename std::enable_if<std::is_base_of<BaseCompareEqualAttribute, typename
                                          std::remove_reference<U>::type>::value &&
                                          std::is_same<typename std::decay<U>::type, T>::value, bool>::type
    operator == (U &&lhs, V &&rhs)
    {
        return std::forward<U>(lhs).operator == (std::forward<V>(rhs));
    }

    /**
     * Compare equal operator
     */
    template<typename U, typename V>
    inline friend typename std::enable_if<std::is_base_of<BaseCompareEqualAttribute, typename
                                          std::remove_reference<V>::type>::value &&
                                         !std::is_base_of<BaseCompareEqualAttribute, typename
                                          std::remove_reference<U>::type>::value &&
                                          std::is_same<typename std::decay<V>::type, T>::value &&
                                         !std::is_same<typename std::decay<U>::type, T>::value, bool>::type
    operator == (U &&lhs, V &&rhs)
    {
        return rhs.operator == (std::forward<U>(lhs));
    }
};

}

}

}

#endif
