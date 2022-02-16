#include "binary_functors.h"
#include "expression_functor.h"
#include "expression_evaluator.h"
#include "expression_tree.h"
#include "testExpressionTree.h"
#include "unary_functors.h"
#include "unitTestManager.h"
#include <iostream>
#include <numeric>
#include <string.h>

// using namespace declarations
using namespace attributes::abstract;
using namespace functional;
using namespace messaging;
using namespace utilities;
using namespace utilities::expression;

// type alias declarations
using ErrorType = ExpressionEvaluator<double>::ErrorType;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testExpressionTree",
                                                    &ExpressionTreeUnitTest::create);

/**
 * Defines an expression function object representing a logical "or" operation
 */
struct logical_or
: public ExpressionFunctor<double>
{
    /**
     * Default constructor
     */
    logical_or(void)
    : ExpressionFunctor<double>("logical_or")
    {
        m_result.resize(1);
    }

    /**
     * Function call operator
     */
    inline Any operator () (std::vector<Any> &arguments) override
    {
        double lhs = any_cast<double>(arguments[0]);
        double rhs = any_cast<double>(arguments[1]);
        m_result[0] = lhs || rhs;

        return std::ref(m_result[0]);
    }

    /**
     * clone() function
     */
    inline logical_or *clone(void) const override
    {
        return new logical_or(*this);
    }

    /**
     * create() function
     */
    inline static logical_or *create(void)
    {
        return new logical_or();
    }
};

/**
 * Defines an expression function object representing a multiplication operation
 */
struct multiply
: public ExpressionFunctor<double>
{
    /**
     * Default constructor
     */
    multiply(void)
    : ExpressionFunctor<double>("multiply")
    {
        m_result.resize(1);
    }

    /**
     * Function call operator
     */
    inline Any operator () (std::vector<Any> &arguments) override
    {
        double lhs = any_cast<double>(arguments[0]);
        double rhs = any_cast<double>(arguments[1]);
        m_result[0] = lhs * rhs;

        return std::ref(m_result[0]);
    }

    /**
     * clone() function
     */
    inline multiply *clone(void) const override
    {
        return new multiply(*this);
    }

    /**
     * create() function
     */
    inline static multiply *create(void)
    {
        return new multiply();
    }
};

/**
 * Defines an expression function object representing a summation operation
 */
struct sum
: public ExpressionFunctor<double>
{
    /**
     * Default constructor
     */
    sum(void)
    : ExpressionFunctor<double>("sum")
    {
        m_result.resize(1);
    }

    /**
     * Function call operator
     */
    inline Any operator () (std::vector<Any> &arguments) override
    {
        auto &&v = any_cast<const std::vector<double> &>(arguments[0]);
        m_result[0] = std::accumulate(v.begin(), v.end(), 0.0);

        return std::ref(m_result[0]);
    }

    /**
     * clone() function
     */
    inline sum *clone(void) const override
    {
        return new sum(*this);
    }

    /**
     * create() function
     */
    inline static sum *create(void)
    {
        return new sum();
    }
};

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
ExpressionTreeUnitTest::ExpressionTreeUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
ExpressionTreeUnitTest *ExpressionTreeUnitTest::create(UnitTestManager *pUnitTestManager)
{
    ExpressionTreeUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new ExpressionTreeUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool ExpressionTreeUnitTest::execute(void)
{
    // register factories...
    FactoryConstructible<ExpressionFunctor<double>>::registerFactory("logical_or", logical_or::create);
    FactoryConstructible<ExpressionFunctor<double>>::registerFactory("multiply", multiply::create);
    FactoryConstructible<ExpressionFunctor<double>>::registerFactory("sum", sum::create);

    std::cout << "Starting unit test for ExpressionEvaluator..." << std::endl << std::endl;

    ExpressionEvaluator<double> evaluator;

    double a = 4, b = 5, c = 2, d = 3, e = 7, f = 10, g = 6, h = 1, i = 8, j = 0, k = 3, l = 5, m = 1, n = 3;
    VariableRegistry variables = { { "a", a }, { "b", b }, { "c", c }, { "d", d }, { "e", e },
                                   { "f", f }, { "g", g }, { "h", h }, { "i", i }, { "j", j },
                                   { "k", k }, { "l", l }, { "m", m }, { "n", n } };

    evaluator.setVariableRegistry(variables);

    // some simple expressions
    double result;
    evaluator.evaluate("1234", result);
    bool bSuccess = (result == 1234) && (evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("1+c*3", result);
    bSuccess &= (result == 7) && (evaluator.getErrorType() == ErrorType::None);

    // parenthesis
    evaluator.evaluate("b*(a+a+1)", result);
    bSuccess &= (result == 45) && (evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("b*(c*(1+3)+1)", result);
    bSuccess &= (result == 45) && (evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("!(b*((1+30%a*c)*c+1)>=55) || a==1+c*3/c+1", result);
    bSuccess &= (result == 0) && (evaluator.getErrorType() == ErrorType::None);

    // spaces
    evaluator.evaluate("b * ((1 + 3) * c + 1) << 1", result);
    bSuccess &= (result == 90 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("b - c * ( 3 )", result);
    bSuccess &= (result == -1 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("b - c * ( ( 9 % b)  - 1 )", result);
    bSuccess &= (result == -1 && evaluator.getErrorType() == ErrorType::None);

    // sign before parenthesis
    evaluator.evaluate("-(c+1)*a", result);
    bSuccess &= (result == -12 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("-a*(c+1)", result);
    bSuccess &= (result == -12 && evaluator.getErrorType() == ErrorType::None);

    // fractional numbers
    evaluator.evaluate("1.5/b", result);
    bSuccess &= (result == 0.3 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("1/5e10", result);
    bSuccess &= (result == 2e-11 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("(a-3)/(a*a)", result);
    bSuccess &= (result == 0.0625 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("1/c/c", result);
    bSuccess &= (result == 0.25 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("0.25 * .5 * 0.5", result);
    bSuccess &= (result == 0.0625 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate(".25 / c * .5", result);
    bSuccess &= (result == 0.0625 && evaluator.getErrorType() == ErrorType::None);

    // repeated operators
    evaluator.evaluate("1+-c", result);
    bSuccess &= (result == -1 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("--c", result);
    bSuccess &= (result == 2 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("c---c", result);
    bSuccess &= (result == 0 && evaluator.getErrorType() == ErrorType::None);
    evaluator.evaluate("c-+-c", result);
    bSuccess &= (result == 4 && evaluator.getErrorType() == ErrorType::None);

    // === Errors ===
    // parenthesis error
    evaluator.evaluate("b*((1+3)*c+1", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Parenthesis &&
                 strcmp(evaluator.getErrorPosition(), "") == 0);
    evaluator.evaluate("b*((1+3)*c)+1)", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Parenthesis &&
                 strcmp(evaluator.getErrorPosition(), ")") == 0);

    // repeated operators (wrong)
    evaluator.evaluate("b*/c", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "/c") == 0);

    // wrong position of an operator
    evaluator.evaluate("*c", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "*c") == 0);
    evaluator.evaluate("c+", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "") == 0);
    evaluator.evaluate("c*", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "") == 0);
#if 0
    // division by zero
    evaluator.evaluate("c/0", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::DivideByZero &&
                 strcmp(evaluator.getErrorPosition(), "/0") == 0);
    evaluator.evaluate("3+1/(b-b)+a", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::DivideByZero &&
                 strcmp(evaluator.getErrorPosition(), "/(b-b)+a") == 0);
#endif
    evaluator.evaluate("c/", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "") == 0);

    // invalid characters
    evaluator.evaluate("$5", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "$5") == 0);
    evaluator.evaluate("5x", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "x") == 0);

    // multiply errors
    evaluator.evaluate("3+1/0+4$", result); // only one error will be detected (in this case, the first
                                            // encountered)
#if 0
    bSuccess &= (evaluator.getErrorType() == ErrorType::DivideByZero &&
                 strcmp(evaluator.getErrorPosition(), "/0+4$") == 0);
    evaluator.evaluate("3+1/0+a", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::DivideByZero &&
                 strcmp(evaluator.getErrorPosition(), "/0+a") == 0);
#endif
    evaluator.evaluate("q+1/0)", result); // ...or the first one
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "q+1/0)") == 0);
    evaluator.evaluate("+1/3)", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Parenthesis &&
                 strcmp(evaluator.getErrorPosition(), ")") == 0);
    evaluator.evaluate("+1/0", result);
#if 0
    bSuccess &= (evaluator.getErrorType() == ErrorType::DivideByZero &&
                 strcmp(evaluator.getErrorPosition(), "/0") == 0);
#endif
    // an empty string
    evaluator.evaluate("", result);
    bSuccess &= (evaluator.getErrorType() == ErrorType::Invalid &&
                 strcmp(evaluator.getErrorPosition(), "") == 0);

    std::vector<double> array = { 1, 2, 3, 4, 5 };
    variables["array"] = array;

    // now build expression trees using the ExpressionTree class and evaluate them
    ExpressionTree<double> expressionTree;
    expressionTree.setVariableRegistry(variables);

    bSuccess &= expressionTree.build("(b * (multiply(h + 3, c) + 1) << 1) + sum(array)");

    ExpressionTree<double> *pExpressionTree = expressionTree.clone();

    bSuccess &= pExpressionTree->evaluate(result);
    bSuccess &= (result == 105);

    bSuccess &= expressionTree.build("logical_or(!(b*((1+30%a*c)*c+1)>=55), logical_or(a==1+c*3/c+1,!j))");

    delete pExpressionTree;
    pExpressionTree = expressionTree.clone();

    bSuccess &= expressionTree.evaluate(result);
    bSuccess &= (result == 1);

    bSuccess &= expressionTree.build("!(a*((b+c%d*e)*f+g)>=h) || i==j+k*l/m+n");

    delete pExpressionTree;
    pExpressionTree = expressionTree.clone();

    bSuccess &= expressionTree.evaluate(result);
    bSuccess &= (result == 0);

    delete pExpressionTree;

    if (bSuccess)
        std::cout << "Test PASSED, all expressions successfully evaluated.";
    else
        std::cout << "Test FALIED.";

    std::cout << std::endl << std::endl;

    // remove factories...
    FactoryConstructible<ExpressionFunctor<double>>::removeFactory("logical_or");
    FactoryConstructible<ExpressionFunctor<double>>::removeFactory("multiply");
    FactoryConstructible<ExpressionFunctor<double>>::removeFactory("sum");

    return bSuccess;
}

}
