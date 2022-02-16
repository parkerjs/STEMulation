#ifndef TEST_REAL_MATRIX_ND_H
#define TEST_REAL_MATRIX_ND_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for RealMatrixNd class
 */
class RealMatrixNdUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    RealMatrixNdUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    RealMatrixNdUnitTest(const RealMatrixNdUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    RealMatrixNdUnitTest(RealMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~RealMatrixNdUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    RealMatrixNdUnitTest &operator = (const RealMatrixNdUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    RealMatrixNdUnitTest &operator = (RealMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static RealMatrixNdUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "RealMatrixNdTest";
    }
};

}

#endif

