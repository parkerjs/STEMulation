#include "bezier.h"
#include "testBezier.h"
#include "unitTestManager.h"
#include <iomanip>
#include <iostream>

// using namespace declarations
using namespace attributes::abstract;
using namespace math::curves;
using namespace messaging;

namespace unit_tests
{

// register factories...
static FactoryRegistrar<UnitTest> unit_test_factory("testBezier", &BezierUnitTest::create);

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
BezierUnitTest::BezierUnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies),
  UnitTest(dependencies)
{

}

/**
 * create() function
 * @param pPublisher a pointer to an instance of the publisher to which this object subscribes
 */
BezierUnitTest *BezierUnitTest::create(UnitTestManager *pUnitTestManager)
{
    BezierUnitTest *pUnitTest = nullptr;
    if (pUnitTestManager != nullptr)
    {
        auto &&dependencies = pUnitTestManager->getDependencies();
        std::get<Publisher *>(dependencies) = pUnitTestManager;
        pUnitTest = new BezierUnitTest(dependencies);
    }

    return pUnitTest;
}

/**
 * Execution function
 */
bool BezierUnitTest::execute(void)
{
    std::cout << "Starting unit test for Bezier Curve interpolator..." << std::endl << std::endl;

    BezierCurve<double, 3> curve;

    double x[] = { 2, 0, -1, 2 };
    double y[] = { 3, 5, -2, 1 };

    int numPoints = 100;
    std::vector<double> p(numPoints), q(numPoints);
    curve.calculate(x, p);
    curve.calculate(y, q);

    std::cout << std::setw(16) << "x," << std::setw(16) << "y," << std::endl;
    for (int i = 0; i < numPoints; ++i)
    {
        std::cout << std::setw(15) << p[i] << ",";
        std::cout << std::setw(15) << q[i] << "," << std::endl;
    }

    return true;
}

}
