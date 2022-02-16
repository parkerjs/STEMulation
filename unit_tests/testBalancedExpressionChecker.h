#ifndef TEST_BALANCED_EXPRESSION_CHECKER_H
#define TEST_BALANCED_EXPRESSION_CHECKER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for URL_Parser class
 */
class BalancedExpressionCheckerUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    BalancedExpressionCheckerUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    BalancedExpressionCheckerUnitTest(const BalancedExpressionCheckerUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    BalancedExpressionCheckerUnitTest(BalancedExpressionCheckerUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~BalancedExpressionCheckerUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    BalancedExpressionCheckerUnitTest &operator = (const BalancedExpressionCheckerUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    BalancedExpressionCheckerUnitTest &operator = (BalancedExpressionCheckerUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static BalancedExpressionCheckerUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "BalancedExpressionCheckerTest";
    }
};

}

#endif
