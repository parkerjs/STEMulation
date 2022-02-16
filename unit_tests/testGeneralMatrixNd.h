#ifndef TEST_GENERAL_MATRIX_ND_H
#define TEST_GENERAL_MATRIX_ND_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for GeneralMatrixNd class
 */
class GeneralMatrixNdUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    GeneralMatrixNdUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    GeneralMatrixNdUnitTest(const GeneralMatrixNdUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    GeneralMatrixNdUnitTest(GeneralMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~GeneralMatrixNdUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    GeneralMatrixNdUnitTest &operator = (const GeneralMatrixNdUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    GeneralMatrixNdUnitTest &operator = (GeneralMatrixNdUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static GeneralMatrixNdUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "GeneralMatrixNdTest";
    }
};

}

#endif

