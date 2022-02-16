#include "matrix.h"
#include "testGeneralMatrixNd.h"
#include "unitTestManager.h"
#include <iostream>

// using namespace declarations
using namespace attributes::abstract;
using namespace math::linear_algebra::matrix;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testGeneralMatrixNd",
                                                    &GeneralMatrixNdUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
GeneralMatrixNdUnitTest::GeneralMatrixNdUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
GeneralMatrixNdUnitTest *GeneralMatrixNdUnitTest::create(UnitTestManager *pUnitTestManager)
{
    GeneralMatrixNdUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new GeneralMatrixNdUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool GeneralMatrixNdUnitTest::execute(void)
{
    bool bSuccess = true;

    // TODO: implement this!

    return bSuccess;
}

}
