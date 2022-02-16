#include "complex_matrix_2d.h"
#include "testComplexMatrix2d.h"
#include "unitTestManager.h"

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testComplexMatrix2d",
                                                    &ComplexMatrix2dUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
ComplexMatrix2dUnitTest::ComplexMatrix2dUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
ComplexMatrix2dUnitTest *ComplexMatrix2dUnitTest::create(UnitTestManager *pUnitTestManager)
{
    ComplexMatrix2dUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new ComplexMatrix2dUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool ComplexMatrix2dUnitTest::execute(void)
{
    bool bSuccess = true;

    // TODO: implement this!

    return bSuccess;
}

}
