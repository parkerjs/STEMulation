#ifndef LAMBDA_HELPER_H
#define LAMBDA_HELPER_H

#include <utility>

namespace functional
{

/**
 * Helper structure to facilitate lambda function overloading
 */
template<typename Function, typename ... Functions>
struct lambda_overload
: public Function,
  public lambda_overload<Functions ...>
{
    /**
     * Using declarations
     */
    using Function::operator ();
    using lambda_overload<Functions ...>::operator ();

    /**
     * Constructor
     */
    lambda_overload(Function &&function, Functions && ... functions)
    : Function(std::move(function)),
      lambda_overload<Functions ...>(std::move(functions) ...)
    {

    }
};

/**
 * Helper structure to facilitate lambda function overloading
 */
template<class Function>
struct lambda_overload<Function>
: public Function
{
    /**
     * Using declarations
     */
    using Function::operator ();

    /**
     * Constructor
     */
    lambda_overload(Function &&function)
    : Function(std::move(function))
    {

    }
};

/**
 * Helper function to make lambda overloads
 */
auto const make_overloaded_lambda = [] (auto ... functions)
{
    return lambda_overload<decltype(functions) ...> { std::move(functions) ... };
};

/**
 * Helper structure to facilitate recursive lambdas
 */
template<class Function>
struct recursive_lambda
{
    /**
     * Function call operator
     */
    template<typename ... Args>
    auto operator () (Args && ... args) const
    {
        return m_function(*this, std::forward<Args>(args) ...);
    }

    /**
     * an instance of the function
     */
    Function m_function;
};

/**
 * Helper function to make recursive lambdas
 */
auto const make_recursive_lambda = [] (auto function)
{
    return recursive_lambda<decltype(function)>{ std::move(function) };
};

/**
 * Function to make both recursive and overloaded lambdas
 */
auto const make_recursive_overloaded_lambda = [] (auto && ... functions)
{
    return make_recursive_lambda(make_overloaded_lambda(functions ...));
};

}

#endif
