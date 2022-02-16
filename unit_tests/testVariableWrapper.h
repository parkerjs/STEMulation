#ifndef TEST_VARIABLE_WRAPPER_H
#define TEST_VARIABLE_WRAPPER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for VariableWrapper class
 */
class VariableWrapperUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    VariableWrapperUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    VariableWrapperUnitTest(const VariableWrapperUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    VariableWrapperUnitTest(VariableWrapperUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~VariableWrapperUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    VariableWrapperUnitTest &operator = (const VariableWrapperUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    VariableWrapperUnitTest &operator = (VariableWrapperUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static VariableWrapperUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "VariableWrapperTest";
    }
};

}

#endif
