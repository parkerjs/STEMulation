#ifndef TEST_QR_H
#define TEST_QR_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for QR matrix decomposition class
 */
class QR_UnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    QR_UnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    QR_UnitTest(const QR_UnitTest &tester) = delete;

    /**
     * Move constructor
     */
    QR_UnitTest(QR_UnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~QR_UnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    QR_UnitTest &operator = (const QR_UnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    QR_UnitTest &operator = (QR_UnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static QR_UnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "QR_Test";
    }
};

}

#endif
