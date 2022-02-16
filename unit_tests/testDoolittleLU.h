#ifndef TEST_DOOLITTLE_LU_H
#define TEST_DOOLITTLE_LU_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Doolittle LU matrix decomposition class
 */
class DoolittleLU_UnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    DoolittleLU_UnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    DoolittleLU_UnitTest(const DoolittleLU_UnitTest &tester) = delete;

    /**
     * Move constructor
     */
    DoolittleLU_UnitTest(DoolittleLU_UnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~DoolittleLU_UnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    DoolittleLU_UnitTest &operator = (const DoolittleLU_UnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    DoolittleLU_UnitTest &operator = (DoolittleLU_UnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static DoolittleLU_UnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "DoolittleLU_Test";
    }
};

}

#endif
