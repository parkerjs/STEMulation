#include "unitTest.h"

// using namespace declarations
using namespace messaging;

namespace unit_tests
{

/**
 * Constructor
 * @param dependencies a tuple of this object's required injection dependencies
 */
UnitTest::UnitTest(const tDependencies &dependencies)
: DependencyInjectableVirtualBaseInitializer(1, dependencies)
{
    auto *pPublisher = getDependency<Publisher *>();
    if (pPublisher != nullptr)
        pPublisher->addSubscriber(this);
}

/**
 * Constructor
 */
UnitTest::~UnitTest(void)
{

}

}
