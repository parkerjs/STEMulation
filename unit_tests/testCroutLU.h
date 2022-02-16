#ifndef TEST_CROUT_LU_H
#define TEST_CROUT_LU_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Crout LU matrix decomposition class
 */
class CroutLU_UnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    CroutLU_UnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    CroutLU_UnitTest(const CroutLU_UnitTest &tester) = delete;

    /**
     * Move constructor
     */
    CroutLU_UnitTest(CroutLU_UnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~CroutLU_UnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    CroutLU_UnitTest &operator = (const CroutLU_UnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    CroutLU_UnitTest &operator = (CroutLU_UnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static CroutLU_UnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "CroutLU_Test";
    }
};

}

#endif
