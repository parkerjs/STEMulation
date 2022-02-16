#ifndef TEST_TRIDIAGONAL_LU_H
#define TEST_TRIDIAGONAL_LU_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Tridiagonal LU matrix decomposition class
 */
class TridiagonalLU_UnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    TridiagonalLU_UnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    TridiagonalLU_UnitTest(const TridiagonalLU_UnitTest &tester) = delete;

    /**
     * Move constructor
     */
    TridiagonalLU_UnitTest(TridiagonalLU_UnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~TridiagonalLU_UnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    TridiagonalLU_UnitTest &operator = (const TridiagonalLU_UnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    TridiagonalLU_UnitTest &operator = (TridiagonalLU_UnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static TridiagonalLU_UnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "TridiagonalLU_Test";
    }
};

}

#endif
