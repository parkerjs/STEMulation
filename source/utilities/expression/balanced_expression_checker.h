#ifndef BALANCED_EXPRESSION_CHECKER_H
#define BALANCED_EXPRESSION_CHECKER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <string.h>

namespace utilities
{

namespace expression
{

/**
 * This class checks an expression for balanced pairs of opening and closing brackets
 */
class BalancedExpressionChecker
{
public:

    /**
     * Constructor
     */
    BalancedExpressionChecker(void)
    : m_bracketPairsMap({ { '{', '}' }, { '[', ']' }, { '(', ')' } })
    {

    }

    /**
     * Constructor
     * @param bracketPairsMap a map of user-defined opening, closing bracket pairs
     */
    BalancedExpressionChecker(const std::map<char, char> &bracketPairsMap)
    : m_bracketPairsMap(bracketPairsMap)
    {

    }

    /**
     * Copy constructor
     */
    BalancedExpressionChecker(const BalancedExpressionChecker &checker)
    {
        operator = (checker);
    }

    /**
     * Move constructor
     */
    BalancedExpressionChecker(BalancedExpressionChecker &&checker)
    {
        operator = (std::move(checker));
    }

    /**
     * Destructor
     */
    virtual ~BalancedExpressionChecker(void)
    {

    }

    /**
     * Copy assignment operator
     */
    BalancedExpressionChecker &operator = (const BalancedExpressionChecker &checker)
    {
        if (&checker != this)
        {
            m_bracketPairsMap = checker.m_bracketPairsMap;
        }

        return *this;
    }

    /**
     * Move assignment operator
     */
    BalancedExpressionChecker &operator = (BalancedExpressionChecker &&checker)
    {
        if (&checker != this)
        {
            m_bracketPairsMap = std::move(checker.m_bracketPairsMap);
        }

        return *this;
    }

    /**
     * Get this object's map of opening, closing bracket pairs
     */
    inline virtual std::map<char, char> &getBracketPairsMap(void) final
    {
        return m_bracketPairsMap;
    }

    /**
     * Finds the positions of the next matching pair of opening and closing brackets within the expression
     * bounded by the beginning and ending iterators; if a matching pair is found, function returns true and
     * the corresponding positions are returned via the function's reference arguments
     * @param itBegin an iterator to the beginning of the string
     * @param itEnd   an iterator to the end of the string
     */
    template<typename iterator>
    bool getNextMatchingBracketPair(iterator &itBegin, iterator &itEnd) const
    {
        std::stack<char> bracketStack;
        auto it = std::find_if(itBegin, itEnd, [this] (auto &&ch) { return this->isOpeningBracket(ch); });
        if (it != itEnd)
        {
            auto posOpeningBrace = std::distance(itBegin, it);
            bracketStack.push(*it);
            while (it != itEnd && !bracketStack.empty())
            {
                if (isOpeningBracket(*++it))
                    bracketStack.push(*it);
                else if (isClosingBracket(*it))
                {
                    if (isMatchingBracketPair(bracketStack.top(), *it))
                        bracketStack.pop();
                    else
                        return false; // unbalanced expression detected
                }
            }

            if (bracketStack.empty())
            {
                itBegin += posOpeningBrace;
                itEnd = it;

                return true;
            }
        }

        return false;
    }

    /**
     * Function to check the expression for balanced pairs of brackets
     */
    inline virtual bool isBalanced(const std::string &expression) const final
    {
        return isBalanced(expression.c_str());
    }

    /**
     * Function to check the expression for balanced pairs of brackets
     */
    inline virtual bool isBalanced(const char *pExpression) const final
    {
        return isBalanced(pExpression, pExpression + strlen(pExpression));
    }

    /**
     * Function to check the expression bounded by the beginning and ending iterators for balanced pairs of
     * brackets
     * @param itBegin an iterator to the beginning of the string
     * @param itEnd   an iterator to the end of the string
     */
    template<typename iterator>
    bool isBalanced(iterator itBegin, iterator itEnd) const
    {
        std::stack<char> bracketStack;
        for (auto &&it = itBegin; it != itEnd; ++it)
        {
            // check for opening brackets
            if (isOpeningBracket(*it))
                bracketStack.push(*it);

            // check for closing brackets
            if (isClosingBracket(*it))
            {
                if (bracketStack.empty() || !isMatchingBracketPair(bracketStack.top(), *it))
                    return false; // unbalanced expression detected
                else
                    bracketStack.pop();
            }
        }

        return bracketStack.empty(); // if stack is empty, then brackets within expression are balanced
    }

protected:

    /**
     * Function to test for closing bracket
     */
    inline virtual bool isClosingBracket(char ch) const final
    {
        return std::find_if(m_bracketPairsMap.cbegin(),
                            m_bracketPairsMap.cend(),
                            [&ch] (auto &&bracketPair)
                            { return ch == bracketPair.second; }) != m_bracketPairsMap.cend();
    }

    /**
     * Function to test for matching bracket pair
     */
    inline virtual bool isMatchingBracketPair(char ch1, char ch2) const final
    {
        return std::find_if(m_bracketPairsMap.cbegin(),
                            m_bracketPairsMap.cend(),
                            [&ch1, &ch2] (auto &&bracketPair)
                            { return ch1 == bracketPair.first &&
                                     ch2 == bracketPair.second; }) != m_bracketPairsMap.cend();
    }

    /**
     * Function to test for opening bracket
     */
    inline virtual bool isOpeningBracket(char ch) const final
    {
        return std::find_if(m_bracketPairsMap.cbegin(),
                            m_bracketPairsMap.cend(),
                            [&ch] (auto &&bracketPair)
                            { return ch == bracketPair.first; }) != m_bracketPairsMap.cend();
    }

public:

    /**
     * Set this object's map of opening, closing bracket pairs
     * @param bracketPairsMap a map of user-defined opening, closing bracket pairs
     */
    inline virtual void setBracketPairsMap(const std::map<char, char> &bracketPairsMap) final
    {
        m_bracketPairsMap = bracketPairsMap;
    }

protected:

    /**
     * map of opening, closing bracket pairs
     */
    std::map<char, char> m_bracketPairsMap;
};

}

}

#endif
