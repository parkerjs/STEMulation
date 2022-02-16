#ifndef TEST_GENERAL_MATRIX_2D_H
#define TEST_GENERAL_MATRIX_2D_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for GeneralMatrix2d class
 */
class GeneralMatrix2dUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    GeneralMatrix2dUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    GeneralMatrix2dUnitTest(const GeneralMatrix2dUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    GeneralMatrix2dUnitTest(GeneralMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~GeneralMatrix2dUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    GeneralMatrix2dUnitTest &operator = (const GeneralMatrix2dUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    GeneralMatrix2dUnitTest &operator = (GeneralMatrix2dUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static GeneralMatrix2dUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "GeneralMatrix2dTest";
    }
};

}

#endif

