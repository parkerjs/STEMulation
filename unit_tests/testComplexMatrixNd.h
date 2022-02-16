#ifndef TEST_COMPLEX_MATRIX_ND_H
#define TEST_COMPLEX_MATRIX_ND_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for ComplexMatrixNd class
 */
class ComplexMatrixNdUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    ComplexMatrixNdUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    ComplexMatrixNdUnitTest(const ComplexMatrixNdUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    ComplexMatrixNdUnitTest(ComplexMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~ComplexMatrixNdUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    ComplexMatrixNdUnitTest &operator = (const ComplexMatrixNdUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    ComplexMatrixNdUnitTest &operator = (ComplexMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static ComplexMatrixNdUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "ComplexMatrixNdTest";
    }
};

}

#endif
