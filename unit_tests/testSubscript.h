#ifndef TEST_SUBSCRIPT_H
#define TEST_SUBSCRIPT_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Subscript class
 */
class SubscriptUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    SubscriptUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    SubscriptUnitTest(const SubscriptUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    SubscriptUnitTest(SubscriptUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~SubscriptUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    SubscriptUnitTest &operator = (const SubscriptUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    SubscriptUnitTest &operator = (SubscriptUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static SubscriptUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    virtual std::string getFactoryName(void) const override final
    {
        return "SubscriptUnitTest";
    }
};

}

#endif

