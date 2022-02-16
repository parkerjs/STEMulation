#include "balanced_expression_checker.h"
#include "testBalancedExpressionChecker.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace messaging;
using namespace utilities::expression;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest>unit_test_factory("testBalancedExpressionChecker",
                                                   &BalancedExpressionCheckerUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
BalancedExpressionCheckerUnitTest::BalancedExpressionCheckerUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
BalancedExpressionCheckerUnitTest *BalancedExpressionCheckerUnitTest::create(UnitTestManager *pUnitTestManager)
{
    BalancedExpressionCheckerUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new BalancedExpressionCheckerUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool BalancedExpressionCheckerUnitTest::execute(void)
{
    bool bSuccess = true; // assume success

    BalancedExpressionChecker checker;

    bSuccess = checker.isBalanced("()(){}{}{()}");
    bSuccess &= !checker.isBalanced("((({}{}))()");

    // TODO: implement this!

    std::cout << std::endl << (bSuccess ? "Test PASSED." : "Test FAILED.") << std::endl << std::endl;

    return bSuccess;
}

}
