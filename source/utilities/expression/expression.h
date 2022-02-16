#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "cloneable.h"
#include "dictionary.h"
#include "expression_binary_functor.h"
#include "expression_functor.h"
#include "expression_unary_functor.h"
#include "initializable.h"
#include <functional>
#include <list>

namespace utilities
{

namespace expression
{

/**
 * Base class for classes that implement expression trees and evaluators
 */
template<typename T, typename Result = T>
class Expression
: public attributes::interfaces::Cloneable<Expression<T, Result>>,
  public attributes::interfaces::Initializable,
  public attributes::concrete::VariableMapRegistrable
{
public:

    /**
     * Typedef declarations
     */
    typedef functional::functors::binary::BinaryFunctorType BinaryFunctorType;
    typedef std::map<std::string, std::string> tTokenMap;
    typedef functional::functors::unary::UnaryFunctorType UnaryFunctorType;

    /**
     * Enumerations
     */
    enum class ErrorType { BadFunctionCall, DivideByZero, Invalid, None, NotANumber, Parenthesis };

protected:

    /**
     * Constructor
     */
    Expression(void)
    : m_errorType(ErrorType::None),
      m_parenthesisCount(0),
      m_pErrorPosition(nullptr)
    {

    }

    /**
     * Copy constructor
     */
    Expression(const Expression<T, Result> &expression)
    : m_errorType(ErrorType::None),
      m_parenthesisCount(0),
      m_pErrorPosition(nullptr)
    {
        operator = (expression);
    }

    /**
     * Move constructor
     */
    Expression(Expression<T, Result> &&expression)
    : m_errorType(ErrorType::None),
      m_parenthesisCount(0),
      m_pErrorPosition(nullptr)
    {
        operator = (std::move(expression));
    }

    /**
     * Copy assignment operator
     */
    Expression<T, Result> &operator = (const Expression<T, Result> &expression)
    {
        if (&expression != this)
        {
            m_errorType = expression.m_errorType;
            m_numericOperands = expression.m_numericOperands;
            m_parenthesisCount = expression.m_parenthesisCount;
            m_pErrorPosition = expression.m_pErrorPosition;
            m_registry = expression.m_registry; // TODO: should this be done?
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    Expression<T, Result> &operator = (Expression<T, Result> &&expression)
    {
        if (&expression != this)
        {
            m_errorType = std::move(expression.m_errorType);
            m_numericOperands = std::move(expression.m_numericOperands);
            m_parenthesisCount = std::move(expression.m_parenthesisCount);
            m_pErrorPosition = std::move(expression.m_pErrorPosition);
            expression.m_pErrorPosition = nullptr;

            m_registry = std::move(expression.m_registry); // TODO: should this be done?
        }

        return *this;
    }

public:

    /**
     * Destructor
     */
    virtual ~Expression(void) override
    {

    }

protected:

    /**
     * Function to perform cleanup
     */
    inline virtual void clear(void) = 0;

    /**
     * Function to create a binary functor
     */
    inline virtual ExpressionBinaryFunctor<T> *createBinaryFunctor(const BinaryFunctorType &binaryFunctorType)
    {
        return ExpressionBinaryFunctor<T>::create(binaryFunctorType);
    }

    /**
     * Function to create an expression functor
     * @param name the name of the ExpressionFunctor object to be instantiated
     */
    inline virtual ExpressionFunctor<T> *createFunctor(const std::string &name)
    {
        return ExpressionFunctor<T>::create(name);
    }

    /**
     * Function to create a unary functor
     */
    inline virtual ExpressionUnaryFunctor<T> *createUnaryFunctor(const UnaryFunctorType &unaryFunctorType)
    {
        return ExpressionUnaryFunctor<T>::create(unaryFunctorType);
    }

    /**
     * Function to evaluate the expression
     * @param pExpression a character string containing the expression
     */
    virtual bool evaluate(const char *pExpression, functional::Any &result)
    {
        bool bSuccess = (pExpression != nullptr);
        if (bSuccess)
        {
            initialize();

            int nextBinaryOperation = getNextBinaryOperation(0);
            bSuccess = parseBinaryOperation(pExpression, result, nextBinaryOperation);

            // now, pExpression should point to '\0', and parenthesis count should be zero
            if (m_parenthesisCount != 0 || *pExpression == ')')
            {
                if (m_errorType == ErrorType::None)
                    m_errorType = ErrorType::Parenthesis;

                bSuccess = false;
            }
            else if (!bSuccess || *pExpression != '\0')
            {
                if (m_errorType == ErrorType::None)
                    m_errorType = ErrorType::Invalid;

                bSuccess = false;
            }

            if (!bSuccess && m_pErrorPosition == nullptr)
                m_pErrorPosition = pExpression;
        }

        return bSuccess;
    }

public:

    /**
     * Get the last error position
     */
    inline virtual const char *getErrorPosition(void) const
    {
        return m_pErrorPosition;
    }

    /**
     * Get the last error code
     */
    inline virtual ErrorType getErrorType(void) const final
    {
        return m_errorType;
    }

protected:

    /**
     * Function to get the next binary operation in order of precedence as a function of the current binary
     * operation
     */
    virtual int getNextBinaryOperation(int currentBinaryOperation)
    {
        typedef BinaryFunctorType::Enum BinaryOp;

        // implement a state machine which defines operators in order of precedence
        int nextBinaryOperation = 0;
        if (currentBinaryOperation == 0)
            nextBinaryOperation = BinaryOp::LogicalOr;
        else if (currentBinaryOperation & BinaryOp::LogicalOr)
            nextBinaryOperation = BinaryOp::LogicalAnd;
        else if (currentBinaryOperation & BinaryOp::LogicalAnd)
            nextBinaryOperation = BinaryOp::BitwiseOr;
        else if (currentBinaryOperation & BinaryOp::BitwiseOr)
            nextBinaryOperation = BinaryOp::BitwiseXor;
        else if (currentBinaryOperation & BinaryOp::BitwiseXor)
            nextBinaryOperation = BinaryOp::BitwiseAnd;
        else if (currentBinaryOperation & BinaryOp::BitwiseAnd)
            nextBinaryOperation = BinaryOp::Equal | BinaryOp::NotEqual;
        else if (currentBinaryOperation & (BinaryOp::Equal | BinaryOp::NotEqual))
            nextBinaryOperation = BinaryOp::Less | BinaryOp::LessEqual | BinaryOp::Greater |
                                  BinaryOp::GreaterEqual;
        else if (currentBinaryOperation & (BinaryOp::Less | BinaryOp::LessEqual | BinaryOp::Greater |
                                           BinaryOp::GreaterEqual))
            nextBinaryOperation = BinaryOp::BitwiseLeftShift | BinaryOp::BitwiseRightShift;
        else if (currentBinaryOperation & (BinaryOp::BitwiseLeftShift | BinaryOp::BitwiseRightShift))
            nextBinaryOperation = BinaryOp::Addition | BinaryOp::Subtraction;
        else if (currentBinaryOperation & (BinaryOp::Addition | BinaryOp::Subtraction))
            nextBinaryOperation = BinaryOp::Division | BinaryOp::Modulo | BinaryOp::Multiplication;
        else if (currentBinaryOperation & (BinaryOp::Division | BinaryOp::Modulo | BinaryOp::Multiplication))
            nextBinaryOperation = 0;

        return nextBinaryOperation;
    }

public:

    /**
     * Initialization function
     */
    inline virtual bool initialize(void) override
    {
        m_errorType = ErrorType::None;
        m_pErrorPosition = nullptr;
        m_parenthesisCount = 0;

        return true;
    }

protected:

    /**
     * Function to parse expression for a binary operation
     * @param pExpression a character string containing the expression
     */
    virtual bool parseBinaryOperation(const char *&pExpression, functional::Any &lhs,
                                      int currentBinaryOperation)
    {
        functional::Any rhs;

        bool bSuccess;
        int nextBinaryOperation = getNextBinaryOperation(currentBinaryOperation);
        if (nextBinaryOperation == 0)
            bSuccess = parseUnaryOperation(pExpression, lhs);
        else
            bSuccess = parseBinaryOperation(pExpression, lhs, nextBinaryOperation);

        while (bSuccess)
        {
            // skip whitespace
            while (*pExpression == ' ' || *pExpression == '\t')
                ++pExpression;

            BinaryFunctorType binaryFunctorType(pExpression);
            if (binaryFunctorType & currentBinaryOperation)
            {
                ExpressionBinaryFunctor<T> *pBinaryFunctor = createBinaryFunctor(binaryFunctorType);
                if (pBinaryFunctor != nullptr)
                {
                    pExpression += std::string(binaryFunctorType).size();
                    if (nextBinaryOperation == 0)
                        bSuccess = parseUnaryOperation(pExpression, rhs);
                    else
                        bSuccess = parseBinaryOperation(pExpression, rhs, nextBinaryOperation);

                    if (bSuccess)
                    {
                        bSuccess = processBinaryOperation(pBinaryFunctor, lhs, rhs);
                        if (!bSuccess && m_pErrorPosition == nullptr)
                            m_pErrorPosition = pExpression;
                    }

                    if (pBinaryFunctor != nullptr)
                    {
                        delete pBinaryFunctor;
                        pBinaryFunctor = nullptr;
                    }
                }
            }
            else
                break;
        }

        return bSuccess;
    }

    /**
     * Function to parse the expression for a function call
     * @param pExpression a character string containing the expression
     */
    virtual bool parseFunction(const char *&pExpression, functional::Any &result)
    {
        bool bSuccess = (pExpression != nullptr);
        if (pExpression != nullptr)
        {
            std::regex criteria("(\\s*\\b([a-zA-Z_]\\w*)\\b\\s*\\().*");
            std::smatch match;
            std::string expression(std::move(pExpression));
            if (std::regex_match(expression, match, criteria))
            {
                ExpressionFunctor<T> *pFunction = createFunctor(match[2]);
                bSuccess = (pFunction != nullptr);
                if (bSuccess)
                {
                    std::vector<functional::Any> arguments;
                    ++m_parenthesisCount;
                    pExpression += match[1].length();
                    int charCount = 0, parenthesisCount = 1;
                    while (*pExpression && parenthesisCount > 0)
                    {
                        if (*pExpression == '(')
                            ++parenthesisCount;
                        else if (*pExpression == ')')
                            --parenthesisCount;

                        bool bComma = (*pExpression == ','), bParenthesis = (*pExpression == ')');
                        if ((parenthesisCount == 0 && bParenthesis) || (parenthesisCount == 1 && bComma))
                        {
                            const char *pSubExpression = pExpression - charCount;
                            if (pSubExpression != nullptr)
                            {
                                functional::Any argument = pFunction;
                                int nextBinaryOperation = getNextBinaryOperation(0);
                                bSuccess = parseBinaryOperation(pSubExpression, argument, nextBinaryOperation);
                                if (bSuccess)
                                    arguments.push_back(argument);
                                else
                                {
                                    if (m_pErrorPosition == nullptr)
                                        m_pErrorPosition = pSubExpression;

                                    break;
                                }
                            }

                            if (bParenthesis)
                            {
                                bSuccess = processFunction(pFunction, arguments, result);
                                if (!bSuccess)
                                {
                                    m_errorType = ErrorType::BadFunctionCall;
                                    if (m_pErrorPosition == nullptr)
                                        m_pErrorPosition = pSubExpression;
                                }
                            }
                            else if (*pSubExpression != ',' && *pSubExpression != ')')
                            {
                                bSuccess = false;
                                m_errorType = ErrorType::Invalid;
                                if (m_pErrorPosition == nullptr)
                                    m_pErrorPosition = pSubExpression;

                                break;
                            }

                            charCount = 0;
                        }
                        else
                            ++charCount;

                        ++pExpression;
                    }

                    if (parenthesisCount == 0)
                        --m_parenthesisCount;

                    if (pFunction != nullptr)
                    {
                        delete pFunction;
                        pFunction = nullptr;
                    }
                }
                else
                {
                    m_errorType = ErrorType::BadFunctionCall;
                    if (m_pErrorPosition == nullptr)
                        m_pErrorPosition = pExpression;
                }
            }
            else
                bSuccess = parseOperand(pExpression, result);
        }

        return bSuccess;
    }

    /**
     * Function to parse the expression for an operand/variable
     * @param pExpression a character string containing the expression
     */
    virtual bool parseOperand(const char *&pExpression, functional::Any &result)
    {
        bool bSuccess = true;

        // skip whitespace
        while (*pExpression == ' ' || *pExpression == '\t')
            ++pExpression;

        // first try to extract a number from the string
        bool bNumericOperand = processNumericOperand(pExpression, result);
        if (!bNumericOperand)
        {
            // try to exact a variable name...
            std::smatch match;
            std::regex criteria("\\b([a-zA-Z_]\\w*(\\s+\\w+)*)\\b(?!\\s*\\().*");
            std::string expression(std::move(pExpression));
            bSuccess = std::regex_match(expression, match, criteria);
            if (bSuccess)
            {
                bSuccess = processVariable(match[1], result);
                if (bSuccess)
                    pExpression += match[1].length();
                else
                {
                    m_errorType = ErrorType::Invalid;
                    if (m_pErrorPosition == nullptr)
                        m_pErrorPosition = pExpression;
                }
            }
        }

        return bSuccess;
    }

    /**
     * Function to parse expression for a unary operation
     * @param pExpression a character string containing the expression
     */
    virtual bool parseUnaryOperation(const char *&pExpression, functional::Any &result)
    {
        // skip whitespace
        while (*pExpression == ' ' || *pExpression == '\t')
            ++pExpression;

        // handle unary operators before parenthesis (or before number)
        bool bSuccess = true;
        UnaryFunctorType unaryFunctorType(pExpression);
        auto *pUnaryFunctor = createUnaryFunctor(unaryFunctorType);
        if (pUnaryFunctor != nullptr)
            bSuccess = parseUnaryOperation(++pExpression, result);

        // check if there is an open paren
        if (*pExpression == '(')
        {
            ++pExpression, ++m_parenthesisCount;
            int nextBinaryOperation = getNextBinaryOperation(0);
            bSuccess = parseBinaryOperation(pExpression, result, nextBinaryOperation);
            if (bSuccess)
            {
                // check if there is a closing paren
                if (*pExpression != ')')
                {
                    // unmatched opening parenthesis
                    m_errorType = ErrorType::Parenthesis;
                    if (m_pErrorPosition == nullptr)
                        m_pErrorPosition = pExpression;

                    bSuccess = false;
                }
                else
                {
                    ++pExpression;
                    --m_parenthesisCount;
                }
            }

            return bSuccess;
        }

        if (pUnaryFunctor == nullptr)
            bSuccess = parseFunction(pExpression, result);
        else
        {
            processUnaryOperation(pUnaryFunctor, result);

            if (pUnaryFunctor != nullptr)
            {
                delete pUnaryFunctor;
                pUnaryFunctor = nullptr;
            }
        }

        return bSuccess;
    }

    /**
     * Function to process a binary operation
     */
    virtual bool processBinaryOperation(ExpressionBinaryFunctor<T> *&pBinaryFunctor, functional::Any &lhs,
                                        functional::Any &rhs) = 0;

    /**
     * Function to process a function call
     */
    virtual bool processFunction(ExpressionFunctor<T> *&pFunction, std::vector<functional::Any> &arguments,
                                 functional::Any &result) = 0;

    /**
     * Function to process a numeric operand
     */
    virtual bool processNumericOperand(const char *&pExpression, functional::Any &result) = 0;

    /**
     * Function to process a unary operation
     */
    virtual bool processUnaryOperation(ExpressionUnaryFunctor<T> *&pUnaryFunctor, functional::Any &rhs) = 0;

    /**
     * Function to process a variable
     */
    virtual bool processVariable(const std::string &name, functional::Any &result) = 0;

public:

    /**
     * Read variables from expression
     * @param expression a string containing the mathematical expression to be evaluated
     */
    static std::set<std::string> readVariablesFromExpression(const std::string &expression)
    {
        std::set<std::string> variables;
        std::regex criteria("\\b([a-zA-Z_]\\w*(\\s+\\w+)*)\\b(?!\\s*\\()");
        std::sregex_token_iterator itRegex(expression.cbegin(), expression.cend(), criteria), itRegexEnd;
        while (itRegex != itRegexEnd)
        {
            if (itRegex->matched)
            {
                variables.insert(itRegex->str());
                ++itRegex;
            }
        }

        return variables;
    }

    /**
     * Function to perform variable substitution on an expression
     * @param expression              a string containing the mathematical expression to be evaluated
     * @param variableSubstitutionMap a map of variable-substitution pairs
     */
    static void substitute(std::string &expression, const tTokenMap &variableSubstitutionMap)
    {
        std::regex criteria("\\b([a-zA-Z_]\\w*(\\s+\\w+)*)\\b(?!\\s*\\()");
        size_t position = 0;
        while (true)
        {
            std::sregex_iterator itRegex(expression.cbegin() + position, expression.cend(), criteria), itRegexEnd;
            if (itRegex != itRegexEnd)
            {
                position += itRegex->position();
                auto &&itTokenPair = variableSubstitutionMap.find(itRegex->str());
                if (itTokenPair != variableSubstitutionMap.cend())
                {
                    auto &&substitution = itTokenPair->second;
                    if (expression.find(substitution, position) != position) // prevent recursive replacements
                        expression.replace(position, itRegex->length(), substitution);

                    position += substitution.length();
                }
                else
                    position += itRegex->length();

                continue;
            }

            break;
        }
    }

protected:

    /**
     * holds the enumerated error type, if one occurs (None by default)
     */
    ErrorType m_errorType;

    /**
     * list used to store numeric operands
     */
    std::list<T> m_numericOperands;

    /**
     * tracks the parenthesis count for balanced parenthesis check
     */
    int m_parenthesisCount;

    /**
     * stores the position at which an error, if any, occurred
     */
    const char *m_pErrorPosition;
};

}

}

#endif
