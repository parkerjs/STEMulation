#ifndef TEST_DIRECTORY_TRAVERSER_H
#define TEST_DIRECTORY_TRAVERSER_H

#include "unitTest.h"

namespace unit_tests
{

// forward declarations
class UnitTestManager;

/**
 * Unit tester for DirectoryTraverser class
 */
class DirectoryTraverserUnitTest final
: public UnitTest
{
private:

    /**
     * Constructor
     * @param dependencies a tuple of this object's required injection dependencies
     */
    DirectoryTraverserUnitTest(const tDependencies &dependencies);

    /**
     * Copy constructor
     */
    DirectoryTraverserUnitTest(const DirectoryTraverserUnitTest &tester) = delete;

    /**
     * Move constructor
     */
    DirectoryTraverserUnitTest(DirectoryTraverserUnitTest &&tester) = delete;

public:

    /**
     * Destructor
     */
    virtual ~DirectoryTraverserUnitTest(void) override
    {

    }

private:

    /**
     * Copy assignment operator
     */
    DirectoryTraverserUnitTest &operator = (const DirectoryTraverserUnitTest &tester) = delete;

    /**
     * Move assignment operator
     */
    DirectoryTraverserUnitTest &operator = (DirectoryTraverserUnitTest &&tester) = delete;

public:

    /**
     * create() function
     * @param pUnitTestManager a pointer to an instance of UnitTestManager
     */
    static DirectoryTraverserUnitTest *create(UnitTestManager *pUnitTestManager);

    /**
     * Execution function
     */
    virtual bool execute(void) override final;

    /**
     * Get the factory name of this constructible
     */
    inline virtual std::string getFactoryName(void) const override final
    {
        return "DirectoryTraverserTest";
    }
};

}

#endif
