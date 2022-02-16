#ifndef TEST_DEPENDENCY_INJECTABLE_H
#define TEST_DEPENDENCY_INJECTABLE_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester to test dependency injection
 */
class DependencyInjectableUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    DependencyInjectableUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    DependencyInjectableUnitTest(const DependencyInjectableUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    DependencyInjectableUnitTest(DependencyInjectableUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~DependencyInjectableUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    DependencyInjectableUnitTest &operator = (const DependencyInjectableUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    DependencyInjectableUnitTest &operator = (DependencyInjectableUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static DependencyInjectableUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "DependencyInjectableTest";
    }
};

}

#endif
