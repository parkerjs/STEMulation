#include "matrix.h"
#include "testRealMatrixNd.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix;
using namespace math::sets;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testRealMatrixNd",
                                                    &RealMatrixNdUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
RealMatrixNdUnitTest::RealMatrixNdUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
RealMatrixNdUnitTest *RealMatrixNdUnitTest::create(UnitTestManager *pUnitTestManager)
{
    RealMatrixNdUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new RealMatrixNdUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool RealMatrixNdUnitTest::execute(void)
{
    bool bSuccess = true;

    // TODO: implement this!

    return bSuccess;
}

}
