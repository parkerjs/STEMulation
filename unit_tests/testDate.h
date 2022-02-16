#ifndef TEST_DATE_H
#define TEST_DATE_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Date class
 */
class DateUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    DateUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    DateUnitTest(const DateUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    DateUnitTest(DateUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~DateUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    DateUnitTest &operator = (const DateUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    DateUnitTest &operator = (DateUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static DateUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    virtual std::string getFactoryName(void) const override final
    {
        return "DateTest";
    }
};

}

#endif
