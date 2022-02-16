#ifndef TEST_STATISTICAL_H
#define TEST_STATISTICAL_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for statistical calculations
 */
class StatisticalUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    StatisticalUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    StatisticalUnitTest(const StatisticalUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    StatisticalUnitTest(StatisticalUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~StatisticalUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    StatisticalUnitTest &operator = (const StatisticalUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    StatisticalUnitTest &operator = (StatisticalUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static StatisticalUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "StatisticalTest";
    }
};

}

#endif
