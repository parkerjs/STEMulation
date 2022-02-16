#ifndef TEST_COMPLEX_MATRIX_2D_H
#define TEST_COMPLEX_MATRIX_2D_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for ComplexMatrix2d class
 */
class ComplexMatrix2dUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    ComplexMatrix2dUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    ComplexMatrix2dUnitTest(const ComplexMatrix2dUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    ComplexMatrix2dUnitTest(ComplexMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~ComplexMatrix2dUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    ComplexMatrix2dUnitTest &operator = (const ComplexMatrix2dUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    ComplexMatrix2dUnitTest &operator = (ComplexMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static ComplexMatrix2dUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "ComplexMatrix2dTest";
    }
};

}

#endif
