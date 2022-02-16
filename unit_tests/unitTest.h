#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include "dependency_injectable.h"
#include "executable.h"
#include "factory_constructible.h"
#include "publisher.h"
#include "subscriber.h"

namespace unit_tests
{

/**
 * Base class for derived unit test classes
 */
class UnitTest
: virtual public attributes::concrete::DependencyInjectable<messaging::Publisher *>,
  public attributes::interfaces::Executable,
  public attributes::abstract::FactoryConstructible<UnitTest>,
  public messaging::Subscriber
{
public:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    UnitTest(const tDependencies &dependencies);

protected:

    /**
     * Copy constructor
     */
    UnitTest(const UnitTest &tester) = delete;

    /**
     * Move constructor
     */
    UnitTest(UnitTest &&tester) = delete;

public:

    /**
     * Constructor
     */
    virtual ~UnitTest(void);

protected:

    /**
     * Copy assignment operator
     */
    UnitTest &operator = (const UnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    UnitTest &operator = (UnitTest &&tester) = delete;
};

}

#endif
