#include "matrix.h"
#include "testNumericMatrixNd.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testNumericMatrixNd",
                                                    &NumericMatrixNdUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
NumericMatrixNdUnitTest::NumericMatrixNdUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
NumericMatrixNdUnitTest *NumericMatrixNdUnitTest::create(UnitTestManager *pUnitTestManager)
{
    NumericMatrixNdUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new NumericMatrixNdUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool NumericMatrixNdUnitTest::execute(void)
{
    bool bSuccess = true;

    // TODO: implement this!

    return bSuccess;
}

}
