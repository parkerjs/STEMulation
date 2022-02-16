#ifndef TEST_BEZIER_H
#define TEST_BEZIER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for Bezier class
 */
class BezierUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    BezierUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    BezierUnitTest(const BezierUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    BezierUnitTest(BezierUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~BezierUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    BezierUnitTest &operator = (const BezierUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    BezierUnitTest &operator = (BezierUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static BezierUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "BezierCurveTest";
    }
};

}

#endif
