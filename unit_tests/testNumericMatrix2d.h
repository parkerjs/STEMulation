#ifndef TEST_NUMERIC_MATRIX_2D_H
#define TEST_NUMERIC_MATRIX_2D_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for NumericMatrix2d class
 */
class NumericMatrix2dUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    NumericMatrix2dUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    NumericMatrix2dUnitTest(const NumericMatrix2dUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    NumericMatrix2dUnitTest(NumericMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~NumericMatrix2dUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    NumericMatrix2dUnitTest &operator = (const NumericMatrix2dUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    NumericMatrix2dUnitTest &operator = (NumericMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static NumericMatrix2dUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "NumericMatrix2dTest";
    }
};

}

#endif
