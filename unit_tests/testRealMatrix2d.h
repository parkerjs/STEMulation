#ifndef TEST_REAL_MATRIX_2D_H
#define TEST_REAL_MATRIX_2D_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for RealMatrix2d class
 */
class RealMatrix2dUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    RealMatrix2dUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    RealMatrix2dUnitTest(const RealMatrix2dUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    RealMatrix2dUnitTest(RealMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~RealMatrix2dUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    RealMatrix2dUnitTest &operator = (const RealMatrix2dUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    RealMatrix2dUnitTest &operator = (RealMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static RealMatrix2dUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "RealMatrix2dTest";
    }
};

}

#endif

