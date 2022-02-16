#ifndef VARIABLE_WRAPPER_H
#define VARIABLE_WRAPPER_H

#include "any.h"
#include "arithmetic_attributes.h"
#include "bitwise_attributes.h"
#include "comparable.h"
#include "has_arithmetic_precision_loss.h"
#include "has_equal.h"
#include "has_greater.h"

#include <iostream>
#include <string>
#include <utility>

namespace functional
{

/**
 * This class can be used to wrap objects, object references, pod types and references, or pointers to
 * objects/pod types. All binary operator overloads are inherited and can be invoked on a VariableWrapper
 * object as long as the raw type supports the appropriate compound assignment operator associated with the
 * desired binary operation (+=, -=, *=, /=, %=, &= |=, <<=, >>=, ^=, ==, <=, >=, <, >, !=)
 */
template<typename T>
class VariableWrapper final
: public attributes::operators::arithmetic::ArithmeticAttributes<VariableWrapper<T>>,
  public attributes::operators::bitwise::BitwiseAttributes<VariableWrapper<T>>,
  public attributes::operators::comparison::Comparable<VariableWrapper<T>>
{
public:

    /**
     * Typedef declarations
     */
    typedef typename std::remove_reference<T>::type type;

    /**
     * Constructor
     */
    explicit VariableWrapper(void)
    : m_bHeapAllocated(true),
      m_pVariable(new type)
    {

    }

    /**
     * Constructor
     */
    template<typename U = type, typename std::enable_if<!std::is_same<U &, T>::value, int>::type = 0>
    VariableWrapper(const U &variable)
    : m_bHeapAllocated(true),
      m_pVariable(new type(variable))
    {

    }

    /**
     * Constructor for reference variables (reference-wrapping enabled only upon new object construction
     * (references cannot be wrapped upon copy construction)
     */
    template<typename U = typename std::decay<T>::type,
             typename std::enable_if<std::is_same<U &, T>::value, int>::type = 0>
    VariableWrapper(U &variable)
    : m_bHeapAllocated(false),
      m_pVariable(&variable)
    {

    }

    /**
     * Copy constructor
     */
    VariableWrapper(const VariableWrapper<T> &variable)
    : m_bHeapAllocated(false),
      m_pVariable(nullptr)
    {
        operator = (variable);
    }

    /**
     * Move constructor
     */
    VariableWrapper(VariableWrapper<T> &&variable)
    : m_bHeapAllocated(false),
      m_pVariable(nullptr)
    {
        operator = (std::move(variable));
    }

    /**
     * Destructor
     */
    virtual ~VariableWrapper(void) override
    {
        if (m_bHeapAllocated)
            delete m_pVariable;

        m_pVariable = nullptr;
    }

    /**
     * Conversion operator
     */
    template<typename U = T, typename std::enable_if<!std::is_reference<U>::value, int>::type = 0>
    inline operator U & (void)
    {
        return *m_pVariable;
    }

    /**
     * Conversion operator
     */
    inline operator T (void)
    {
        return *m_pVariable;
    }

    /**
     * Conversion operator
     */
    inline operator T (void) const
    {
        return *m_pVariable;
    }

    /**
     * Copy assignment operator
     */
    VariableWrapper<T> &operator = (const VariableWrapper<T> &variable)
    {
        if (&variable != this)
        {
            m_pVariable = new type(*variable.m_pVariable);
            m_bHeapAllocated = true;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    VariableWrapper<T> &operator = (VariableWrapper<T> &&variable)
    {
        if (m_bHeapAllocated)
            delete m_pVariable;

        m_bHeapAllocated = std::move(variable.m_bHeapAllocated);
        m_pVariable = std::move(variable.m_pVariable);
        variable.m_pVariable = nullptr;
        variable.m_bHeapAllocated = false;

        return *this;
    }

    /**
     * Assignment operator
     */
    template<typename U>
    VariableWrapper<T> &operator = (U &&variable)
    {
        *m_pVariable = std::forward<U>(variable);

        return *this;
    }

    /**
     * Unary plus operator
     */
    inline type operator + (void) const
    {
        return +*m_pVariable;
    }

    /**
     * Unary minus operator
     */
    inline type operator - (void) const
    {
        return -*m_pVariable;
    }

    /**
     * Prefix increment operator
     */
    inline T &operator ++ (void)
    {
        return ++(*m_pVariable);
    }

    /**
     * Prefix decrement operator
     */
    inline T &operator -- (void)
    {
        return --(*m_pVariable);
    }

    /**
     * Equality operator
     */
    template<typename U> inline typename
    std::enable_if<traits::tests::operators::comparison::has_compare_equal_operator<T, U>::value, bool>::type
    operator == (U &&variable) const
    {
        return *m_pVariable == std::forward<U>(variable);
    }

    /**
     * Equality operator
     */
    template<typename U> inline typename
    std::enable_if<!traits::tests::operators::comparison::has_compare_equal_operator<T, U>::value, bool>::type
    operator == (U &&variable) const
    {
        return false;
    }

    /**
     * Greater than operator
     */
    template<typename U> inline typename
    std::enable_if<traits::tests::operators::comparison::has_compare_greater_operator<T, U>::value, bool>::type
    operator > (U &&variable) const
    {
        return *m_pVariable > std::forward<U>(variable);
    }

    /**
     * Greater than operator
     */
    template<typename U> inline typename
    std::enable_if<!traits::tests::operators::comparison::has_compare_greater_operator<T, U>::value, bool>::type
    operator > (U &&variable) const
    {
        return false;
    }

    /**
     * Logical negation operator
     */
    inline T operator ! (void) const
    {
        return !(*m_pVariable);
    }

    /**
     * Bitwise NOT operator
     */
    inline T operator ~ () const
    {
        return ~*m_pVariable;
    }

    /**
     * Compound operator definitions
     */
    #define CompoundOperator(Symbol)                                                                         \
    template<typename U> inline typename                                                                     \
    std::enable_if<!traits::tests::has_arithmetic_precision_loss<T, U>::value, VariableWrapper<T> &>::type   \
    operator Symbol##= (U && variable)                                                                       \
    {                                                                                                        \
        *m_pVariable Symbol##= std::forward<U>(variable);                                                    \
                                                                                                             \
        return *this;                                                                                        \
    }                                                                                                        \
                                                                                                             \
    template<typename U> inline typename                                                                     \
    std::enable_if<traits::tests::has_arithmetic_precision_loss<T, U>::value, VariableWrapper<T> &>::type    \
    operator Symbol##= (U &&variable)                                                                        \
    {                                                                                                        \
        *m_pVariable = typename std::decay<T>::type(                                                         \
                       typename std::decay<U>::type(*m_pVariable) Symbol std::forward<U>(variable));         \
                                                                                                             \
        return *this;                                                                                        \
    }                                                                                                        \
                                                                                                             \
    template<typename U> inline friend typename                                                              \
    std::enable_if<!traits::tests::has_arithmetic_precision_loss<U, T>::value, U &>::type                    \
    operator Symbol##= (U &&lhs, const VariableWrapper<T> &variable)                                         \
    {                                                                                                        \
        return lhs Symbol##= *variable.m_pVariable;                                                          \
    }                                                                                                        \
                                                                                                             \
    template<typename U> inline friend typename                                                              \
    std::enable_if<traits::tests::has_arithmetic_precision_loss<U, T>::value, U &>::type                     \
    operator Symbol##= (U &&lhs, const VariableWrapper<T> &variable)                                         \
    {                                                                                                        \
        lhs = typename std::decay<U>::type(                                                                  \
              typename std::decay<T>::type(std::forward<U>(lhs)) Symbol *variable.m_pVariable);              \
                                                                                                             \
        return lhs;                                                                                          \
    }

    /**
     * Addition assignment operator
     */
    CompoundOperator(+)

    /**
     * Subtraction assignment operator
     */
    CompoundOperator(-)

    /**
     * Multiplication assignment operator
     */
    CompoundOperator(*)

    /**
     * Division assignment operator
     */
    CompoundOperator(/)

    /**
     * Modulo assignment operator
     */
    CompoundOperator(%)

    /**
     * Bitwise AND assignment operator
     */
    CompoundOperator(&)

    /**
     * Bitwise OR assignment operator
     */
    CompoundOperator(|)

    /**
     * Bitwise XOR assignment operator
     */
    CompoundOperator(^)

    /**
     * Bitwise left shift assignment operator
     */
    CompoundOperator(<<)

    /**
     * std::ostream operator
     */
    inline friend std::ostream &operator << (std::ostream &stream, const VariableWrapper<T> &variable)
    {
        return stream << *variable.m_pVariable;
    }

    /**
     * Bitwise right shift assignment operator
     */
    CompoundOperator(>>)

    /**
     * std::ostream operator
     */
    inline friend std::ostream &operator >> (std::ostream &stream, VariableWrapper<T> &variable)
    {
        return stream >> *variable.m_pVariable;
    }

protected:

    /**
     * a flag to indicate that the variable was heap-allocated
     */
    bool m_bHeapAllocated;

    /**
     * a pointer to the variable wrapped within this object
     */
    type *m_pVariable;
};

}

#endif
