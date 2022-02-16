#ifndef TEST_NUMERIC_MATRIX_ND_H
#define TEST_NUMERIC_MATRIX_ND_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for NumericMatrixNd class
 */
class NumericMatrixNdUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    NumericMatrixNdUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    NumericMatrixNdUnitTest(const NumericMatrixNdUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    NumericMatrixNdUnitTest(NumericMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~NumericMatrixNdUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    NumericMatrixNdUnitTest &operator = (const NumericMatrixNdUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    NumericMatrixNdUnitTest &operator = (NumericMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static NumericMatrixNdUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "NumericMatrixNdTest";
    }
};

}

#endif

