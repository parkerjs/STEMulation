#ifndef TEST_CHOLESKY_H
#define TEST_CHOLESKY_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Cholesky matrix decomposition class
 */
class CholeskyUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    CholeskyUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    CholeskyUnitTest(const CholeskyUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    CholeskyUnitTest(CholeskyUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~CholeskyUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    CholeskyUnitTest &operator = (const CholeskyUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    CholeskyUnitTest &operator = (CholeskyUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static CholeskyUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "CholeskyTest";
    }
};

}

#endif
