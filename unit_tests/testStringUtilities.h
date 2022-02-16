#ifndef TEST_STRING_UTILITIES_H
#define TEST_STRING_UTILITIES_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for StringUtilities class
 */
class StringUtilitiesUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    StringUtilitiesUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    StringUtilitiesUnitTest(const StringUtilitiesUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    StringUtilitiesUnitTest(StringUtilitiesUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~StringUtilitiesUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    StringUtilitiesUnitTest &operator = (const StringUtilitiesUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    StringUtilitiesUnitTest &operator = (StringUtilitiesUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static StringUtilitiesUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "StringUtilitiesTest";
    }
};

}

#endif
