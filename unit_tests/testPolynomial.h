#ifndef TEST_POLYNOMIAL_H
#define TEST_POLYNOMIAL_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Polynomial class
 */
class PolynomialUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    PolynomialUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    PolynomialUnitTest(const PolynomialUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    PolynomialUnitTest(PolynomialUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~PolynomialUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    PolynomialUnitTest &operator = (const PolynomialUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    PolynomialUnitTest &operator = (PolynomialUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static PolynomialUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "PolynomialTest";
    }
};

}

#endif
