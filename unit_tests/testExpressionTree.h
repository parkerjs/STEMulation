#ifndef TEST_EXPRESSION_TREE_H
#define TEST_EXPRESSION_TREE_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for ExpressionTree class
 */
class ExpressionTreeUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    ExpressionTreeUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    ExpressionTreeUnitTest(const ExpressionTreeUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    ExpressionTreeUnitTest(ExpressionTreeUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~ExpressionTreeUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    ExpressionTreeUnitTest &operator = (const ExpressionTreeUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    ExpressionTreeUnitTest &operator = (ExpressionTreeUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static ExpressionTreeUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "ExpressionTreeTest";
    }
};

}

#endif
