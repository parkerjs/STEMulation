#include "numeric_matrix_2d.h"
#include "testNumericMatrix2d.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testNumericMatrix2d",
                                                    &NumericMatrix2dUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
NumericMatrix2dUnitTest::NumericMatrix2dUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
NumericMatrix2dUnitTest *NumericMatrix2dUnitTest::create(UnitTestManager *pUnitTestManager)
{
    NumericMatrix2dUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new NumericMatrix2dUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool NumericMatrix2dUnitTest::execute(void)
{
    bool bSuccess = true;

    // TODO: implement this!

    return bSuccess;
}

}

